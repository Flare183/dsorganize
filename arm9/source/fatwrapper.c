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
#include <stdlib.h>
#include <string.h>
#include <libfat.h>
#include "general.h"
#include "fatwrapper.h"
#include "language.h"

static u32 curFiles[MAX_FILES];

bool DRAGON_InitFiles()
{
	memset(curFiles, 0, MAX_FILES * sizeof(u32));
	return FAT_InitFiles();
}

bool DRAGON_FreeFiles()
{
	return FAT_FreeFiles();
}

char *DRAGON_tryingInterface()
{
	return FAT_tryingInterface();
}

bool DRAGON_chdir(const char* path)
{
	return FAT_chdir(path);
}

int DRAGON_mkdir(const char* path)
{
	return FAT_mkdir(path);
}

int DRAGON_FileExists(const char* filename)
{
	return FAT_FileExists(filename);
}

DRAGON_FILE* DRAGON_fopen(const char* path, const char* mode)
{
	DRAGON_FILE *df = (DRAGON_FILE *)safeMalloc(sizeof(DRAGON_FILE));
	
	if(!df)
		return NULL;
	
	int i;
	bool found = false;
	for(i = 0;i < MAX_FILES; i++)
	{
		if(curFiles[i] == 0)
		{
			found = true;
			curFiles[i] = (u32)df;
			
			break;
		}
	}
	
	if(!found)
	{
		fatalHalt(l_outoffiles);
		
		return NULL;
	}
	
	memset(df, 0, sizeof(DRAGON_FILE));
	
	FAT_FILE *fp = FAT_fopen(path, mode);
	
	df->firstCluster = fp->firstCluster;
	df->chanceOfUnicode = 0;
	df->fp = (void *)fp;	
	df->cacheEnabled = false;
	
	return df;
}

int DRAGON_fseek(DRAGON_FILE* df, s32 offset, int origin)
{
	if(!df)
		return 0;
	if(!df->fp)
		return 0;
	
	if(df->cacheEnabled)
	{
		// figure out how many bytes we actually have to play with
		u32 totalBytes = 0;
		
		if(df->leadCursor > df->lagCursor)
		{
			// normal subtraction to find length
			totalBytes = df->leadCursor - df->lagCursor;
		}
		else
		{
			// we've looped around
			totalBytes = CACHE_SIZE - df->lagCursor;
			totalBytes += df->leadCursor;
		}
		
		// now find out the relative seek position of the data
		int relPos = 0;
		
		switch(origin)
		{
			case SEEK_SET:
				// how many more bytes forward is it from the current position?
				relPos = offset - df->filePos;
				break;
			case SEEK_CUR:
				// this one's easy
				relPos = offset;
				break;
			case SEEK_END:
				// treat this as a SEEK_SET after figuring out how many bytes in it is
				relPos = (df->fileLen + offset) - df->filePos;
				break;
		}
		
		if(relPos == 0)
		{
			return 0;
		}
		
		if(origin == SEEK_CUR)
		{
			// we must move to where the requesting process thinks the file is first			
			FAT_fseek((FAT_FILE *)df->fp, df->filePos, SEEK_SET);
		}
		
		if(relPos < 0 || relPos > (totalBytes - SEEK_BUFFER))
		{
			// we don't have (enough) data cached here, do a manual seek, and update the data
			FAT_fseek((FAT_FILE *)df->fp, offset, origin);
			
			// make sure it's flagged to be updated
			df->needsUpdate = true;
			DRAGON_cacheLoop();
		}
		else
		{
			// easy cheese, just update the pointers to point to the new location
			df->lagCursor += relPos;
			df->filePos += relPos;
		}
		
		return 0;
	}
	else
	{
		return FAT_fseek((FAT_FILE *)df->fp, offset, origin);
	}
}

bool DRAGON_feof(DRAGON_FILE* df)
{
	if(!df)
		return true;
	if(!df->fp)
		return true;
	
	if(df->cacheEnabled)
	{
		// figure out if we ran over the buffer
		return ((df->filePos) >= (df->fileLen));
	}
	else
	{
		return FAT_feof((FAT_FILE *)df->fp);
	}
}

