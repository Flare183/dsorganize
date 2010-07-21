/***************************************************************************
 *                                                                         *
 *  This file is part of DSOrganize.                                       *
 *                                                                         *
 *  DSOrganize is free software: you can redistribute it and/or modify     *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  DSOrganize is distributed in the hope that it will be useful,          *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with DSOrganize.  If not, see <http://www.gnu.org/licenses/>.    *
 *                                                                         *
 ***************************************************************************/
 
#include <nds.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libfb/libcommon.h>
#include "autopatch.h"
#include "nds_loader_arm9.h"
#include "settings.h"
#include "globals.h"
#include "fatwrapper.h"
#include "general.h"
#include "colors.h"
#include "language.h"
#include "font_arial_11.h"

static const data_t dldiMagicString[] = "\xED\xA5\x8D\xBF Chishm";	// Normal DLDI file

static addr_t readAddr (data_t *mem, addr_t offset) {
	return ((addr_t*)mem)[offset/sizeof(addr_t)];
}

static void writeAddr (data_t *mem, addr_t offset, addr_t value) {
	((addr_t*)mem)[offset/sizeof(addr_t)] = value;
}

static addr_t fileFind(DRAGON_FILE *df, const data_t* search, size_t dataLen, size_t searchLen) 
{
	int *dataChunk = (int *)safeMalloc(CHUNK_SIZE);
	int searchChunk = ((const int*)search)[0];
	int offset = 0;
	
	DRAGON_fseek(df, 0, SEEK_SET);
	
	while(!DRAGON_feof(df))
	{
		memset(dataChunk, 0, CHUNK_SIZE);
		
		addr_t i;
		addr_t dataChunkEnd = DRAGON_fread(dataChunk, 1, CHUNK_SIZE, df) / sizeof(int);
		
		for(i = 0; i < dataChunkEnd; i++)
		{
			if (dataChunk[i] == searchChunk) 
			{
				if (((i+offset)*sizeof(int) + searchLen) > dataLen) 
					return -1;
				if (memcmp (&dataChunk[i], search, searchLen) == 0)
					return (i+offset)*sizeof(int);
			}
		}
		
		if(!DRAGON_feof(df))
		{
			offset += dataChunkEnd - 16;		
			DRAGON_fseek(df, (-16 * sizeof(int)), SEEK_CUR); // overlap just in case	
		}
	}
	
	return -1;
}

