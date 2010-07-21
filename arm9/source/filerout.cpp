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
 
#include <string.h>
#include <stdio.h>
#include <nds.h>
#include <stdlib.h>
#include <libpicture.h>
#include <vcard.h>
#include "fatwrapper.h"
#include "filerout.h"
#include "graphics.h"
#include "general.h"
#include "globals.h"
#include "shortcuts.h"
#include "keyboard.h"
#include "drawing\browser.h"
#include "drawing\todo.h"
#include "drawing\address.h"

extern bool showHidden;

TODO_FILE *todoList = NULL;
VCARD_FILE *vCardList = NULL;
BROWSER_FILE *dirList = NULL;
SCRIBBLE_FILE *scribbleList = NULL;
#define soundList dirList

static char lastDir[256] = { 0 };

void safeFileName(char *fName)
{
	int i = 0;
	
	while(fName[i] != 0)
	{
		switch(fName[i])
		{
			case '\\':
			case '/':
			case ':':
			case '*':
			case '?':
			case '\"':
			case '<':
			case '>':
			case '|':
				fName[i] = '_';
				break;
		}
		i++;
	}
}

void appendSlash(char *dir)
{
	int x = strlen(dir);
	
	if(dir[x-1] != '/')
	{
		strcat(dir, "/");
	}
}

int compareVCard(const void * a, const void * b)
{
	VCARD_FILE *v1 = (VCARD_FILE*)a;
	VCARD_FILE *v2 = (VCARD_FILE*)b;
	
	char str1[70];
	char str2[70];
	
	calculateListName(v1, str1);
	calculateListName(v2, str2);
	
	strlwr(str1);
	strlwr(str2);
	
	return strcmp(str1, str2);	
}

int compareDir(const void * a, const void * b)
{
	BROWSER_FILE *v1 = (BROWSER_FILE*)a;
	BROWSER_FILE *v2 = (BROWSER_FILE*)b;
	
	if(v1->fileType == DIRECTORY && v2->fileType == DIRECTORY)
	{
		// ensure that '.' and '..' always come first		
		if(strcmp(v1->longName, ".") == 0)
		{
			return -1;
		}
		
		if(strcmp(v2->longName, ".") == 0)
		{
			return 1;
		}
		
		// we now know that neither file is '.', so test for '..'
		if(strcmp(v1->longName, "..") == 0)
		{
			return -1;
		}
		
		if(strcmp(v2->longName, "..") == 0)
		{
			return 1;
		}
	}
	
	if(v1->fileType == DIRECTORY && v2->fileType != DIRECTORY)
		return -1; // directories always come first.
	
	if(v1->fileType != DIRECTORY && v2->fileType == DIRECTORY)
		return 1; // directories always come first.
	
	char str1[256];
	char str2[256];
	
	strncpy(str1, v1->longName, 255);
	strncpy(str2, v2->longName, 255);
	
	strlwr(str1);
	strlwr(str2);
	
	return strcmp(str1, str2);	
}

void sortVCard(int pos)
{
	if(pos > 1) // lets sort this fucker
		qsort(vCardList, pos, sizeof(VCARD_FILE), compareVCard);
}

void sortDirList(int pos)
{
	if(pos > 1) // lets sort this fucker
		qsort(dirList, pos, sizeof(BROWSER_FILE), compareDir);
}

int populateVCard(char *dir)
{
	char tmpFile[256];
	int fType;
	int pos = 0;
	
	appendSlash(dir);	
	freeVCard();
	
	DRAGON_chdir("/");
	DRAGON_chdir(dir);
	fType = DRAGON_FindFirstFile(tmpFile);
	
	while(fType != FE_NONE)
	{
		if(fType == FE_FILE)
		{
			if(isVCard(tmpFile))
			{
				if(pos == 0)
				{
					vCardList = (VCARD_FILE *)trackMalloc(sizeof(VCARD_FILE), "vCard file");
					pos = 1;
				}
				else
				{
					pos++;
					vCardList = (VCARD_FILE *)trackRealloc(vCardList, sizeof(VCARD_FILE) * pos);
				}
				
				loadVCard(tmpFile, &vCardList[pos-1]);
			}			
		}
		
		fType = DRAGON_FindNextFile(tmpFile);		
	}
	
	DRAGON_closeFind();
	DRAGON_chdir("/");		
	sortVCard(pos);
	
	return pos;
}