bool DRAGON_fclose(DRAGON_FILE* df)
{
	if(!df)
		return false;
	
	int i;
	bool found = false;
	for(i = 0;i < MAX_FILES; i++)
	{
		if(curFiles[i] == (u32)df)
		{
			found = true;
			curFiles[i] = 0;
			
			break;
		}
	}
	
	if(!found)
		return false;
	
	bool result = FAT_fclose((FAT_FILE *)df->fp);
	
	if(df->cacheEnabled)
	{
		free(df->cache);
		df->cache = NULL;
		
		df->cacheEnabled = false;
	}
	
	free(df);
	
	return result;
}

u32 DRAGON_fread(void* buffer, u32 size, u32 count, DRAGON_FILE* df)
{
	if(!df)
		return 0;
	if(!df->fp)
		return 0;
	
	if(df->cacheEnabled)
	{	
		// figure out the size of the copy
		u32 toCopy = size * count;
		
		if((df->filePos + toCopy) > df->fileLen)
		{
			// make sure we never read over the end of the file
			toCopy = df->fileLen - df->filePos;
		}
		
		if(toCopy == 0)
		{	
			// eof
			return 0;
		}
		
		// we assume we've got enough in the cache to read
		if((df->lagCursor + toCopy) > CACHE_SIZE)
		{
			// split into two parts
			u32 firstCopy = CACHE_SIZE - df->lagCursor;
			u32 secondCopy = toCopy - firstCopy;
			
			u8 *tBuffer = (u8 *)buffer;
			
			memcpy(tBuffer, df->cache + df->lagCursor, firstCopy);
			memcpy(tBuffer + firstCopy, df->cache, secondCopy);
			
			df->lagCursor = secondCopy;
			df->filePos += toCopy;
		}
		else
		{
			// one direct copy
			memcpy(buffer, df->cache + df->lagCursor, toCopy);
			
			df->lagCursor += toCopy;
			df->filePos += toCopy;
		}
		
		if(toCopy < (size * count))
		{
			// didn't copy enough, lets fill with 0 to be sure
			memset(buffer + toCopy, 0, (size * count) - toCopy);
		}
		
		// make sure it returns the actual number of elements
		return toCopy;
	}
	else
	{
		return FAT_fread(buffer, size, count, (FAT_FILE *)df->fp);
	}
}

u32 DRAGON_fwrite(const void* buffer, u32 size, u32 count, DRAGON_FILE* df)
{
	if(!df)
		return 0;
	if(!df->fp)
		return 0;
	
	if(df->cacheEnabled)
	{
		// can't write in cached files
		return 0;
	}
	else
	{
		return FAT_fwrite(buffer, size, count, (FAT_FILE *)df->fp);
	}
}

u32 DRAGON_flength(DRAGON_FILE* df)
{
	if(!df)
		return 0;
	if(!df->fp)
		return 0;
	
	if(df->cacheEnabled)
	{
		// return length of file
		return df->fileLen;
	}
	else
	{
		u32 cPos = FAT_ftell((FAT_FILE *)df->fp);
		FAT_fseek((FAT_FILE *)df->fp, 0, SEEK_END);
		u32 aPos = FAT_ftell((FAT_FILE *)df->fp);
		FAT_fseek((FAT_FILE *)df->fp, cPos, SEEK_SET);
		
		return aPos;
	}
}

u32 DRAGON_ftell(DRAGON_FILE* df)
{
	if(!df)
		return 0;
	if(!df->fp)
		return 0;
	
	if(df->cacheEnabled)
	{
		// can't write in cached files
		return df->filePos;
	}
	else
	{
		return FAT_ftell((FAT_FILE *)df->fp);
	}
}

int DRAGON_FindFirstFile(char* lfn)
{
	return FAT_FindFirstFileLFN(lfn);
}

