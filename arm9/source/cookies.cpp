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
#include "cookies.h"
#include "fatwrapper.h"
#include "globals.h"
#include "general.h"
#include "shortcuts.h"
#include "http.h"
#include "errors.h"
#include "wifi.h"

static COOKIE_TYPE *cookies = NULL;
static int numCookies = 0;

static char *cookieString = "Cookie :";
static char *cookieDelim = "; ";
static char *defaultPath = "/";
static char *TLDList[MAX_TLD] = { ".com", ".net", ".org", ".edu", ".gov", ".mil", ".aero", ".arpa", ".asia", ".biz", ".cat", ".coop", ".info", ".int", ".jobs", ".mobi", ".museum", ".name", ".pro", ".tel", ".travel" };

bool isReverseMatch(char *searchDomain, char *matchDomain)
{
	int sD = strlen(searchDomain);
	int mD = strlen(matchDomain);

	if(sD < mD || mD == 0)
	{
		// can't have match domain in search domain if matchdomain is longer
		// or if match domain isn't set
		return false;
	}

	// The check also checks the terminator, so therefor <=
	for(int x = 0; x <= mD; x++)
	{
		if(makeLower(searchDomain[sD-x]) != makeLower(matchDomain[mD-x]))
		{
			// They don't match for this position
			return false;
		}
	}

	return true;
}

int countDots(char *str)
{
	int dotsCount = 0;
	
	for(u32 x = 0; x < strlen(str); x++)
	{
		if(str[x] == '.')
		{
			// Add to dot count
			dotsCount++;
		}
	}
	
	// Return final count
	return dotsCount;
}

bool isTLD(char *str)
{
	for(int x = 0; x < MAX_TLD; x++)
	{
		if(isReverseMatch(str,TLDList[x]))
		{
			// It's a match with one of them
			return true;
		}
	}

	// Never did match
	return false;
}

bool containsEmbeddedDot(char *str)
{
	int dCount = countDots(str);
	
	if(dCount <= 1)
	{
		// more dots, more dots!
		return false;
	}
	
	if(dCount >= 3)
	{
		// it's guaranteed to be safe
		return true;
	}
	
	// if it's two dots, then verify that it's a proper tld
	return isTLD(str);
}

bool containsLeadingDot(char *str)
{
	return *str == '.';
}

bool isValidDomain(char *domain)
{
	// Ensure that it contains both a leading dot...
	if(!containsLeadingDot(domain))
	{
		return false;
	}
	
	// ... and at least one embedded dot (or two if it's not a special domain)
	return containsEmbeddedDot(domain);
}