void freeVCard()
{
	if(vCardList != NULL)
		trackFree(vCardList);	
	
	vCardList = NULL;
}

int addVCard()
{
	int tmpEntries = getAddressEntries();
	vCardList = (VCARD_FILE *)trackRealloc(vCardList, (sizeof(VCARD_FILE)) * (tmpEntries + 1));
	
	return tmpEntries;
}

int getFileType(char *fName)
{
	char tmpFile[256];	
	char tStr[256];
	
	strcpy(tmpFile, fName);
	
	separateExtension(tmpFile,tStr);		
	strlwr(tStr);
	
	if(strlen(tStr) == 0)
		return UNKNOWN;
	
	if(strcmp(tStr, ".nds") == 0)
		return NDSFILE;	
	if(strcmp(tStr, ".gba") == 0)
	{
		separateExtension(tmpFile,tStr);		
		strlwr(tStr);
		if(strcmp(tStr, ".ds") == 0)
			return NDSFILE;
	}
	if(	strcmp(tStr, ".txt") == 0 || strcmp(tStr, ".ini") == 0 || strcmp(tStr, ".lng") == 0 || \
		strcmp(tStr, ".htm") == 0 || strcmp(tStr, ".html") == 0 || strcmp(tStr, ".cfg") == 0 || \
		strcmp(tStr, ".conf") == 0 || strcmp(tStr, ".xml") == 0)
		return TXTFILE;				
	if(strcmp(tStr, ".vcf") == 0)
		return VCFILE;
	if(strcmp(tStr, ".dpl") == 0)
		return DVFILE;
	if(strcmp(tStr, ".rem") == 0)				
		return REMFILE;
	if(strcmp(tStr, ".bmp") == 0 || strcmp(tStr, ".jpg") == 0 || strcmp(tStr, ".jpeg") == 0 || strcmp(tStr, ".png") == 0 || strcmp(tStr, ".gif") == 0)
		return PICFILE;
	if(strcmp(tStr, ".bin") == 0 || strcmp(tStr, ".raw") == 0 || strcmp(tStr, ".pal") == 0)				
		return BINFILE;
	if(strcmp(tStr, ".todo") == 0)
		return TODOFILE;
	if(	strcmp(tStr, ".wav") == 0 || strcmp(tStr, ".mp3") == 0 || strcmp(tStr, ".ogg") == 0 || \
		strcmp(tStr, ".flac") == 0 || strcmp(tStr, ".sid") == 0 || strcmp(tStr, ".nsf") == 0 || \
		strcmp(tStr, ".mp2") == 0 || strcmp(tStr, ".mpa") == 0 || strcmp(tStr, ".spc") == 0 || \
		strcmp(tStr, ".aac") == 0 || strcmp(tStr, ".m4a") == 0 || strcmp(tStr, ".m4b") == 0 || \
		strcmp(tStr, ".it") == 0  || strcmp(tStr, ".mod") == 0 || strcmp(tStr, ".s3m") == 0 || \
        strcmp(tStr, ".xm") == 0 || strcmp(tStr, ".sndh") == 0 || strcmp(tStr, ".snd") == 0 || \
		strcmp(tStr, ".sc68") == 0 || strcmp(tStr, ".gbs") == 0)
		return SOUNDFILE;
	if(strcmp(tStr, ".pls") == 0 || strcmp(tStr, ".m3u") == 0)
		return PLAYLISTFILE;
	if(strcmp(tStr, ".hbdb") == 0)
		return HBFILE;
	if(strcmp(tStr, ".pkg") == 0)
		return PKGFILE;
	if(strcmp(tStr, ".dss") == 0)
		return SHORTCUTFILE;
	
	return UNKNOWN;
}

char *getLastDir()
{
	if(lastDir[0] != '/')
	{
		char tmp[256];
		sprintf(tmp, "/%s", lastDir);
		strcpy(lastDir, tmp);
	}
	
	if(lastDir[strlen(lastDir)-1] != '/')
	{
		int x = strlen(lastDir);
		
		lastDir[x] = '/';
		lastDir[x+1] = 0;
	}
	
	return lastDir;
}