int DRAGON_FindNextFile(char* lfn)
{
	return FAT_FindNextFileLFN(lfn);
}

void DRAGON_closeFind()
{

}

uint16 DRAGON_fgetc(DRAGON_FILE *df)
{
	if(!df)
		return 0;
	if(!df->fp)
		return 0;
	
	if(df->cacheEnabled)
	{
		// read in character
		int tChar = df->cache[df->lagCursor];
		df->lagCursor++;
		
		return tChar;
	}
	else
	{
		return FAT_fgetc((FAT_FILE *)df->fp);
	}
}

void DRAGON_fputc(uint16 c, DRAGON_FILE* df)
{
	if(!df)
		return;
	if(!df->fp)
		return;
	
	if(df->cacheEnabled)
	{
		// can't write in cached files
		return;
	}
	else
	{
		FAT_fputc(c, (FAT_FILE *)df->fp);
	}
}

char *DRAGON_fgets(char *tgtBuffer, int num, DRAGON_FILE* df)
{
	if(!df)
		return NULL;
	if(!df->fp)
		return NULL;
	
	if(df->chanceOfUnicode == 0)
	{
		if(df->cacheEnabled)
		{
			// nothing that's cached should require this function
			return 0;
		}
		else
		{
			return FAT_fgets(tgtBuffer, num, (FAT_FILE *)df->fp);
		}
	}
	else
	{
		if(df->cacheEnabled)
		{
			// nothing that's cached should require this function
			return 0;
		}
		else
		{
			u32 curPos = DRAGON_ftell(df);
			u32 readLength = 0;
			char *tStr = (char *)trackMalloc(num*2, "unicode fake load");
			
			readLength = DRAGON_fread(tStr,1,(num-1) * 2,df);
			memset(tgtBuffer, 0, num);
			tStr[num-1] = '\0';
			tStr[num] = '\0';
			
			if (readLength==0) 
			{ 
				// return error 
				tgtBuffer[0] = '\0';
				return NULL; 
			}
			
			u32 i = 0;
			
			while(i < (num-1))
			{
				if(tStr[(i << 1)+1] == '\n')
				{
					tgtBuffer[i] = 0;
					DRAGON_fseek(df, curPos + ((i+1) << 1), 0);
					break;
				}
				
				if(tStr[(i << 1)+1] == '\r')
				{
					tgtBuffer[i] = 0;
					
					if(tStr[(i << 1)+3] == '\n')
					{
						DRAGON_fseek(df, curPos + ((i+2) << 1), 0);
						break;				
					}
					
					DRAGON_fseek(df, curPos + ((i+1) << 1), 0);
					break;
				}
				
				tgtBuffer[i] = tStr[(i << 1)+1];
				i++;
			}
			
			trackFree(tStr);		
			return tgtBuffer;
		}
	}
}

void DRAGON_detectUnicode(DRAGON_FILE* df)
{
	char tStr[4];
	
	DRAGON_fseek(df, 0, SEEK_SET);
	DRAGON_fread(tStr, 4, 1, df);
	
	if(tStr[0] == 0 && tStr[2] == 0 && tStr[1] != 0 && tStr[3] != 0) // fairly good chance it's unicode
		df->chanceOfUnicode = 1;
	else
		df->chanceOfUnicode = 0;
	
	DRAGON_fseek(df, 0, SEEK_SET);
}

int DRAGON_fputs(const char *string, DRAGON_FILE* df)
{
	if(!df)
		return 0;
	if(!df->fp)
		return 0;
	
	if(df->cacheEnabled)
	{
		// can't write in cached files
		return 0;
	}
	else
	{
		return FAT_fputs(string, (FAT_FILE *)df->fp);
	}
}

u8 DRAGON_GetFileAttributes()
{
	return FAT_GetFileAttributes();
}

u8 DRAGON_SetFileAttributes(const char* filename, u8 attributes, u8 mask)
{
	return FAT_SetFileAttributes (filename, attributes, mask);
}

void DRAGON_preserveVars()
{
	FAT_preserveVars();
}