void patchFile(char *fName)
{
	data_t *pAH = NULL;
	data_t *pDH = NULL;
	char *dldiPath = getDLDI();
	bool needsFree = false;
	
	fb_swapBuffers();
	bg_swapBuffers();
	bg_swapBuffers();
	
	fb_setClipping(5,5,250,187);
	
	setColor(genericTextColor);			
	setFont(font_arial_11);
	fb_dispString(0,0, l_autopatch);
	fb_swapBuffers();
	
	pDH = (data_t*)(((u32*)(&_io_dldi)) -24);
	
	if (*((u32*)(pDH + DO_ioType)) == DEVICE_TYPE_DLDI) // no built in dldi, load from scratch
	{	
		if(strlen(dldiPath) == 0) // No DLDI patch
			return;
		
		// load and then present
		
		DRAGON_chdir(d_res);
		
		DRAGON_FILE *df = DRAGON_fopen(dldiPath, "r");
		
		u32 patchLen = DRAGON_flength(df);
		pDH = (data_t*)safeMalloc(PATCH_SIZE);
		memset(pDH, 0, PATCH_SIZE);
		
		DRAGON_fread(pDH, 1, patchLen, df);
		DRAGON_fclose(df);
		
		if(strcmp((char *)dldiMagicString, (char*)&pDH[DO_magicString]) != 0) 
		{
			free(pDH);
			return;
		}
		if(pDH[DO_version] != DLDI_VERSION) 
		{
			free(pDH);
			return;
		}
		
		needsFree = true;
	}
	
	addr_t memOffset;			// Offset of DLDI after the file is loaded into memory
	addr_t patchOffset;			// Position of patch destination in the file
	addr_t relocationOffset;	// Value added to all offsets within the patch to fix it properly
	addr_t ddmemOffset;			// Original offset used in the DLDI file
	addr_t ddmemStart;			// Start of range that offsets can be in the DLDI file
	addr_t ddmemEnd;			// End of range that offsets can be in the DLDI file
	addr_t ddmemSize;			// Size of range that offsets can be in the DLDI file
	
	size_t dldiFileSize = 0;    // lol comment
	addr_t addrIter;			// lol comment
	
	DRAGON_chdir("/");
	DRAGON_FILE *ndsFile = DRAGON_fopen(fName, "r");
	
	patchOffset = fileFind(ndsFile, dldiMagicString, DRAGON_flength(ndsFile), sizeof(dldiMagicString));
	
	if(patchOffset < 0) 
	{
		// does not have a DLDI section
		
		free(pDH);
		DRAGON_fclose(ndsFile);
		
		return;
	}
	else
	{
		pAH = (data_t*)safeMalloc(PATCH_SIZE);
		memset(pAH, 0, PATCH_SIZE);
		
		DRAGON_fseek(ndsFile, patchOffset, SEEK_SET);
		DRAGON_fread(pAH, 1, PATCH_SIZE, ndsFile);
		DRAGON_fclose(ndsFile);
	}
	
	if (pDH[DO_driverSize] > pAH[DO_allocatedSpace]) 
	{
		free(pAH);
		free(pDH);
		
		return;
	}
	
	if(*(int *)&pAH[DO_ioType] == *(int *)&pDH[DO_ioType])
	{	
		free(pAH);
		free(pDH);
		
		return;
	}
	
	// we've passed all the checks, time to patch
	
	dldiFileSize = 1 << pDH[DO_driverSize];

	memOffset = readAddr (pAH, DO_text_start);
	
	if(memOffset == 0) 
		memOffset = readAddr (pAH, DO_startup) - DO_code;
	
	ddmemOffset = readAddr (pDH, DO_text_start);
	relocationOffset = memOffset - ddmemOffset;
	
	ddmemStart = readAddr (pDH, DO_text_start);
	ddmemSize = (1 << pDH[DO_driverSize]);
	ddmemEnd = ddmemStart + ddmemSize;

	// Remember how much space is actually reserved
	pDH[DO_allocatedSpace] = pAH[DO_allocatedSpace];
	// Copy the DLDI patch into the application
	memcpy (pAH, pDH, dldiFileSize);

	// Fix the section pointers in the header
	writeAddr (pAH, DO_text_start, readAddr (pAH, DO_text_start) + relocationOffset);
	writeAddr (pAH, DO_data_end, readAddr (pAH, DO_data_end) + relocationOffset);
	writeAddr (pAH, DO_glue_start, readAddr (pAH, DO_glue_start) + relocationOffset);
	writeAddr (pAH, DO_glue_end, readAddr (pAH, DO_glue_end) + relocationOffset);
	writeAddr (pAH, DO_got_start, readAddr (pAH, DO_got_start) + relocationOffset);
	writeAddr (pAH, DO_got_end, readAddr (pAH, DO_got_end) + relocationOffset);
	writeAddr (pAH, DO_bss_start, readAddr (pAH, DO_bss_start) + relocationOffset);
	writeAddr (pAH, DO_bss_end, readAddr (pAH, DO_bss_end) + relocationOffset);
	// Fix the function pointers in the header
	writeAddr (pAH, DO_startup, readAddr (pAH, DO_startup) + relocationOffset);
	writeAddr (pAH, DO_isInserted, readAddr (pAH, DO_isInserted) + relocationOffset);
	writeAddr (pAH, DO_readSectors, readAddr (pAH, DO_readSectors) + relocationOffset);
	writeAddr (pAH, DO_writeSectors, readAddr (pAH, DO_writeSectors) + relocationOffset);
	writeAddr (pAH, DO_clearStatus, readAddr (pAH, DO_clearStatus) + relocationOffset);
	writeAddr (pAH, DO_shutdown, readAddr (pAH, DO_shutdown) + relocationOffset);

	if (pDH[DO_fixSections] & FIX_ALL) { 
		// Search through and fix pointers within the data section of the file
		for (addrIter = (readAddr(pDH, DO_text_start) - ddmemStart); addrIter < (readAddr(pDH, DO_data_end) - ddmemStart); addrIter++) {
			if ((ddmemStart <= readAddr(pAH, addrIter)) && (readAddr(pAH, addrIter) < ddmemEnd)) {
				writeAddr (pAH, addrIter, readAddr(pAH, addrIter) + relocationOffset);
			}
		}
	}

	if (pDH[DO_fixSections] & FIX_GLUE) { 
		// Search through and fix pointers within the glue section of the file
		for (addrIter = (readAddr(pDH, DO_glue_start) - ddmemStart); addrIter < (readAddr(pDH, DO_glue_end) - ddmemStart); addrIter++) {
			if ((ddmemStart <= readAddr(pAH, addrIter)) && (readAddr(pAH, addrIter) < ddmemEnd)) {
				writeAddr (pAH, addrIter, readAddr(pAH, addrIter) + relocationOffset);
			}
		}
	}

	if (pDH[DO_fixSections] & FIX_GOT) { 
		// Search through and fix pointers within the Global Offset Table section of the file
		for (addrIter = (readAddr(pDH, DO_got_start) - ddmemStart); addrIter < (readAddr(pDH, DO_got_end) - ddmemStart); addrIter++) {
			if ((ddmemStart <= readAddr(pAH, addrIter)) && (readAddr(pAH, addrIter) < ddmemEnd)) {
				writeAddr (pAH, addrIter, readAddr(pAH, addrIter) + relocationOffset);
			}
		}
	}

	if (pDH[DO_fixSections] & FIX_BSS) { 
		// Initialise the BSS to 0
		memset (&pAH[readAddr(pDH, DO_bss_start) - ddmemStart] , 0, readAddr(pDH, DO_bss_end) - readAddr(pDH, DO_bss_start));
	}
	
	// write back
	
	ndsFile = DRAGON_fopen(fName, "r+");
	DRAGON_fseek(ndsFile, patchOffset, SEEK_SET);
	DRAGON_fwrite(pAH, 1, dldiFileSize, ndsFile);
	DRAGON_fclose(ndsFile);
	
	if(needsFree)
		free(pDH); // we are just gunna boot anyway, but this is proper
	free(pAH);
}