int populateDirList(char *dir)
{
	char tmpFile[256];
	int fType;
	
	appendSlash(dir);
	
	int pos = 0;
	
	strcpy(lastDir, dir);
	DRAGON_chdir("/");
	DRAGON_chdir(dir);
	fType = DRAGON_FindFirstFile(tmpFile);
	
	freeDirList();	
	dirList = (BROWSER_FILE *)trackMalloc(sizeof(BROWSER_FILE),"Browser list");
	
	while(fType != FE_NONE)
	{
		dirList = (BROWSER_FILE *)trackRealloc(dirList, (sizeof(BROWSER_FILE)) * (pos + 2));
		
		memset(&dirList[pos], 0, sizeof(BROWSER_FILE));
		
		dirList[pos].hidden = ((DRAGON_GetFileAttributes() & ATTRIB_HID) == ATTRIB_HID) | ((DRAGON_GetFileAttributes() & ATTRIB_SYS) == ATTRIB_SYS);
		
		if(strcmp(tmpFile, ".") == 0 || strcmp(tmpFile, "..") == 0)
			dirList[pos].hidden = true;
		
		if(showHidden == false && dirList[pos].hidden == true)
		{
			// skip this file
		}
		else
		{
			strcpy(dirList[pos].longName, tmpFile);
			dirList[pos].played = false;			
			dirList[pos].fileSize = 0;
			
			if(fType == FE_FILE)
			{
				DRAGON_FILE *fFile = DRAGON_fopen(tmpFile, "r");
				dirList[pos].fileSize = DRAGON_flength(fFile);
				DRAGON_fclose (fFile);	
				
				dirList[pos].fileType = getFileType(tmpFile);			
			}
			if(fType == FE_DIR)
				dirList[pos].fileType = DIRECTORY;
			
			pos++;
		}
		
		fType = DRAGON_FindNextFile(tmpFile);
	}
	
	DRAGON_closeFind();
	DRAGON_chdir("/");	
	sortDirList(pos);
	
	return pos;
}

void freeDirList()
{
	if(dirList != NULL)
		trackFree(dirList);	
	
	dirList = NULL;
}

bool isM3SC(char *fName)
{
	int x = strlen(fName) - 4;
	
	if((fName[x-1] == 'c' || fName[x-1] == 'C') && (fName[x-2] == 's' || fName[x-2] == 'S') && (fName[x-3] == '.'))
		return true;
	
	if((fName[x-1] == 's' || fName[x-1] == 'S') && (fName[x-2] == 'd' || fName[x-2] == 'D') && (fName[x-3] == '.'))
		return true;
	
	return false;
}

void checkForSC(char *fName)
{
	int x = strlen(fName);
	
	if(isM3SC(fName))
		fName[x-3] = 0;
}

int readNDSIcon(DRAGON_FILE *fFile, u32 offset, uint16 *data)
{
	char c;
	int x,y,z;
	uint16 pallate[16];
	
	DRAGON_fseek(fFile, offset + 544, 0);
	
	for(x=0;x<16;x++)
	{
		pallate[x] = 0;
		
		c = DRAGON_fgetc(fFile);	
		pallate[x] = pallate[x] | c;
		
		c = DRAGON_fgetc(fFile);
		pallate[x] = pallate[x] | (c << 8);
	}
	
	DRAGON_fseek(fFile, offset + 32, 0);
	uint16 *rawData = (uint16 *)safeMalloc(sizeof(uint16) * NDS_ICON_SIZE);
	
	for(x=0;x<NDS_ICON_SIZE;x+=2)
	{
		c = DRAGON_fgetc(fFile);	
		
		rawData[x+1] = pallate[(int)((c >> 4) & 0x0F)];
		rawData[x] = pallate[(int)(c & 0x0F)];
	}
	
	// ok we gotta pull it outta 8x8 mode.
	x = 0;
	int xx, yy;
	
	for(y=0;y<4;y++)
	{
		for(z=0;z<4;z++)
		{
			// location z,y
			
			for(yy=0;yy<8;yy++)
			{
				for(xx=0;xx<8;xx++)
				{
					// location (z * 8) + xx, (y * 8) + yy;
					
					data[(z * 8) + xx + (((y * 8) + yy) * 32) + 2] = rawData[x];
					x++;
				}
			}
		}
	}
	
	free(rawData);
	return pallate[0];
}

