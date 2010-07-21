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
#include <string.h>
#include <stdio.h>
#include "fatwrapper.h"
#include "boot.h"
#include "globals.h"
#include "general.h"
#include "nds_loader_arm9.h"

#define LF_LOADARM9				0x00000001
#define LF_LOADARM7				0x00000002
#define LF_CLEARMEM				0x00000004
#define LF_LOADGBFS				0x00000008
#define LF_MEMDIRECT			0x00000010
#define LF_NOVBLANK				0x00000020
#define LF_GBABOOT				0x00000040

void exec(char *command, bool gbaBoot, bool memBoot)
{
	strlwr(command);
	
	// "..." ignored atm
	// Set up VRAM, to put the binary in. 
	//  Bank A might be used for text out (will be used by the stub)
	//  Bank B,D to carry over param&binary
    vramSetMainBanks(   VRAM_A_MAIN_BG_0x6000000,       
                        VRAM_B_MAIN_BG_0x6020000,      
                        VRAM_C_LCD,  
                        VRAM_D_MAIN_BG_0x6040000   
                        );    
						
	//set the video mode
    videoSetMode(  MODE_0_2D | DISPLAY_BG0_ACTIVE );
	// black backdrop
	BG_PALETTE[0]=RGB15(0,0,0);
	BG0_CR = BG_MAP_BASE(31);
	BG_PALETTE[255] = RGB15(31,31,31);
	// set up debug output 
	
	// you can load the stub from whatever place: gbfs,wifi,fat & whatever
	DRAGON_chdir("/");
	char cCommand[256];
	sprintf(cCommand, "%sexec_stub.bin", d_res);
	DRAGON_FILE *stub = DRAGON_fopen(cCommand,"r");
	u32 tLen = DRAGON_flength(stub);
	
	long *buffer = (long *)safeMalloc(tLen);
	
	DRAGON_fread(buffer,tLen,1,stub) ;
	DRAGON_fclose(stub) ;
	
	dldiPatchLoader((data_t *)buffer, tLen);
	
	// we copy wordwise, as bytewise write access to VRAM is a bad idea
	u32 i ;
	long *target = (long *)0x6020000 ;
	for (i=0;i<tLen >> 2;i++) {
		target[i] = buffer[i] ;
	} ;
	
	if(memBoot)
	{
		*(vu32 *)0x0605FFFC = (unsigned int)command;
	}
	else
	{
		char *exec_filename = (char *)0x0605FC00 ;
		// don't use str* functions as we can't write bytewise to VRAM
		for (i=0;i<=(strlen(command)+1)/4;i++) {
			((vu32 *)exec_filename)[i] = ((vu32 *)command)[i] ;
		} ;
		*(vu32 *)0x0605FFFC = 0x0605FC00 ;
	}

	u32 tBoot = LF_LOADARM9 | LF_LOADARM7 | LF_CLEARMEM | LF_LOADGBFS;
	
	if(memBoot)
		tBoot |= LF_MEMDIRECT;
	else
	{
		if(gbaBoot)
			tBoot |= LF_GBABOOT;
	}
	
	*(unsigned long *)0x0605FFF8 = tBoot;
	
	typedef void (*stub_type)(void) ;

	stub_type stb = (stub_type)0x06020000 ;
	stb() ;
	
} ;

