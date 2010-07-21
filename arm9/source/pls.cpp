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
#include "fatwrapper.h"
#include "general.h"
#include "pls.h"
#include "http.h"
#include "filerout.h"

bool isNumber(char c)
{
	if(c < '0' || c > '9')
		return false;
	
	return true;
}

void fillBlanks(PLS_TYPE *outData)
{
	// fills any empty descriptions with the url so it looks better
	
	if(outData->numEntries == 0) // no entries
		return;
	
	for(int i=0;i<outData->numEntries;i++)
	{
		if(strlen(outData->descriptionEntry[i].data) == 0)
		{
			// it's blank, copy over the url			
			memcpy(outData->descriptionEntry[i].data, outData->urlEntry[i].data, 256);
		}
	}
}

bool loadM3U(char *fName, PLS_TYPE *outData)
{
	char str[513];
	char lastTitle[512];
	bool isExt = false;
	
	outData->descriptionEntry = NULL;
	outData->urlEntry = NULL;
	outData->numEntries = 0;
	
	DRAGON_FILE *fp = DRAGON_fopen(fName, "r");
	
	memset(lastTitle, 0, 512);
	memset(str, 0, 513);
	DRAGON_fgets(str, 512, fp);
	
	if(strncmp(str, "#EXTM3U", 7) == 0)
		isExt = true;
	
	while(1)
	{
		if(str[0] != '#') // ignore comments
		{
			if(outData->numEntries == 0)
			{
				outData->descriptionEntry = (DCHAR *)trackMalloc(sizeof(DCHAR),"pls description");
				outData->urlEntry = (DCHAR *)trackMalloc(sizeof(DCHAR),"pls url");
			}
			else
			{
				outData->descriptionEntry = (DCHAR *)trackRealloc(outData->descriptionEntry, sizeof(DCHAR) * (outData->numEntries + 1));
				outData->urlEntry = (DCHAR *)trackRealloc(outData->urlEntry, sizeof(DCHAR) * (outData->numEntries + 1));
			}
			
			memset(&outData->descriptionEntry[outData->numEntries], 0, sizeof(DCHAR));
			memset(&outData->urlEntry[outData->numEntries], 0, sizeof(DCHAR));
			
			char *tmpStr = str;
			
			if(isURL(tmpStr))
				strcpy(outData->urlEntry[outData->numEntries].data, tmpStr);
			else
			{
				if(tmpStr[0] == '/' || tmpStr[0] == '\\') // assume its an absolute directory
					strcpy(outData->urlEntry[outData->numEntries].data, tmpStr);					
				else
				{
					// check first to see if we have a drive letter
					
					if(((tmpStr[0] >= 'A' && tmpStr[0] <= 'Z') || (tmpStr[0] >= 'a' && tmpStr[0] <= 'z')) && tmpStr[1] == ':')
						tmpStr+=2; // skip drive
					
					if(tmpStr[0] == '/' || tmpStr[0] == '\\') // assume its an absolute directory
						strcpy(outData->urlEntry[outData->numEntries].data, tmpStr);					
					else
						sprintf(outData->urlEntry[outData->numEntries].data, "%s%s", getLastDir(), tmpStr);
				}
				
				// change windows \ to unix-friendly /
				
				for(int i=0;i<(int)strlen(outData->urlEntry[outData->numEntries].data);i++)
				{
					if(outData->urlEntry[outData->numEntries].data[i] == '\\')
						outData->urlEntry[outData->numEntries].data[i] = '/';
				}
			}
			
			if(isExt && strlen(lastTitle) > 0)
			{
				strcpy(outData->descriptionEntry[outData->numEntries].data, lastTitle);	
				memset(lastTitle, 0, 512);
			}	
			else
			{
				if(isURL(outData->urlEntry[outData->numEntries].data))
					strcpy(outData->descriptionEntry[outData->numEntries].data,outData->urlEntry[outData->numEntries].data);
				else
				{
					// create title out of name
					char ext[512];
					
					memset(str, 0, 513);
					strcpy(str, outData->urlEntry[outData->numEntries].data);
					
					separateExtension(str,ext);
					
					char *lastStr = str;
					int i = 0;
					
					while(str[i] != 0)
					{
						if(str[i] == '/')
							lastStr = &str[i]+1;
						
						i++;
					}
					
					strcpy(outData->descriptionEntry[outData->numEntries].data,lastStr);
				}
			}
			
			outData->numEntries++;
		}
		else
		{
			if(isExt) // parse it for the file title
			{
				memset(lastTitle, 0, 512);
				if(strncmp(str, "#EXTINF", 7) == 0)
				{	
					char *tmpStr = strchr(str, ',');
					
					if(tmpStr)
						strcpy(lastTitle, tmpStr+1);
				}
			}
		}
		
		if(DRAGON_feof(fp))
		{
			DRAGON_fclose(fp);
			fillBlanks(outData);
			return true;
		}
		
		memset(str, 0, 513);
		DRAGON_fgets(str, 512, fp);
	}
}