void getInfo(BROWSER_FILE *bf, char *path1, FILE_INFO *file)
{	
	char path[256];
	char str[256];
	char c;
	int x,y;
	u32 offset;
	
	strcpy(path, path1);
	strcpy(file->longName, bf->longName);
	
	memset(file->iconData, 0xFFFF, 1026*2);
	file->iconData[0] = 31;
	file->iconData[1] = 31;	
	file->transColor = 31775;
	
	x = strlen(path)-1;
	
	if(path[x] == '/')
		path[x] = 0;
	
	strcat(path, "/");
	strcat(path, bf->longName);
	
	memset(file->formattedName, 0, 256 * 3);
	
	if(getFileType(path) == NDSFILE)
	{		
		int scm3Offset = 0;
		
		if(isM3SC(bf->longName))
			scm3Offset = 512;
		
		DRAGON_FILE *fFile = DRAGON_fopen(path, "r");
		u32 tLength = DRAGON_flength(fFile);
		
		if(tLength <= 107)
		{
			// file isn't big enough to be valid
			
			strncpy(str, bf->longName, strlen(bf->longName) - 4);
			strncpy(file->formattedName[1], bf->longName, strlen(bf->longName) - 4);
			checkForSC(file->formattedName[1]);
			memcpy(file->iconData, ndsgenericlarge, 1026*2);
			
			strcpy(file->internalName, str);
			DRAGON_fclose(fFile);
			return;
		}
		
		DRAGON_fseek(fFile, 104 + scm3Offset, 0);
		
		offset = 0;
		
		c = DRAGON_fgetc(fFile);	
		offset = offset | c;
		
		c = DRAGON_fgetc(fFile);
		offset = offset | (c << 8);
		
		c = DRAGON_fgetc(fFile);
		offset = offset | (c << 16);
		
		c = DRAGON_fgetc(fFile);
		offset = offset | (c << 24);
		
		// check the version, some files that run through file managers of
		// m3 units will get destroyed picture information
		
		int version = 0;
		
		if(tLength >= (offset+2111+scm3Offset))
		{
			DRAGON_fseek(fFile, offset + scm3Offset, 0);
			
			c = DRAGON_fgetc(fFile);	
			version = version | c;
			
			c = DRAGON_fgetc(fFile);
			version = version | (c << 8);		
		}
		
		if(version != 1)
		{
			// file isn't big enough to be valid, or is not proper data
			
			strncpy(str, bf->longName, strlen(bf->longName) - 4);
			strncpy(file->formattedName[1], bf->longName, strlen(bf->longName) - 4);
			checkForSC(file->formattedName[1]);
			memcpy(file->iconData, ndsgenericlarge, 1026*2);
			
			strcpy(file->internalName, str);
			DRAGON_fclose(fFile);
			return;
		}
		
		memset(str,0,256);
		if(offset > 0)
		{		
			// now to read the name
			// since libfb doesn't support unicode, we are going to
			// just read every other byte, cheap trick, but eh.
			DRAGON_fseek(fFile, offset + 832 + scm3Offset, 0);
			
			c = DRAGON_fgetc(fFile);
			DRAGON_fgetc(fFile);
			
			x = 0;
			
			while(c != 0)
			{
				str[x] = c;
				x++;
				
				c = DRAGON_fgetc(fFile);
				DRAGON_fgetc(fFile);
			}			
			
			// now to format it to properly display if needed
			
			x = 0;
			y = 0;
			int z = 0;
			
			while(str[x] != 0 && y < 3)
			{
				if(str[x] != '\n')
				{
					file->formattedName[y][z] = str[x];
					x++;
					z++;
				}
				else
				{
					file->formattedName[y][z] = 0;
					z = 0;
					y++;
					x++;
				}
			}			
			
			// now to read the icons
			
			file->transColor = readNDSIcon(fFile, offset + scm3Offset, file->iconData);
		}
		else
		{
			// no name supplied, lets just fill in an icon sprite
			// and the filename for the title
			
			strncpy(str, bf->longName, strlen(bf->longName) - 4);
			strncpy(file->formattedName[1], bf->longName, strlen(bf->longName) - 4);
			checkForSC(file->formattedName[1]);
			memcpy(file->iconData, ndsgenericlarge, 1026*2);			
		}
		
		strcpy(file->internalName, str);
		
		DRAGON_fclose(fFile);
	} 
	else if(bf->fileType == DIRECTORY)
	{
		strcpy(str, bf->longName);
		memcpy(file->iconData, diretorylarge, 1026*2);	
		
		strcpy(file->internalName, str);		
	} 
	else if(bf->fileType == VCFILE)
	{
		strcpy(str, bf->longName);
		memcpy(file->iconData, vcfilelarge, 1026*2);	
		
		strcpy(file->internalName, str);		
	} 
	else if(bf->fileType == DVFILE)
	{
		strcpy(str, bf->longName);
		memcpy(file->iconData, dpfilelarge, 1026*2);	
		
		strcpy(file->internalName, str);		
	} 	
	else if(bf->fileType == REMFILE)
	{
		strcpy(str, bf->longName);
		memcpy(file->iconData, reminderfilelarge, 1026*2);	
		
		strcpy(file->internalName, str);		
	} 
	else if(bf->fileType == TXTFILE)
	{
		strcpy(str, bf->longName);
		memcpy(file->iconData, txtfilelarge, 1026*2);	
		
		strcpy(file->internalName, str);		
	} 
	else if(bf->fileType == PICFILE || bf->fileType == BINFILE)
	{
		strcpy(str, bf->longName);
		memcpy(file->iconData, picfilelarge, 1026*2);	
		
		strcpy(file->internalName, str);		
	} 
	else if(bf->fileType == TODOFILE)
	{
		strcpy(str, bf->longName);
		memcpy(file->iconData, todofilelarge, 1026*2);	
		
		strcpy(file->internalName, str);		
	}  
	else if(bf->fileType == SOUNDFILE)
	{
		strcpy(str, bf->longName);
		memcpy(file->iconData, soundfilelarge, 1026*2);	
		
		strcpy(file->internalName, str);		
	} 
	else if(bf->fileType == PLAYLISTFILE)
	{
		strcpy(str, bf->longName);
		memcpy(file->iconData, playlistfilelarge, 1026*2);	
		
		strcpy(file->internalName, str);		
	} 
	else if(bf->fileType == HBFILE || bf->fileType == PKGFILE)
	{
		strcpy(str, bf->longName);
		memcpy(file->iconData, hbfilelarge, 1026*2);	
		
		strcpy(file->internalName, str);		
	}
	else if(bf->fileType == SHORTCUTFILE)
	{
		strcpy(str, path);
		
		SHORTCUT sc;
		
		if(loadShortcut(path, &sc))
			strcpy(file->internalName, sc.name);
		else
			strcpy(file->internalName, str);
		
		DRAGON_preserveVars();
		DRAGON_chdir("/");
		
		switch(DRAGON_FileExists(sc.path))
		{
			case FE_DIR:
				memcpy(file->iconData, diretorylarge, 1026*2);
				
				break;
			case FE_FILE:
				int tFileType = getFileType(sc.path);
				
				if(tFileType == NDSFILE)
				{
					u32 scm3Offset = 0;
					
					if(isM3SC(sc.path))
						scm3Offset = 512;
					
					DRAGON_FILE *fFile = DRAGON_fopen(sc.path, "r");
					u32 tLength = DRAGON_flength(fFile);
					
					if(tLength <= 107)
					{
						memcpy(file->iconData, ndsgenericlarge, 1026*2);
						DRAGON_fclose(fFile);
						break;
					}
					
					DRAGON_fseek(fFile, 104 + scm3Offset, 0);
					
					offset = 0;
					
					c = DRAGON_fgetc(fFile);	
					offset = offset | c;
					
					c = DRAGON_fgetc(fFile);
					offset = offset | (c << 8);
					
					c = DRAGON_fgetc(fFile);
					offset = offset | (c << 16);
					
					c = DRAGON_fgetc(fFile);
					offset = offset | (c << 24);
					
					// check the version, some files that run through file managers of
					// m3 units will get destroyed picture information
					
					int version = 0;
					
					if(tLength >= (offset+2111+scm3Offset))
					{
						DRAGON_fseek(fFile, offset + scm3Offset, 0);
						
						c = DRAGON_fgetc(fFile);	
						version = version | c;
						
						c = DRAGON_fgetc(fFile);
						version = version | (c << 8);		
					}
					
					if(version != 1)
					{
						memcpy(file->iconData, ndsgenericlarge, 1026*2);
						DRAGON_fclose(fFile);
						break;
					}
					
					file->transColor = readNDSIcon(fFile, offset + scm3Offset, file->iconData);
					DRAGON_fclose(fFile);
				}
				else
				{
					uint16 *tIcons[] = { 0, 0, ndsgenericlarge, txtfilelarge, vcfilelarge, dpfilelarge, reminderfilelarge, picfilelarge, picfilelarge, todofilelarge, soundfilelarge, hbfilelarge, hbfilelarge, playlistfilelarge, cantlaunchlarge };
					
					memcpy(file->iconData, tIcons[tFileType], 1026*2);
				}
				
				break;
			default:
				memcpy(file->iconData, cantlaunchlarge, 1026*2);
				
				break;
		}
		
		DRAGON_restoreVars();
	}
	else
	{
		strcpy(str, bf->longName);
		memcpy(file->iconData, unknownlarge, 1026*2);	
		
		strcpy(file->internalName, str);
	}
}