void DRAGON_restoreVars()
{
	FAT_restoreVars();
}

bool DRAGON_GetAlias(char* alias)
{
	return FAT_GetAlias(alias);
}

int DRAGON_remove(const char* path)
{	
	return FAT_remove(path);
}

bool DRAGON_rename(const char *oldName, const char *newName)
{	
	return FAT_rename(oldName, newName);
}

u32 DRAGON_DiscType()
{
	return disc_HostType();
}

bool DRAGON_GetLongFilename(char* filename)
{
	return FAT_GetLongFilename(filename);
}

u32 DRAGON_GetFileSize()
{
	return FAT_GetFileSize();
}

u32 DRAGON_GetFileCluster()
{
	return FAT_GetFileCluster();
}

void DRAGON_enableCaching(DRAGON_FILE *df)
{	
	// fill in length
	df->fileLen = DRAGON_flength(df);

	// set up default memory space
	df->cacheEnabled = true;	
	df->cache = (u8 *)safeMalloc(CACHE_SIZE);
	
	df->needsUpdate = true;
	
	// load some actual data
	DRAGON_cacheLoop();
}

bool DRAGON_isCached(DRAGON_FILE *df)
{
	return df->cacheEnabled;
}

void DRAGON_cacheLoop()
{
	int i;
	for(i = 0;i < MAX_FILES; i++)
	{
		if(curFiles[i] != 0)
		{
			// found an open handle
			DRAGON_FILE *df = (DRAGON_FILE *)curFiles[i];
			
			// test to see if it's cached
			if(df->cacheEnabled)
			{
				if(df->needsUpdate)
				{
					df->needsUpdate = false;
					
					df->filePos = FAT_ftell((FAT_FILE *)df->fp);
					
					df->leadCursor = 0;
					df->lagCursor = 0;
				}
				
				// figure out how many bytes we actually have loaded
				u32 totalBytes = 0;
				
				if(df->leadCursor > df->lagCursor)
				{
					// normal subtraction to find length
					totalBytes = df->leadCursor - df->lagCursor;
				}
				else
				{
					// we've looped around
					totalBytes = CACHE_SIZE - df->lagCursor;
					totalBytes += df->leadCursor;
				}
				
				// make sure we haven't loaded past the end of the file
				if(df->filePos + totalBytes >= df->fileLen)
				{
					// we already have enough loaded
					continue;
				}
				
				// cached, ensure that we have enough data				
				u32 tLead = df->leadCursor;
				u32 tLag = df->lagCursor;
				u32 loadSize = 0;
				
				if(tLag > tLead)
				{
					// we have looped around the ring buffer
					u32 bufSize = (tLag - tLead); // should be CACHE_BUFFER if filled
					
					if(bufSize > CACHE_BUFFER)
					{
						// not entirely filled
						loadSize = bufSize - CACHE_BUFFER;
					}
				}
				else
				{
					// we are still in one consecutive chunk
					u32 bufSize = CACHE_SIZE - (tLead - tLag); // should be CACHE_BUFFER if filled
					
					if(bufSize > CACHE_BUFFER)
					{
						// not entirely filled
						loadSize = bufSize - CACHE_BUFFER;
					}
				}
				
				if(loadSize > 0)
				{	
					// we need to cache additional data, load it into the lead cursors position and update accordingly
					if(loadSize + tLead > CACHE_SIZE)
					{
						// special case, loops around
						u32 firstRead = CACHE_SIZE - tLead;
						u32 secondRead = loadSize - firstRead;
						
						FAT_fread(df->cache + tLead, 1, firstRead, (FAT_FILE *)df->fp);
						FAT_fread(df->cache, 1, secondRead, (FAT_FILE *)df->fp);
						
						df->leadCursor = secondRead;
					}
					else
					{
						// we can just read in
						FAT_fread(df->cache + tLead, 1, loadSize, (FAT_FILE *)df->fp);
						df->leadCursor += loadSize;
					}
				}
			}
		}
	}
}