bool loadPlaylist(char *fName, PLS_TYPE *outData)
{
	char str[513];
	int numEntries = 0;
	
	outData->descriptionEntry = NULL;
	outData->urlEntry = NULL;
	
	DRAGON_FILE *fp = DRAGON_fopen(fName, "r");
	
	memset(str, 0, 513);
	DRAGON_fgets(str, 512, fp);
	strlwr(str);
	
	if(strncmp(str, "[playlist]", 10) != 0)
	{
		DRAGON_fclose(fp);
		return loadM3U(fName, outData); // assume its either a misnamed m3u or a raw list
	}
	
	// its a pls
	
	while(numEntries == 0)
	{	
		if(DRAGON_feof(fp))
		{
			DRAGON_fclose(fp);
			return false;
		}
		
		memset(str, 0, 513);
		DRAGON_fgets(str, 512, fp);
		strlwr(str);
		
		if(strncmp(str, "numberofentries=", 16) == 0)
		{			
			numEntries = 0;
			
			// we got it
			for(int i=16;i<(int)strlen(str);i++)
			{
				if(isNumber(str[i]))
				{
					numEntries *= 10;
					numEntries += (str[i] - '0');
				}
			}
			
			DRAGON_fclose(fp);
		}
	}
	
	outData->numEntries = numEntries;
	
	outData->descriptionEntry = (DCHAR *)trackMalloc(numEntries * sizeof(DCHAR),"pls description");
	outData->urlEntry = (DCHAR *)trackMalloc(numEntries * sizeof(DCHAR),"pls url");
	memset(outData->descriptionEntry, 0, numEntries * sizeof(DCHAR));
	memset(outData->urlEntry, 0, numEntries * sizeof(DCHAR));
	
	fp = DRAGON_fopen(fName, "r");
	
	while(1)
	{	
		if(DRAGON_feof(fp))
		{
			DRAGON_fclose(fp);
			fillBlanks(outData);
			return true;
		}
		
		memset(str, 0, 513);
		DRAGON_fgets(str, 512, fp);
		
		char leftFour[5];
		strncpy(leftFour, str, 4);
		leftFour[4] = 0;
		strlwr(leftFour);
		
		int whichOne = 0;
		
		if(strcmp(leftFour, "titl") == 0) // description field
		{
			for(int i=5;str[i] != '=';i++)
			{
				if(isNumber(str[i]))
				{
					whichOne *= 10;
					whichOne += (str[i] - '0');
				}
			}
			
			if(whichOne <= numEntries) // we are in bounds
			{
				char *tmpStr = strchr(str, '=') + 1;
				
				strcpy(outData->descriptionEntry[whichOne-1].data, tmpStr);
			}
		}
		
		if(strcmp(leftFour, "file") == 0) // url field
		{
			for(int i=4;str[i] != '=';i++)
			{
				if(isNumber(str[i]))
				{
					whichOne *= 10;
					whichOne += (str[i] - '0');
				}
			}
			
			if(whichOne <= numEntries) // we are in bounds
			{
				char *tmpStr = strchr(str, '=') + 1;
				
				if(isURL(tmpStr))
					strcpy(outData->urlEntry[whichOne-1].data, tmpStr);
				else
				{
					if(tmpStr[0] == '/' || tmpStr[0] == '\\') // assume its an absolute directory
						strcpy(outData->urlEntry[whichOne-1].data, tmpStr);					
					else
					{
						// check first to see if we have a drive letter
						
						if(((tmpStr[0] >= 'A' && tmpStr[0] <= 'Z') || (tmpStr[0] >= 'a' && tmpStr[0] <= 'z')) && tmpStr[1] == ':')
							tmpStr+=2; // skip drive
						
						if(tmpStr[0] == '/' || tmpStr[0] == '\\') // assume its an absolute directory
							strcpy(outData->urlEntry[whichOne-1].data, tmpStr);					
						else
							sprintf(outData->urlEntry[whichOne-1].data, "%s%s", getLastDir(), tmpStr);
					}
					
					// change windows \ to unix-friendly /
					
					for(int i=0;i<(int)strlen(outData->urlEntry[whichOne-1].data);i++)
					{
						if(outData->urlEntry[whichOne-1].data[i] == '\\')
							outData->urlEntry[whichOne-1].data[i] = '/';
					}
				}
			}
		}
	}
}

void destroyPlaylist(PLS_TYPE *outData)
{
	if(outData->descriptionEntry != NULL)
		trackFree(outData->descriptionEntry);
	if(outData->urlEntry != NULL)
		trackFree(outData->urlEntry);
	
	outData->descriptionEntry = NULL;
	outData->urlEntry = NULL;
	outData->numEntries = 0;
}