int compareTodo(const void * a, const void * b)
{
	TODO_FILE *v1 = (TODO_FILE*)a;
	TODO_FILE *v2 = (TODO_FILE*)b;
	
	if(v1->urgency == FINISHED && v2->urgency != FINISHED)
		return 1;
		
	if(v1->urgency != FINISHED && v2->urgency == FINISHED)
		return -1;
	
	if(v1->urgency > v2->urgency)
		return -1; // urgency sort first
	
	if(v1->urgency < v2->urgency)
		return 1; // urgency sort first
	
	char str1[63];
	char str2[63];
	
	strcpy(str1, v1->title);
	strcpy(str2, v2->title);
	
	strlwr(str1);
	strlwr(str2);
	
	if(strcmp(str1, str2) == 0)
	{
		strcpy(str1, v1->fileName);
		strcpy(str2, v2->fileName);
		
		strlwr(str1);
		strlwr(str2);
	}
	
	return strcmp(str1, str2);	
}

int populateTodoList(char *dir)
{
	char tmpFile[256];
	int fType;
	
	int pos = 0;
	
	appendSlash(dir);
	DRAGON_chdir("/");
	DRAGON_chdir(dir);
	fType = DRAGON_FindFirstFile(tmpFile);
	
	freeTodoList();
	
	todoList = (TODO_FILE *)trackMalloc(sizeof(TODO_FILE),"Todo list");	
	memset(todoList,0,sizeof(TODO_FILE));
	
	while(fType != FE_NONE)
	{
		if(getFileType(tmpFile) == TODOFILE)
		{
			todoList = (TODO_FILE *)trackRealloc(todoList, (sizeof(TODO_FILE)) * (pos + 2));
			
			DRAGON_FILE *fp = DRAGON_fopen(tmpFile, "r");
			int sz = 0;
			
			todoList[pos].urgency = DRAGON_fgetc(fp);
			
			sz = DRAGON_fgetc(fp);
			sz = sz | DRAGON_fgetc(fp) << 8;			
			memset(todoList[pos].title, 0, 41);			
			DRAGON_fread(todoList[pos].title, 1, sz, fp); // get the entry itself
			
			sz = DRAGON_fgetc(fp);
			sz = sz | DRAGON_fgetc(fp) << 8;			
			memset(todoList[pos].message, 0, 41);			
			DRAGON_fread(todoList[pos].message, 1, sz, fp); // get the entry itself
			
			DRAGON_fclose(fp);
			strcpy(todoList[pos].fileName, tmpFile);
			
			pos++;
		}
		
		fType = DRAGON_FindNextFile(tmpFile);		
	}
	
	DRAGON_closeFind();
	DRAGON_chdir("/");	
	sortTodoList(pos);	
	return pos;
}