bool doesHostMatch(char *requestHost, char *domain)
{
	// More tight requirements as to the match
	if(!isReverseMatch(requestHost, domain))
	{
		// Can't possibly due to domains not even matching
		return false;
	}
	
	// Ensure that the remaining bits don't have a dot in them
	int length = strlen(requestHost) - strlen(domain);

	if(length == 0)
	{
		return true;
	}

	char h[256];
	memset(h, 0, 256);
	strncpy(h, requestHost, length);

	if(h[0] == '.')
	{
		// Was added in
		h[0] = '_';
	}

	// Ensure no dots in the domain remaining
	if(countDots(h) > 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool doesPathMatch(char *requestPath, char *path)
{
	return strstr(requestPath,path) != NULL;
}

void insertCookie(char *domain, char *path, char *data)
{
	if(*data == '$')
	{
		// Cookie names cannot have a $ as this is reserved for non-application use
		return;
	}
	
	// Get name portion of data
	int nameLength = 0;
	
	while(data[nameLength] != '=')
	{
		nameLength++;
	}
	
	// Figure out whether this cookie domain, path, and name pair have been set already
	for(int i = 0; i < numCookies; i++)
	{
		if(!areEqual(domain, cookies[i].domain, MAX_DOMAIN))
			continue;
		if(!areEqual(path, cookies[i].path, MAX_PATH))
			continue;
		if(!areEqual(data, cookies[i].name, (nameLength > MAX_NAME) ? MAX_NAME : nameLength))
			continue;
		
		// The cookie is just an update for the original value
		char cookieFile[256];
		sprintf(cookieFile, "%s%08X.CKY", d_cookies, cookies[i].checksum);
		
		if(DRAGON_FileExists(cookieFile) != FE_NONE)
		{
			DRAGON_remove(cookieFile);
		}
		
		// Write the new cookie data
		DRAGON_FILE *df = DRAGON_fopen(cookieFile, "w");
		DRAGON_fwrite(data, 1, strlen(data), df);
		DRAGON_fclose(df);
		
		// Job's done!
		return;
	}
	
	// We have to add a new cookie to the list
	if(!cookies)
	{
		setGlobalError(ERROR_COOKIES_CREATE);
		cookies = (COOKIE_TYPE *)trackMalloc(sizeof(COOKIE_TYPE), "cookies");
		setGlobalError(ERROR_NONE);
		
		memset(cookies, 0, sizeof(COOKIE_TYPE));
		numCookies = 1;
	}
	else
	{
		numCookies++;
		
		cookies = (COOKIE_TYPE *)trackRealloc(cookies, sizeof(COOKIE_TYPE) * numCookies);
		memset(&cookies[numCookies - 1], 0, sizeof(COOKIE_TYPE));
	}
	
	// Now copy in all the elements
	strncpy(cookies[numCookies - 1].domain, domain, MAX_DOMAIN);
	strncpy(cookies[numCookies - 1].path, path, MAX_PATH);
	strncpy(cookies[numCookies - 1].name, data, (nameLength > MAX_NAME) ? MAX_NAME : nameLength);
	
	// Compute the checksum
	{
		char tData[256];
		strcpy(tData, cookies[numCookies - 1].domain);
		strcat(tData, cookies[numCookies - 1].path);
		strcat(tData, cookies[numCookies - 1].name);
		
		cookies[numCookies - 1].checksum = CalcCRC32(tData);
	}
	
	// Write data out to file
	char cookieFile[256];
	sprintf(cookieFile, "%s%08X.CKY", d_cookies, cookies[numCookies - 1].checksum);
	
	if(DRAGON_FileExists(cookieFile) != FE_NONE)
	{
		DRAGON_remove(cookieFile);
	}
	
	// Write the new cookie data
	DRAGON_FILE *df = DRAGON_fopen(cookieFile, "w");
	DRAGON_fwrite(data, 1, strlen(data), df);
	DRAGON_fclose(df);
}

void addCookie(char *url, char *header)
{
	// Grab the domain in case they don't specify one
	URL_TYPE tURL;
	
	if(!urlFromString(url, &tURL))
	{
		return;
	}
	
	if(strstr(header, "Set-Cookie") == NULL)
	{
		destroyURL(&tURL);
		return;
	}
	
	// Now loop through the header, finding wherever there's a set-cookie request
	while(!areEqual(header, "\r\n", 2) && *header != 0)
	{	
		if(areEqual(header, "Set-Cookie: ", 12))
		{
			// The header is now positioned at the first data byte
			header += 12;
			
			// This is the value/data pair here			
			int dataLength = 0;
			
			while(header[dataLength] != ';' && header[dataLength] != '\r' && header[dataLength] != 0)
			{
				dataLength++;
			}
			
			// Grab actual data
			char *tData = (char *)safeMalloc(dataLength + 1);
			memcpy(tData, header, dataLength);
			
			if(header[dataLength] == '\r')
			{
				// Nothing else for the cookie, so assume defaults
				insertCookie(tURL.server, defaultPath, tData);
				
				// Skip past the data and the end
				header += dataLength + 2;
			}
			else
			{
				// There is other stuff to parse here
				char *tPath = NULL;
				char *tDomain = NULL;
				
				// Skip past the cookie data
				header += dataLength;
				
				// Skip any additional spaces
				while(*header == ';' || *header == ' ')
				{
					header ++;
				}
				
				// Parse until the end
				while(!areEqual(header, "\r\n", 2))
				{
					if(areEqual(header, "Domain=", 7))
					{
						// Set the domain
						header += 7;
						
						dataLength = 0;
						
						while(header[dataLength] != ';' && header[dataLength] != '\r' && header[dataLength] != 0)
						{
							dataLength++;
						}
						
						// Grab actual domain
						tDomain = (char *)safeMalloc(dataLength + 1);
						memcpy(tDomain, header, dataLength);
						
						header += dataLength;
					}
					else if(areEqual(header, "Path=", 5))
					{
						// Set path
						header += 5;
						
						dataLength = 0;
						
						while(header[dataLength] != ';' && header[dataLength] != '\r' && header[dataLength] != 0)
						{
							dataLength++;
						}
						
						// Grab actual domain
						tPath = (char *)safeMalloc(dataLength + 1);
						memcpy(tPath, header, dataLength);
						
						header += dataLength;
					}
					else if(areEqual(header, "secure", 5))
					{
						header += 6;
					}
					else
					{
						// We don't parse anything else
						while(*header != ';' && *header != '\r' && *header != 0)
						{
							// Advance until we hit the end of the token
							header++;
						}
					}
					
					// Skip any additional spaces
					while(*header == ';' || *header == ' ')
					{
						header++;
					}
				}
				
				// Set defaults if needed
				char *sPath;
				
				if(tPath == NULL)
				{
					sPath = defaultPath;
				}
				else
				{
					sPath = tPath;
				}
				
				if(tDomain == NULL)
				{
					insertCookie(tURL.server, sPath, tData);
				}
				else
				{
					// Ensure they can only set the cookie if it's valid
					if(isValidDomain(tDomain))
					{
						insertCookie(tDomain, sPath, tData);
						return;
					}
				}
				
				// Set the cookie
				
				// Free memory
				if(tPath)
				{
					free(tPath);
				}
				
				if(tDomain)
				{
					free(tDomain);
				}
				
				// Advance past the end of the line
				header += 2;
			}
			
			// free data memory
			free(tData);
		}
		else
		{
			// Advance to the end
			while(!areEqual(header, "\r\n", 2) && *header != 0)
			{
				header++;
			}
			
			
			if(*header != 0)
			{
				// Advance past the end
				header += 2;
			}
		}
	}
	
	destroyURL(&tURL);
}

void sendCookies(int sock, char *server, char *path)
{
	// Remember if we even sent a cookie
	bool hasFound = false;
	
	// Figure out whether this cookie domain, path, and name pair have been set already
	for(int i = 0; i < numCookies; i++)
	{
		//writeDebug("%s %s", server, cookies[i].domain);
		//writeDebug("%s %s", path, cookies[i].path);
		
		// Figure out if it matches
		if(!doesHostMatch(server, cookies[i].domain))
			continue;
		if(!doesPathMatch(path, cookies[i].path))
			continue;
		
		// The domain and path are a match, send this cookie
		if(!hasFound)
		{
			sendData(sock, cookieString, strlen(cookieString));
			hasFound = true;
		}
		else
		{
			sendData(sock, cookieDelim, strlen(cookieDelim));
		}
		
		//writeDebug("Sent!");
		
		// Now open the file, read in the contents, and send it
		char cookieFile[256];
		sprintf(cookieFile, "%s%08X.CKY", d_cookies, cookies[i].checksum);
		
		if(DRAGON_FileExists(cookieFile) == FE_FILE)
		{
			DRAGON_FILE *df = DRAGON_fopen(cookieFile, "r");
			
			u32 fLength = DRAGON_flength(df);			
			char *tCookie = (char *)malloc(fLength); // No need for null as senddata takes in a length parameter
			
			DRAGON_fread(tCookie, 1, fLength, df);
			DRAGON_fclose(df);
			
			// Actually send
			sendData(sock, tCookie, fLength);
			
			free(tCookie);
		}
	}
	
	// Only add a line terminator if we sent cookie data
	if(hasFound)
	{
		sendString(sock, "\r\n");
	}
}