void freeTodoList()
{
	if(todoList != NULL)
		trackFree(todoList);	
	
	todoList = NULL;
}

int addTodo()
{
	int tmpEntries = getTodoEntries();
	todoList = (TODO_FILE *)trackRealloc(todoList, (sizeof(TODO_FILE)) * (tmpEntries + 1));
	
	return tmpEntries;
}

void sortTodoList(int pos)
{
	if(pos > 1) // lets sort this fucker
		qsort(todoList, pos, sizeof(TODO_FILE), compareTodo);
}

void clearTodo(TODO_FILE *vf)
{
	memset(vf->title, 0, 41);
	memset(vf->message, 0, 1025);
	memset(vf->fileName, 0, 256);
	vf->urgency = 0;
}

bool isScribbleFile(char *tmpFile)
{
	int x = strlen(tmpFile);			
	
	if(x > 3)
	{
		x--;
		
		char tStr[5];
		
		tStr[0] = tmpFile[x-3];
		tStr[1] = tmpFile[x-2];
		tStr[2] = tmpFile[x-1];
		tStr[3] = tmpFile[x];
		tStr[4] = 0;
		
		strlwr(tStr);
		if(strcmp(tStr, ".bmp") == 0 || strcmp(tStr, ".png") == 0)
		{
			int x, y;
			
			if(getPictureSize(tmpFile, x, y))
			{
				if(x == 256 && y == 192)
					return true;
			}
		}
	}
	
	return false;
}

int compareScribble(const void * a, const void * b)
{
	SCRIBBLE_FILE *v1 = (SCRIBBLE_FILE*)a;
	SCRIBBLE_FILE *v2 = (SCRIBBLE_FILE*)b;
	
	char str1[63];
	char str2[63];
	
	strcpy(str1, v1->fileName);
	strcpy(str2, v2->fileName);
	
	strlwr(str1);
	strlwr(str2);
	
	return strcmp(str1, str2);	
}

int populateScribbleList(char *dir)
{
	char tmpFile[256];
	int fType;
	
	int pos = 0;
	
	appendSlash(dir);
	DRAGON_chdir("/");
	DRAGON_chdir(dir);
	fType = DRAGON_FindFirstFile(tmpFile);
	
	freeScribbleList();
	
	scribbleList = (SCRIBBLE_FILE *)trackMalloc(sizeof(SCRIBBLE_FILE),"Scribble list");	
	memset(scribbleList,0,sizeof(SCRIBBLE_FILE));
	
	while(fType != FE_NONE)
	{
		if(isScribbleFile(tmpFile))
		{
			scribbleList = (SCRIBBLE_FILE *)trackRealloc(scribbleList, (sizeof(SCRIBBLE_FILE)) * (pos + 2));
			
			char str[256];
			
			strcpy(str, dir);
			
			int x = strlen(str)-1;
			
			if(str[x] == '/')
				str[x] = 0;
				
			strcat(str, "/");
			strcat(str, tmpFile);			
			strcpy(scribbleList[pos].fileName, str);
			
			char ext[256];
			
			separateExtension(tmpFile,ext);			
			strcpy(scribbleList[pos].shortName, tmpFile);
			
			pos++;
		}
		
		fType = DRAGON_FindNextFile(tmpFile);		
	}
	
	DRAGON_closeFind();
	DRAGON_chdir("/");	
	sortScribbleList(pos);	
	return pos;
}

void freeScribbleList()
{
	if(scribbleList != NULL)
		trackFree(scribbleList);	
	
	scribbleList = NULL;
}

void sortScribbleList(int pos)
{
	if(pos > 1) // lets sort this fucker
		qsort(scribbleList, pos, sizeof(SCRIBBLE_FILE), compareScribble);
}

void separateDir(char *dir, char *file)
{
	int x = 0;
	int y = 0;
	
	while(file[x] != 0)
	{
		if(file[x] == '/')
			y = x;
		
		x++;
	}
	
	if(y == 0) // file is already the whole thing
	{
		strcpy(dir, "/");
		
		if(file[0] == '/')
		{
			// needs shift
			x = 1;
			y = 0;
			while(file[x] != 0)
			{
				file[y] = file[x];
				y++;
				x++;
			}
			
			file[y] = 0;
		}		
		return;
	}
	
	file[y] = 0;
	strcpy(dir, file);
	if(dir[strlen(dir)-1] != '/')
		strcat(dir, "/");
	
	x = y + 1;
	y = 0;
	while(file[x] != 0)
	{
		file[y] = file[x];
		y++;
		x++;
	}
	
	file[y] = 0;
}

int nextFile(char *curFile, int numEntries)
{
	char curDir[256];
	char curName[256];
	
	strcpy(curName, curFile);
	
	separateDir(curDir, curName);
	
	bool found = false;
	int i = 0;
	
	while(!found)
	{
		if(strcmp(curName, soundList[i].longName) == 0)
		{
			found = true;
			if(i == numEntries - 1)
				return -1; // no next element
			
			char str[256];
			strcpy(str, curDir);		
			
			int x = strlen(str)-1;
			
			if(str[x] == '/')
				str[x] = 0;
			
			strcat(str, "/");
			strcat(str, soundList[i+1].longName);
			
			strcpy(curFile,str);
			return i+1;
		}
		
		i++;
	}
	
	return -1;
}

int prevFile(char *curFile)
{
	char curDir[256];
	char curName[256];	
	
	strcpy(curName, curFile);
	
	separateDir(curDir, curName);
	
	bool found = false;
	int i = 0;
	
	while(!found)
	{
		if(strcmp(curName, soundList[i].longName) == 0)
		{
			found = true;
			if(i == 0)
				return -1; // no next element
			
			char str[256];
			strcpy(str, curDir);		
			
			int x = strlen(str)-1;
			
			if(str[x] == '/')
				str[x] = 0;
			
			strcat(str, "/");
			strcat(str, soundList[i-1].longName);
			
			strcpy(curFile,str);
			return i-1;
		}
		
		i++;
	}
	
	return -1;
}

int randFile(char *curFile, int fileType, int numEntries)
{
	char curDir[256];
	char curName[256];	
	int *numArray = NULL;
	int numFiles = 0;
	int tmpFiles = 0;
	int lastFile = 0;
	
	numArray = (int *)trackMalloc(sizeof(int) * numEntries,"Random file");
	memset(numArray, 0, sizeof(int) * numEntries);
	
	strcpy(curName, curFile);	
	separateDir(curDir, curName);	
	soundList[getCursor()].played = true;
	
	for(int i=0;i<numEntries;i++)
	{
		if(getFileType(soundList[i].longName) == fileType)		
		{
			tmpFiles++;
			if(!soundList[i].played)
			{
				numFiles++;
				numArray[i] = 1;
			}
		}
	}
	
	if(tmpFiles < 2) // none in this dir
	{
		trackFree(numArray);
		return -1;
	}
	else
	{
		if(numFiles == 0) // we have results, but they have all been played
		{
			numFiles = 0;
			for(int i=0;i<numEntries;i++)
			{
				if(getFileType(soundList[i].longName) == fileType)		
				{
					if(i != getCursor())
					{
						numFiles++;
						numArray[i] = 1;
						soundList[i].played = false;
					}
					else
						soundList[i].played = true;
				}
			}		
		}
	}
	
	// more than one file
	int whichFile = ((rand() >> 8) % numFiles);	
	bool found = false;
	numFiles = 0;
	
	while(!found)
	{
		if(numArray[lastFile] == 1)
		{
			if(numFiles == whichFile) // success!
			{
				found = true;
				char str[256];
				strcpy(str, curDir);		
				
				int x = strlen(str)-1;
				
				if(str[x] == '/')
					str[x] = 0;
				
				strcat(str, "/");
				strcat(str, soundList[lastFile].longName);
				
				strcpy(curFile,str);
				trackFree(numArray);
				return lastFile;
			}
			else
			{
				numFiles++;
			}
		}
		
		lastFile++;
	}
	
	trackFree(numArray);
	return -1;
}
