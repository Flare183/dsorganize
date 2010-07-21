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
#include <stdlib.h>
#include <libfb/libcommon.h>
#include <libdt/libcommon.h>
#include "irc.h"
#include "webbrowser.h"
#include "fatwrapper.h"
#include "colors.h"
#include "general.h"
#include "fonts.h"
#include "globals.h"
#include "settings.h"
#include "wifi.h"
#include "graphics.h"
#include "keyboard.h"
#include "http.h"
#include "resources.h"
#include "sidetabs.h"
#include "sound.h"
#include "language.h"
#include "controls.h"
#include "errors.h"
#include "help.h"

static char backBuffer[6][MAX_INPUT+1];
static int backPlace = 0;
static int ircMode;
static SCREEN_BUFFER *console = NULL;
static TAB_ARRAY *tabs = NULL;
static int tabCount;
static int activeTab;
static int ircSocket = 0;
static bool connected = false;
static char *serverBuffer = NULL;
static char *tcpTemp = NULL;
static int connectMode = 0;
static char curServer[128];
static char quitMessage[256];
static bool isTopic = false;
static char displayNick[NICK_LEN];
static const uint16 ircColors[16] = { 0xFFFF, 0x0, 0x4000, 0x0240, 0x001F, 0x000F, 0x4C13, 0x01FF, 0x03FF, 0x03E0, 0x4A40, 0x7FE0, 0x7C00, 0x7C1F, 0x3DEF, 0x6B5A };
static uint16 ircColors2[16] = { 0x0, 0x0, 0x4000, 0x0240, 0x001F, 0x000F, 0x4C13, 0x01FF, 0x03FF, 0x03E0, 0x4A40, 0x7FE0, 0x7C00, 0x7C1F, 0x3DEF, 0x6B5A };
static char ircModes[10];
static char ircSymbols[10];
static bool reconnect = false;
static char lastServer[60];
static char myModes[20];
static char curNetwork[128];
static bool queuedEnter = false;
static int maxNickLen = 0;
static char useNickName[NICK_LEN];
static char usealtNickName[NICK_LEN];
static bool toPerform = false;
static int colorCursor = -1;
static int colorCursorCount = 0;
static bool notifyPresent = false;
static int ircLines = 13;
static u32 ircCount = 0;
static int waitCount = 0;
static bool cancelOnWait = false;
static bool ignoreCommands = false;

char *inputBuffer = NULL;
bool strUpdated = false;
int textBlock = 0;
char *pointedText = NULL;

extern char nickName[NICK_LEN];
extern char altnickName[NICK_LEN];
extern char ircServer[60];
extern bool autoConnect;

// prototypes
void closeServerConnection();
int scrollOffset(int tab);
void concatTab(int tab, char *toAdd, char *colorAdd, int importance);
void connectToServer(char *s);
void createIRC();
void drawTopIRCScreen();
void drawBottomIRCScreen();
void ircUp();
void ircDown();
void createIRCButtons();
void destroyRecvBuffer();
void ircStartPressed(int x, int y);
void ircForward(int x, int y);
void ircBack(int x, int y);
void ircLButton(int x, int y);
void ircRButton(int x, int y);
int getIRCNickCount();
void editIRCAction(char c);

int inputLength()
{
	return strlen(pointedText);
}

void incrimentIRCCount()
{
	ircCount++;
	
	if(cancelOnWait)
		waitCount++;
}

bool isChar(char a, char b)
{
	if(a >= 'a' && a <= 'z')
		a -= ('a' - 'A');
	if(b >= 'a' && b <= 'z')
		b -= ('a' - 'A');
	
	return a == b;
}

void addIfValidURL(char *buf)
{	
	if(isChar(buf[0], 'h') && isChar(buf[1], 't') && isChar(buf[2], 't') && isChar(buf[3], 'p')) // starts with http
	{
		if(!strstr(buf, "//")) // make sure it's either http:// or https://
			return;
		
		if(strchr(strstr(buf, "//") + 2, '.'))
			addURLToCapture(buf); // it's valid
		
		return; // don't test afterwards
	}
	
	if(isChar(buf[0], 'w') && isChar(buf[1], 'w') && isChar(buf[2], 'w') && isChar(buf[3], '.')) // starts with www.
	{
		if(strchr(buf + 4, '.')) // maie sure it contains a top level domain
			addURLToCapture(buf); // it's valid
	}
}

void findURLs(char *buf)
{
	if(!buf)
		return;
	
	bool inURL = false;
	int urlCount = 0;
	setGlobalError(ERROR_IRC_FINDURL);
	char *tURL = (char *)safeMalloc(MAX_URL_IRC + 1); // checked
	setGlobalError(ERROR_NONE);
	
	while(*buf)
	{	
		if(!inURL)
		{
			if(isChar(buf[0], 'h') && isChar(buf[1], 't') && isChar(buf[2], 't') && isChar(buf[3], 'p'))
			{
				// probably a url
				if(isChar(buf[4], ':'))
					inURL = true;
				if(isChar(buf[4], 's') && isChar(buf[5], ':'))
					inURL = true;
			}
			
			if(isChar(buf[0], 'w') && isChar(buf[1], 'w') && isChar(buf[2], 'w') && isChar(buf[3], '.'))
				inURL = true;
		}
		
		if(inURL)
		{
			if(buf[0] == ' ' || buf[0] == '	' || buf[0] == '\r' || buf[0] == '\n')
			{
				addIfValidURL(tURL);
				memset(tURL, 0, MAX_URL_IRC + 1);
				
				inURL = false;
				urlCount = 0;
			}
			else
			{
				if(urlCount < MAX_URL_IRC)
				{
					urlCount++;
					strccat(tURL, *buf);
				}
			}
		}		
		
		buf++;
	}
	
	if(inURL) // catch urls at the end of the line
	{
		addIfValidURL(tURL);
	}
	
	free(tURL);
}

void makeNormal(char *str, int len)
{	
	memset(str, ' ', len);
	str[len] = 0;
}

char makeCommand(bool b, bool u, int color)
{
	if(color < 0)
		color = 0;
	if(color > 15)
		color = 15;
	
	char c = (char)(color & 0xF);
	
	if(b == false)
		c |= 0x20;
	if(u == true)
		c |= 0x10;
	
	c |= 0x80; // stop it from fuckin' up
	
	return c;
}

void concatCommand(char *str, char c, int amount)
{
	if(amount > 0)
	{
		memset(str, c, amount);
		str[amount] = 0;
	}
}

bool setNetwork(char *msg)
{	
	if(strstr(msg, "NETWORK=") == NULL)
		return false;

	memset(curNetwork,0,128);	
	char *network = strstr(msg, "NETWORK=") + 8;
	
	while(network[0] != ' ' && network[0] != 0)
	{
		curNetwork[strlen(curNetwork)] = network[0];
		network++;
	}
	
	if(strlen(curNetwork) != 0)
		return true;
	
	return false;
}

bool setNickLen(char *msg)
{
	if(strstr(msg, "NICKLEN=") == NULL)
		return false;
	
	char *nicklen = strstr(msg, "NICKLEN=") + 8;
	maxNickLen = 0;
	
	while(nicklen[0] != ' ' && nicklen[0] != 0)
	{
		if(nicklen[0] >= '0' && nicklen[0] <= '9')
		{
			maxNickLen *= 10;
			maxNickLen += nicklen[0] - '0';
		}
		
		nicklen++;
	}
	
	if(maxNickLen != 0)
		return true;
	
	return false;
}

bool parseModes(char *modes)
{
	if(strstr(modes, "PREFIX=") == NULL)
		return false;
	
	if(strstr(modes, "PREFIX=")[7] == '(')
	{	
		char *prefix = strstr(modes, "PREFIX=") + 8; // skip the opening (
		
		memset(ircModes, 0, 10);
		memset(ircSymbols, 0, 10);
		
		while(*prefix != ')')
		{
			ircModes[strlen(ircModes)] = *prefix;
			prefix++;
		}
		
		prefix++; // skip closing )
		
		while(*prefix != ' ')
		{
			ircSymbols[strlen(ircSymbols)] = *prefix;
			prefix++;
		}
		
		return true;
	}
	else
	{
		// for some stupid reason, this irc server doesn't conform to standards and just sent a list of the mode symbols
		
		char *prefix = strstr(modes, "PREFIX=") + 7; // skip the prefix part
		
		memset(ircModes, 0, 10);
		memset(ircSymbols, 0, 10);
		
		while(*prefix != ' ') // we are going to assume a few things here, such as what symbol goes to what mode, to make things work
		{
			ircSymbols[strlen(ircSymbols)] = *prefix;
			
			switch(*prefix)
			{
				case '@':
					ircModes[strlen(ircModes)] = 'o';
					break;
				case '%':
					ircModes[strlen(ircModes)] = 'h';
					break;
				case '+':
					ircModes[strlen(ircModes)] = 'v';
					break;
				default: // we're sol
					ircModes[strlen(ircModes)] = ' ';
					break;
			}
			
			prefix++;
		}
		
		return true;
	}
}

void makeModes(uint16 modes, char *outModes)
{
	for(u32 i=0;i<strlen(ircSymbols);i++)
	{
		if((modes & BIT(i)) != 0)
			outModes[strlen(outModes)] = ircSymbols[i];
	}
}

uint16 highestMode(uint16 modes) // returns a number for the first bit set, 0 being highest, strlen of ircsymbols as lowest (no mode set)
{
	for(u32 i=0;i<strlen(ircSymbols);i++)
	{
		if((modes & BIT(i)) != 0)
			return i;
	}
	
	return strlen(ircSymbols);
}

char getCurrentMode(SCREEN_BUFFER *tmp, char *nick)
{
	if(tmp->nickCount == 0)
		return 0;
	
	for(u32 i=0;i<tmp->nickCount;i++)
	{
		if(areEqual(tmp->nicks[i].nick, nick, COMPARE_INFINITE))
		{
			uint16 mode = highestMode(tmp->nicks[i].modes);
			
			if(mode == strlen(ircSymbols))
				return 0;
			else
				return ircSymbols[mode];
		}
	}
	
	return 0;
}

uint16 getModes(char *nick)
{
	uint16 modes = 0;
	
	while(true)
	{
		if(strchr(ircSymbols, nick[0]) != NULL)
		{
			int place = (strchr(ircSymbols, nick[0]) - ircSymbols); // how far in it is
			modes |= BIT(place);
			nick++;
		}
		else
			return modes;
	}
	
	return 0;
}

void changeModes(SCREEN_BUFFER *tmp, char *nick, char mode, int modeDir)
{
	if(tmp->nickCount == 0)
		return;
	
	for(u32 i=0;i<tmp->nickCount;i++)
	{
		if(areEqual(tmp->nicks[i].nick, nick, COMPARE_INFINITE))
		{
			int place = (strchr(ircModes, mode) - ircModes); // how far in it is
			if(modeDir == -1)
				tmp->nicks[i].modes ^= BIT(place);
			else if(modeDir == 1)
				tmp->nicks[i].modes |= BIT(place);
			
			return;		
		}
	}	
}

void appendMode(char *modeStr, char c, int modeDir)
{
	switch(modeDir)
	{
		case -1: // remove mode
			if(strchr(modeStr, c) != NULL) // mode is set, can be removed
			{
				int pos = (strchr(modeStr, c) - modeStr);
				
				while(modeStr[pos] != 0)
				{
					modeStr[pos] = modeStr[pos+1];
					pos++;
				}
			}
			
			break;
		case 1:
			if(strchr(modeStr, c) == NULL) // mode is unset, can be added
			{
				if(strlen(modeStr) == 0) // just add it
				{
					modeStr[0] = c;
					modeStr[1] = 0;
				}
				else
				{
					int pos = 0;
					bool added = false;
					
					while(modeStr[pos] != 0 && !added)
					{
						if(c < modeStr[pos])
						{
							int x = strlen(modeStr);
							
							while(x > pos)
							{
								modeStr[x] = modeStr[x-1];
								x--;
							}
							
							modeStr[pos] = c;
							added = true;
						}
						
						pos++;
					}
					
					if(!added)
						modeStr[strlen(modeStr)] = c;
				}
			}
			
			break;
	}
}

void formatText(char *text, char *color, bool defBold, bool defUnderline, int defColor)
{
	int curColor = defColor;
	bool bold = false;
	bool underline = false;
	
	int i = 0;
	
	while(text[i] != 0)
	{
		int offset = 0;
		
		if(text[i] == CONTROL_RESET)
		{
			bold = false;
			underline = false;
			curColor = defColor;
			offset = 1;
		}
		else if(text[i] == CONTROL_BOLD)
		{
			bold = !bold;
			offset = 1;
		}
		else if(text[i] == CONTROL_UNDERLINE)
		{
			underline = !underline;
			offset = 1;
		}
		else if(text[i] == CONTROL_COLOR)
		{
			offset = 1;
			
			if(!isNumerical(text[i+1])) // end color
				curColor = defColor;
			else
			{
				curColor = text[i+1] - '0';
				offset++;
				
				if(isNumerical(text[i+2]))
				{
					curColor *= 10;
					curColor += text[i+2] - '0';
					offset++;
					
					if(text[i+3] == ',') // ignore background colors
					{
						offset++;
						if(isNumerical(text[i+4]))
						{
							offset++;
							if(isNumerical(text[i+5]))
								offset++;
						}
					}
				}
				else
				{
					if(text[i+2] == ',') // ignore background colors
					{
						offset++;
						if(isNumerical(text[i+3]))
						{
							offset++;
							if(isNumerical(text[i+4]))
								offset++;
						}
					}
				}
			}
		}
		
		color[i] = makeCommand((defBold || bold), (defUnderline || underline), curColor);
		
		if(offset > 0)
		{
			int j = i;
			
			while(text[j] != 0)
			{
				text[j] = text[j+offset];
				j++;
			}
		}
		
		if(offset == 0)
			i++;
	}
	
	color[i] = 0;
}

int ircCallback(int pos, u32 c, int pass, int xpos, int ypos)
{
	if(c == 0)
		return 0;
	
	char *cmd = tabs[activeTab].screen->colorBuffer + scrollOffset(activeTab);
	char command = cmd[pos];	
	
	setColor(ircColors2[(int)(command & 0xF)]);
	
	if((command & 32) != 0) // bold is off
		setBold(false);
	else
		setBold(true);
	
	if((command & 16) != 0) // underline is on
	{
		if(c >= 32)
			setUnderline(true);
		else
			setUnderline(false);
	}
	else
		setUnderline(false);
	
	return 0;
}

SCREEN_BUFFER *createBuffer(int x, int y, int w, int h)
{
	setGlobalError(ERROR_IRC_SCREENBUFFER);
	SCREEN_BUFFER *tmp = (SCREEN_BUFFER *)trackMalloc(sizeof(SCREEN_BUFFER), "screen buffer"); // checked
	setGlobalError(ERROR_NONE);
	
	setGlobalError(ERROR_IRC_BACKBUFFER);
	tmp->buffer = (char *)trackMalloc(100, "screen buffer buffer"); // checked
	setGlobalError(ERROR_IRC_COLORBUFFER);
	tmp->colorBuffer = (char *)trackMalloc(100, "screen color buffer"); // checked
	setGlobalError(ERROR_NONE);
	
	memset(tmp->buffer, 0, 100);
	memset(tmp->colorBuffer, ' ', 100);
	memset(tmp->modes, 0, 20);
	tmp->length = 100;
	
	tmp->nicks = NULL;
	tmp->nickCount = 0;
	
	tmp->crPts = NULL;
	tmp->numPts = 0;
	tmp->curPt = 0;
	tmp->maxPt = 0;
	
	tmp->x = x;
	tmp->y = y;
	tmp->w = w;
	tmp->h = h;
	
	tmp->autoScroll = true;
	tmp->hasKey = false;
	tmp->hasLimit = false;
	
	return tmp;
}

void destroyBuffer(SCREEN_BUFFER *tmp)
{
	trackFree(tmp->buffer);
	tmp->buffer = NULL;
	trackFree(tmp->colorBuffer);
	tmp->colorBuffer = NULL;
	
	if(tmp->nicks != NULL)
		trackFree(tmp->nicks);
	tmp->nicks = NULL;
	
	if(tmp->crPts != NULL)
		trackFree(tmp->crPts);
	tmp->crPts = NULL;
	
	tmp->numPts = 0;
	tmp->nickCount = 0;	
	tmp->length = 0;
	
	trackFree(tmp);
}

void clearBuffer(SCREEN_BUFFER *tmp)
{
	memset(tmp->buffer, 0, tmp->length);
	memset(tmp->colorBuffer, ' ', tmp->length);
	
	if(tmp->crPts != NULL) // get ridda points because the screen is clear
		trackFree(tmp->crPts);
	tmp->crPts = NULL;
	
	tmp->numPts = 0;
	tmp->curPt = 0;
}

void concatBuffer(SCREEN_BUFFER *tmp, char *toAdd, char *colorAdd)
{
	if(tmp == NULL)
		return;
	if(toAdd == NULL)
		return;	
	if(strlen(toAdd) == 0)
		return;
	
	if(strlen(toAdd) + strlen(tmp->buffer) >= tmp->length - 5)
	{
		int l = tmp->length + strlen(toAdd) + 50;
		tmp->buffer = (char *)trackRealloc(tmp->buffer, l);
		tmp->colorBuffer = (char *)trackRealloc(tmp->colorBuffer, l);
		
		assert(tmp->buffer == 0, "screen buffer is null");
		assert(tmp->colorBuffer == 0, "screen color buffer is null");
		
		memset(tmp->buffer + tmp->length, 0, l - tmp->length); // set to zero to make sure
		memset(tmp->colorBuffer + tmp->length, ' ', l - tmp->length); // set to normal to make sure
		
		tmp->length = l;
	}
	
	if(strlen(tmp->buffer) == 0 && (toAdd[0] == '\r' || toAdd[0] == '\n'))
	{
		toAdd++;
		colorAdd++;
		
		if(strlen(toAdd) == 0)
			return;
	}
	
	int i = strlen(tmp->buffer);
	strcat(tmp->buffer, toAdd);
	
	findURLs(toAdd);
	
	if(colorAdd)
	{
		int j = strlen(colorAdd);
		if(j > (int)strlen(toAdd))
			j = strlen(toAdd);
		
		memcpy(tmp->colorBuffer + i, colorAdd, j);
	}
	
	if(tmp->crPts != NULL)
		trackFree(tmp->crPts);
	tmp->crPts = NULL;
	
	tmp->numPts = getWrapPoints(0, 0, tmp->buffer, tmp->x, tmp->y, tmp->w, tmp->h, &tmp->crPts, setIRCFont());
	
	if(tmp->numPts > MAX_BUFFER_LINES)
	{
		char *toLoc = tmp->buffer;
		char *fromLoc = tmp->buffer + tmp->crPts[tmp->numPts-MAX_BUFFER_LINES];
		char *toLoc2 = tmp->colorBuffer;
		char *fromLoc2 = tmp->colorBuffer + tmp->crPts[tmp->numPts-MAX_BUFFER_LINES];
		
		int moveLen = strlen(tmp->buffer) - tmp->crPts[tmp->numPts-MAX_BUFFER_LINES];
		
		memmove(toLoc, fromLoc, moveLen);
		memmove(toLoc2, fromLoc2, moveLen);
		toLoc[moveLen] = 0;
		
		memset(toLoc2 + moveLen, ' ', tmp->length - moveLen);
		
		if(tmp->crPts != NULL)
			trackFree(tmp->crPts);
		tmp->crPts = NULL;
		
		tmp->numPts = getWrapPoints(0, 0, tmp->buffer, tmp->x, tmp->y, tmp->w, tmp->h, &tmp->crPts, setIRCFont());
	}	
	
	if(tmp->autoScroll)
	{
		tmp->curPt = tmp->numPts - ircLines;
		if(tmp->curPt < 0)
			tmp->curPt = 0;
	}
	
	tmp->maxPt = tmp->numPts - ircLines;
	if(tmp->maxPt < 0)
		tmp->maxPt = 0;
}

int compareModes(const void * a, const void * b)
{
	NICK_LIST *n1 = (NICK_LIST*)a;
	NICK_LIST *n2 = (NICK_LIST*)b;
	
	if(highestMode(n1->modes) < highestMode(n2->modes)) // n1 is higher
		return -1;
	
	if(highestMode(n1->modes) > highestMode(n2->modes)) // n2 is higher
		return 1;
	
	// they are equal
	
	char str1[NICK_LEN];
	char str2[NICK_LEN];
	
	strcpy(str1, n1->nick);
	strcpy(str2, n2->nick);
	strlwr(str1);
	strlwr(str2);
	
	return strcmp(str1, str2);
}

void sortNicks(int tab)
{
	if(tab == -1)
		return;
	
	if(tabs[tab].screen->nickCount > 1)	
		qsort(tabs[tab].screen->nicks, tabs[tab].screen->nickCount, sizeof(NICK_LIST), compareModes);
}

bool isNickInScreen(SCREEN_BUFFER *tmp, char *toAdd)
{
	if(tmp->nickCount == 0)
		return false;
	
	for(u32 i=0;i<tmp->nickCount;i++)
	{
		if(areEqual(tmp->nicks[i].nick, toAdd, COMPARE_INFINITE))
			return true;
	}
	
	return false;
}

void dispNicks(int tab)
{	
	if(tab == -1)
		return;
	
	char tStr[1024];
	char cStr[1024];
	
	sprintf(cStr, "\n%s", irc_nicksinchannel);
	osprintf(tStr, cStr);
	memset(cStr, 0, 1024);
	formatText(tStr, cStr, false, false, ircColorTOPIC);
	concatTab(tab, tStr, cStr, UNIMPORTANT);
	
	for(u32 i=0;i<tabs[tab].screen->nickCount;i++)
	{		
		sprintf(cStr, " %s", irc_nicklist);
		osprintf(tStr, cStr, getCurrentMode(tabs[tab].screen, tabs[tab].screen->nicks[i].nick), tabs[tab].screen->nicks[i].nick);
		memset(cStr, 0, 1024);
		concatCommand(cStr, makeCommand(false, false, ircColorTOPIC), strlen(tStr));
		concatTab(tab, tStr, cStr, UNIMPORTANT);
	}
}

void changeNick(SCREEN_BUFFER *tmp, char *toFind, char *toAdd)
{
	if(tmp->nickCount == 0)
		return;
	
	for(u32 i=0;i<tmp->nickCount;i++)
	{
		if(areEqual(tmp->nicks[i].nick, toFind, COMPARE_INFINITE))
		{
			bool loopIt = true;
			
			while(loopIt)
			{
				if(strchr(ircSymbols, toAdd[0]) != NULL)
					toAdd++;
				else
					loopIt = false;
			}
			
			strcpy(tmp->nicks[i].nick, toAdd);
			return;		
		}
	}
}

void subNick(SCREEN_BUFFER *tmp, char *toAdd)
{
	if(tmp->nickCount == 0)
		return;
	
	for(u32 i=0;i<tmp->nickCount;i++)
	{
		if(areEqual(tmp->nicks[i].nick, toAdd, COMPARE_INFINITE))
		{
			// here is the nick listing
			if(i == tmp->nickCount - 1) // last entry
			{
				if(tmp->nickCount == 1) // if theres only one, delete it
				{
					trackFree(tmp->nicks);
					tmp->nicks = NULL;
					tmp->nickCount = 0;					
				}
				else
				{
					tmp->nickCount--;
					tmp->nicks = (NICK_LIST *)trackRealloc(tmp->nicks, sizeof(NICK_LIST) * tmp->nickCount);
				}
			}
			else // gotta copy the other entries, cant be only entry but not last entry
			{
				memcpy(&tmp->nicks[i], &tmp->nicks[i+1], (sizeof(NICK_LIST))*(tmp->nickCount - (i + 1)));
				tmp->nickCount--;
				tmp->nicks = (NICK_LIST *)trackRealloc(tmp->nicks, sizeof(NICK_LIST) * tmp->nickCount);
			}
			
			return;
		}
	}
}

void addNick(SCREEN_BUFFER *tmp, char *toAdd)
{	
	if(isNickInScreen(tmp, toAdd)) // do nothing
		return;
	
	tmp->nickCount++;
	
	if(tmp->nicks == NULL) // create if it does not exist
	{
		setGlobalError(ERROR_IRC_NICKLIST);
		tmp->nicks = (NICK_LIST *)trackMalloc(sizeof(NICK_LIST) * tmp->nickCount, "nick list"); // checked
		setGlobalError(ERROR_NONE);
	}
	else
	{
		tmp->nicks = (NICK_LIST *)trackRealloc(tmp->nicks, sizeof(NICK_LIST) * tmp->nickCount); // checked
	}
	
	int i = tmp->nickCount - 1;
	tmp->nicks[i].modes = getModes(toAdd);
	
	bool loopIt = true;	
	
	while(loopIt)
	{
		if(strchr(ircSymbols, toAdd[0]) != NULL)
			toAdd++;
		else
			loopIt = false;
	}
	
	strcpy(tmp->nicks[i].nick, toAdd);	
}

void initBackBuffers()
{
	memset(backBuffer, 0, 6 * (MAX_INPUT+1));
	backPlace = 0;
}

void resetBackBuffers()
{
	memset(backBuffer[5], 0, MAX_INPUT+1);
	backPlace = 0;
}

void addBackBuffer()
{
	if(strlen(inputBuffer) == 0)
		return;
	
	for(int i=0;i<5;i++)
	{
		if(strcmp(backBuffer[i], inputBuffer) == 0)
			return; // don't copy if it exists already on the backbuffer
	}
	
	memset(backBuffer[5], 0, MAX_INPUT+1);
	memcpy(backBuffer[4], backBuffer[3], MAX_INPUT+1);
	memcpy(backBuffer[3], backBuffer[2], MAX_INPUT+1);
	memcpy(backBuffer[2], backBuffer[1], MAX_INPUT+1);
	memcpy(backBuffer[1], backBuffer[0], MAX_INPUT+1);
	memcpy(backBuffer[0], inputBuffer, MAX_INPUT+1);
	backPlace = 0;
}

void scrollUpBuffer()
{
	if(backPlace >= 5) // don't scroll past the end
		return;
	
	if(backPlace == 0) // we haven't scrolled up yet, store the current type in the temp location
		memcpy(backBuffer[5], inputBuffer, MAX_INPUT+1);
	
	if(strlen(backBuffer[backPlace]) == 0) // don't scroll if nothing else is left
		return;
	
	memcpy(inputBuffer, backBuffer[backPlace], MAX_INPUT+1);
	backPlace++;
	resetKBCursor();
}

void scrollDownBuffer()
{
	if(backPlace == 0) // don't scroll past the end
		return;
	
	backPlace--;
	resetKBCursor();
	if(backPlace == 0)
		memcpy(inputBuffer, backBuffer[5], MAX_INPUT+1);
	else
		memcpy(inputBuffer, backBuffer[backPlace-1], MAX_INPUT+1);
}

void createTabs()
{
	setGlobalError(ERROR_IRC_TABARRAY);
	tabs = (TAB_ARRAY *)trackMalloc(sizeof(TAB_ARRAY), "tab array"); // checked
	setGlobalError(ERROR_NONE);
	
	strcpy(tabs[0].name, "Console");
	tabs[0].screen = console;
	tabs[0].isUpdated = NOUPDATE;
	
	tabCount = 1;
	activeTab = 0;
}

void destroyTabs()
{
	if(tabCount > 1)
	{
		for(int i=1;i<tabCount;i++)
			destroyBuffer(tabs[i].screen);
	}
	
	if(tabs != NULL)
	{
		trackFree(tabs);
	}
	
	tabs = NULL;
	tabCount = 0;
}

void addTab(char *name)
{
	tabCount++;
	tabs = (TAB_ARRAY *)trackRealloc(tabs, sizeof(TAB_ARRAY) * tabCount);
	
	strcpy(tabs[tabCount-1].name, name);
	tabs[tabCount-1].isUpdated = NOUPDATE;
	tabs[tabCount-1].screen = createBuffer(5, 21, 250, 186);
}

int tabFromName(char *name)
{
	char tStr[64];
	char tStr2[64];
	
	strcpy(tStr, name);
	strlwr(tStr);
	
	for(int i=0;i<tabCount;i++)
	{
		strcpy(tStr2, tabs[i].name);
		strlwr(tStr2);
		
		if(strcmp(tStr, tStr2) == 0)
			return i;
	}
	
	return -1;
}	

void concatTab(int tab, char *toAdd, char *colorAdd, int importance)
{
	if(tab > tabCount - 1)
		return;
	if(tab < 0)
		return;
		
	tabs[tab].isUpdated = importance;
	concatBuffer(tabs[tab].screen, toAdd, colorAdd);
}

void removeTab(int tab)
{
	if(tab == -1)
		return;
	
	if(tab == tabCount - 1)
	{
		destroyBuffer(tabs[tab].screen);
		tabCount--;
		tabs = (TAB_ARRAY *)trackRealloc(tabs, sizeof(TAB_ARRAY) * tabCount);
	}
	else
	{
		destroyBuffer(tabs[tab].screen);
		
		for(int i=tab;i<tabCount-1;i++)
			memcpy(&tabs[i], &tabs[i+1], sizeof(TAB_ARRAY));
		
		tabCount--;
		tabs = (TAB_ARRAY *)trackRealloc(tabs, sizeof(TAB_ARRAY) * tabCount);
	}
}

void drawTabs()
{
	setGlobalError(ERROR_IRC_DRAWTABS);
	int *widths = (int *)trackMalloc(sizeof(int) * tabCount, "tabs calc"); // checked
	setGlobalError(ERROR_NONE);
	
	setFont(font_gautami_10);
	for(int i=0;i<tabCount;i++)
		widths[i] = getStringWidth(tabs[i].name, font_gautami_10) + 10;	
	
	bg_setDefaultClipping();
	
	int startTab = 0;
	int tmpWidths = 0;
	
	for(int i=0;i<tabCount;i++)
		tmpWidths += widths[i];
	
	if(tmpWidths > 207)
	{	
		if(activeTab > 0)
		{
			tmpWidths = 0;
			int tmpTab = 0;
			
			for(int i=0;i<activeTab;i++)
				tmpWidths += widths[i];
			
			bool keepGoing = true;
			
			while(tmpWidths > (128 - (widths[activeTab] / 2)) && keepGoing)
			{
				tmpWidths -= widths[tmpTab];
				tmpTab++;
				
				if(tmpTab == activeTab)
					tmpWidths = 0;
				
				int curLocation = 24;
				
				keepGoing = false;
				for(int i=tmpTab;i<tabCount;i++)
				{
					if(curLocation + widths[i] > 231)
						keepGoing = true;
					
					curLocation += widths[i];
				}
			}
			
			startTab = tmpTab;
		}
	}
	else
		startTab = 0;
	
	int curLocation = 24;
	int endTab = 0;
	bool toColor = false;
	
	for(int i=startTab;i<tabCount;i++)
	{
		endTab = i;
		
		if(curLocation + widths[i] > 231)
		{
			if(229-curLocation > 0)
			{
				bg_drawRect(curLocation + 1, 0, 230, TOPAREA, ircTabMoreTabsColor); // draw more tabs indication
				bg_drawBox(curLocation + 1, TOPAREA, 230, TOPAREA, ircTabBorderColor);				
			}
			toColor = true;
			break;
		}
		
		if(i == activeTab)
			bg_drawRect(curLocation + 1, 0, curLocation + widths[i], TOPAREA, ircTabHighlightColor); // hilight current
		else
			bg_drawRect(curLocation + 1, 0, curLocation + widths[i], TOPAREA, ircTabFillColor); // regular background
		
		switch(tabs[i].isUpdated)
		{
			case NOUPDATE:
				setColor(ircTabTextColor);
				break;
			case UNIMPORTANT:
				setColor(ircTabUnimportantTextColor);
				break;
			case IMPORTANT:
				setColor(ircTabImprtantTextColor);
				break;
		}
		
		bg_dispString(curLocation + 5, 3, tabs[i].name);
		
		bg_drawBox(curLocation, TOPAREA, curLocation + widths[i], TOPAREA, ircTabBorderColor);
		bg_drawBox(curLocation + widths[i], 0, curLocation + widths[i], TOPAREA, ircTabBorderColor);
		
		curLocation += widths[i];
	}
	
	trackFree(widths);
	
	int tmpUpdate = NOUPDATE;
	
	if(startTab > 0)
	{
		for(int i=0;i<startTab;i++)
		{
			if(tabs[i].isUpdated > tmpUpdate)
				tmpUpdate = tabs[i].isUpdated;
		}
	}
	
	switch(tmpUpdate)
	{
		case NOUPDATE:
			tmpUpdate = ircTabFillColor;
			break;
		case UNIMPORTANT:
			tmpUpdate = ircTabUnimportantTextColor;
			break;
		case IMPORTANT:
			tmpUpdate = ircTabImprtantTextColor;
			break;
	}
	
	bg_drawRect(0, 0, 24, TOPAREA, tmpUpdate);
	
	tmpUpdate = NOUPDATE;
	
	if(endTab < tabCount && toColor)
	{
		for(int i=endTab;i<tabCount;i++)
		{
			if(tabs[i].isUpdated > tmpUpdate)
				tmpUpdate = tabs[i].isUpdated;
		}
	}
	
	switch(tmpUpdate)
	{
		case NOUPDATE:
			tmpUpdate = ircTabFillColor;
			break;
		case UNIMPORTANT:
			tmpUpdate = ircTabUnimportantTextColor;
			break;
		case IMPORTANT:
			tmpUpdate = ircTabImprtantTextColor;
			break;
	}	
	
	bg_drawRect(231, 0, 255, TOPAREA, tmpUpdate);
	
	bg_dispChar(5, 3, BUTTON_L);
	bg_dispChar(239, 3, BUTTON_R);		
	bg_drawBox(0, TOPAREA, 24, TOPAREA, ircTabBorderColor);
	bg_drawBox(24, 0, 24, TOPAREA, ircTabBorderColor);
	bg_drawBox(231, TOPAREA, 255, TOPAREA, ircTabBorderColor);
	bg_drawBox(231, 0, 231, TOPAREA, ircTabBorderColor);
}

void drawColors()
{
	bg_drawRect(COLORS_LEFT + 1, COLORS_TOP + 23, COLORS_LEFT + 18, COLORS_TOP + 40, 0xFFFF);
	bg_drawRect(19 + COLORS_LEFT + 1, COLORS_TOP + 23, 19 + COLORS_LEFT + 18, COLORS_TOP + 40, 0x0);
	
	// fill colors
	for(int k=0;k<16;k++)
	{
		int i = k % 8;
		int j = k / 8;
		
		uint16 tColor = ircColors[k];
		
		if(colorCursor == k)
			tColor = keyboardHighlightColor;
		
		bg_drawFilledRect(i*19 + COLORS_LEFT, j*19 + COLORS_TOP + 22, i*19 + COLORS_LEFT + 19, j*19 + COLORS_TOP + 41, keyboardBorderColor, tColor);
	}
	
	for(int k=16;k<20;k++)
	{
		int i = k - 16;
		uint16 tColor = keyboardFillColor;
		
		if(colorCursor == k)
			tColor = keyboardHighlightColor;
		
		bg_drawFilledRect(i*19 + COLORS_LEFT, COLORS_TOP, i*19 + COLORS_LEFT + 19, COLORS_TOP + 19, keyboardBorderColor, tColor);			
	}	
	
	if(colorCursor != -1)
	{
		colorCursorCount--;
		
		if(colorCursorCount == 0)
			colorCursor = -1;
	}
	
	bg_dispSprite(COLORS_LEFT + 5, COLORS_TOP + 5, bold, 31775);
	bg_dispSprite(COLORS_LEFT + 24, COLORS_TOP + 5, underline, 31775);
	bg_dispSprite(COLORS_LEFT + 43, COLORS_TOP + 5, color, 31775);
	bg_dispSprite(COLORS_LEFT + 60, COLORS_TOP + 7, backspace, 31775);
}

void handleColorsClick(int px, int py)
{
	if(px < COLORS_LEFT || py < COLORS_TOP)
		return;
	
	for(int k=0;k<16;k++)
	{
		int i = k % 8;
		int j = k / 8;
		
		if(px >= (i*19 + COLORS_LEFT + 1) && py >= (j*19 + COLORS_TOP + 23) && px <= (i*19 + COLORS_LEFT + 18) && py <= (j*19 + COLORS_TOP + 40))
		{
			colorCursor = k;
			colorCursorCount = 2;
			
			editIRCAction(28);
			
			if(colorCursor >= 10)
			{
				editIRCAction('1');
				editIRCAction((colorCursor - 10) + '0');
			}
			else
			{
				editIRCAction(colorCursor + '0');
			}			
			
			return;
		}
	}
	
	for(int k=0;k<4;k++)
	{
		int i = k % 8;
		int j = k / 8;
		
		if(px >= (i*19 + COLORS_LEFT + 1) && py >= (j*19 + COLORS_TOP + 1) && px <= (i*19 + COLORS_LEFT + 18) && py <= (j*19 + COLORS_TOP + 18))
		{
			colorCursor = k + 16;
			colorCursorCount = 2;
			
			switch(colorCursor)
			{
				case 16:
					editIRCAction(29);
					break;
				case 17:
					editIRCAction(31);
					break;
				case 18:
					editIRCAction(28);
					break;
				case 19:
					editIRCAction(BSP);
					break;
			}
			
			return;
		}
	}
}

void convertColorCodes(char *strToHandle)
{
	for(int x=0;x<(int)strlen(strToHandle);x++)
	{
		switch(strToHandle[x])
		{
			case 28: // irc_color
				strToHandle[x] = CONTROL_COLOR;
				break;
			case 29: // irc_bold
				strToHandle[x] = CONTROL_BOLD;
				break;
			case 31: // irc_underline
				strToHandle[x] = CONTROL_UNDERLINE;
				break;
		}
	}
}

void IRCListCallback(int pos, int x, int y)
{
	char str[512];
	char tMode = getCurrentMode(tabs[activeTab].screen, tabs[activeTab].screen->nicks[pos].nick);
	
	if(tMode != 0)
	{
		sprintf(str, "%c%s", getCurrentMode(tabs[activeTab].screen, tabs[activeTab].screen->nicks[pos].nick), tabs[activeTab].screen->nicks[pos].nick);
	}
	else
	{
		strcpy(str, tabs[activeTab].screen->nicks[pos].nick);
	}
	
	abbreviateString(str, LIST_WIDTH - (15 + 18), font_arial_9);		
	
	bg_dispString(15, 0, str);
	bg_drawRect(x + 3, y + 3, x + 8, y + 8, listTextColor);
}

bool isChan(char *ch)
{
	switch(ch[0])
	{
		case '&':
		case '#':
		case '+':
		case '!':
			return true;
	}
	
	return false;
}

bool commandIsServer() // check for commands that can be sent when not connected
{	
	char tStr[1024];
	strcpy(tStr,inputBuffer);
	strlwr(tStr);

	if(strncmp(tStr, "/s ", 3) == 0 || strncmp(tStr, "/server ", 8) == 0) // server command
		return true;
	if(strcmp(tStr, "/h") == 0 || strcmp(tStr, "/help") == 0) // help command, only in console
		return true;
	if(strcmp(tStr, "/morehelp") == 0) // help command, only in console
		return true;
	if(strcmp(tStr, "/cls") == 0) // clear current screen
		return true;
	
	return false;
}

void checkNickLen(char *oldNick, char *newNick)
{
	if(maxNickLen > 0)
	{
		if((int)strlen(oldNick) > maxNickLen) // trim nick if needed
			strncpy(newNick, oldNick, maxNickLen);
		else
			strcpy(newNick, oldNick);
	}
	else
		strcpy(newNick, oldNick);
}

void insertNewCommand(char *command, int replaceLength)
{
	char *tStr = (char *)safeMalloc(strlen(inputBuffer)+1);
	strcpy(tStr, inputBuffer);
	
	strcpy(inputBuffer, command);
	strcat(inputBuffer, tStr + replaceLength);
	
	free(tStr);
}

void substituteAliases()
{
	if(areEqual(inputBuffer, "/nickserv", 9))
	{
		// Only accept the command if it is whole or ended
		switch(inputBuffer[9])
		{
			case ' ':
			case 0:
				break;
			default: 
				return;
		}
		
		insertNewCommand("/msg nickserv", 9);
		
		return;
	}
	
	if(areEqual(inputBuffer, "/ns", 3))
	{
		// Only accept the command if it is whole or ended
		switch(inputBuffer[3])
		{
			case ' ':
			case 0:
				break;
			default: 
				return;
		}
		
		insertNewCommand("/msg nickserv", 3);
		
		return;
	}
	
	if(areEqual(inputBuffer, "/chanserv", 9))
	{
		// Only accept the command if it is whole or ended
		switch(inputBuffer[9])
		{
			case ' ':
			case 0:
				break;
			default: 
				return;
		}
		
		insertNewCommand("/msg chanserv", 9);
		
		return;
	}
	
	if(areEqual(inputBuffer, "/cs", 3))
	{
		// Only accept the command if it is whole or ended
		switch(inputBuffer[3])
		{
			case ' ':
			case 0:
				break;
			default: 
				return;
		}
		
		insertNewCommand("/msg chanserv", 3);
		
		return;
	}
	
	if(areEqual(inputBuffer, "/memoserv", 9))
	{
		// Only accept the command if it is whole or ended
		switch(inputBuffer[9])
		{
			case ' ':
			case 0:
				break;
			default: 
				return;
		}
		
		insertNewCommand("/msg memoserv", 9);
		
		return;
	}
	
	if(areEqual(inputBuffer, "/ms", 3))
	{
		// Only accept the command if it is whole or ended
		switch(inputBuffer[3])
		{
			case ' ':
			case 0:
				break;
			default: 
				return;
		}
		
		insertNewCommand("/msg memoserv", 3);
		
		return;
	}
}

bool checkCommand()
{	
	if(ignoreCommands)
	{
		ignoreCommands = false;
		return false;
	}
	
	substituteAliases();
	
	static char tStr[1024];
	static char cStr[1024];
	
	memset(tStr, 0, 1024);
	strcpy(tStr,inputBuffer);
	strlwr(tStr);
	
	if(strcmp(tStr, "/h") == 0 || strcmp(tStr, "/help") == 0) // help command
	{
		sprintf(tStr, "\n\nDSOrganize IRC Help\n\n/s /server - Connect to new server\n/j /join - Join channel\n/p /part - Part channel\n/hop - Rejoin channel\n/q /quit - Close connection\n/pm /query - Open private message window\n/c /close - Close private message window\n/me /action - Action message\n/n /notice - Send notice\n/m /msg - Send message\n/d /describe - Send action\n/nn /nick - New nick\n/w /whois - Whois user\n/nl /nicklist - List nicks in channel\n/a /away - Sets your away status\n/cls - Clear window\n/morehelp - View advanced commands.\n\nEnd of help.");
		concatCommand(cStr, makeCommand(true, false, ircColorDSO), strlen(tStr));		
		concatTab(activeTab, tStr, cStr, UNIMPORTANT);
		
		return true;
	}
	
	if(strcmp(tStr, "/morehelp") == 0) // extendedhelp command
	{
		sprintf(tStr, "\n\nDSOrganize IRC Advanced Help\n\n/topic - Display or change topic.\n/mode - Set channel or nick modes.\n/invite - Invite user to channel.\n/kick - Kick user from channel.\n/motd - View extended MOTD.\n/whowas - Find out past information about a nick.\n/ctcp - Send raw CTCP message.\n/version - Send CTCP version message.\n/time - Send CTCP time message.\n/ping - Send CTCP ping message.\n/raw /quote - Send raw message.\n\nEnd of help.");
		concatCommand(cStr, makeCommand(true, false, ircColorDSO), strlen(tStr));		
		concatTab(activeTab, tStr, cStr, UNIMPORTANT);
		
		return true;
	}
	
	if(strcmp(tStr, "/p") == 0 || strcmp(tStr, "/part") == 0 || strncmp(tStr, "/p ", 3) == 0 || strncmp(tStr, "/part ", 6) == 0 ) // part command		
	{		
		if(isChan(tabs[activeTab].name))
		{
			if(strchr(inputBuffer, ' ') != NULL)
				sprintf(tStr, "PART %s :%s\r\n", tabs[activeTab].name, strchr(inputBuffer, ' ') + 1);			
			else
				sprintf(tStr, "PART %s\r\n", tabs[activeTab].name);
			
			removeTab(activeTab);
			activeTab--;
			
			sendData(ircSocket, tStr, strlen(tStr));
		}
		else
		{
			sprintf(tStr, "\n%s", irc_cannotpart);
			makeNormal(cStr, strlen(tStr));
			concatTab(activeTab, tStr, cStr, UNIMPORTANT);
		}
		
		return true;
	}
	
	if(strcmp(tStr, "/hop") == 0 || strncmp(tStr, "/hop ", 5) == 0 ) // hop command		
	{		
		if(isChan(tabs[activeTab].name))
		{
			char tName[64];
			strcpy(tName, tabs[activeTab].name);
			
			removeTab(activeTab);
			activeTab--;
			
			if(strchr(inputBuffer, ' ') != NULL)
			{
				sprintf(tStr, "PART %s :%s\r\n", tName, strchr(inputBuffer, ' ') + 1);
			}
			else
			{
				sprintf(tStr, "PART %s\r\n", tName);
			}
			
			sendData(ircSocket, tStr, strlen(tStr));
			
			sprintf(tStr, "JOIN %s\r\nMODE %s\r\n", tName, tName);
			sendData(ircSocket, tStr, strlen(tStr));
		}
		else
		{
			sprintf(tStr, "\n%s", irc_cannotpart);
			makeNormal(cStr, strlen(tStr));
			concatTab(activeTab, tStr, cStr, UNIMPORTANT);
		}
		
		return true;
	}
	
	if(strcmp(tStr, "/q") == 0 || strcmp(tStr, "/quit") == 0 || strncmp(tStr, "/q ", 3) == 0 || strncmp(tStr, "/quit ", 6) == 0 ) // quit command		
	{		
		if(strchr(inputBuffer, ' ') != NULL)
		{
			memset(quitMessage, 0, 256);
			strncpy(quitMessage, strchr(inputBuffer, ' ') + 1, 255);
		}
		
		ircMode = IRC_CLOSEDCONNECTION;
		createIRCButtons();
		
		return true;
	}
	
	if(strcmp(tStr, "/s") == 0 || strncmp(tStr, "/s ", 3) == 0 || strcmp(tStr, "/server") == 0 || strncmp(tStr, "/server ", 8) == 0 || strcmp(tStr, "/connect") == 0 || strncmp(tStr, "/connect ", 9) == 0) // server command
	{
		if(strchr(inputBuffer, ' ') == NULL)
		{
			sprintf(tStr, "\n%s", irc_noserver);
			makeNormal(cStr, strlen(tStr));
			concatTab(activeTab, tStr, cStr, UNIMPORTANT);
			
			return true;
		}
		
		char *server = strchr(inputBuffer, ' ') + 1;
		
		if(connected)
		{
			ircMode = IRC_CLOSEDCONNECTION;
			createIRCButtons();
			
			reconnect = true;
			strcpy(curServer, server);
		}
		else
		{
			char tmpServer[128];
			strcpy(tmpServer,server);
			
			connectToServer(tmpServer);	
			
			ircMode = IRC_READY;
			createIRCButtons();
		}
		
		return true;
	}
	
	if(strcmp(tStr, "/j") == 0 || strncmp(tStr, "/j ", 3) == 0 || strcmp(tStr, "/join") == 0 || strncmp(tStr, "/join ", 6) == 0) // join command
	{
		if(strchr(inputBuffer, ' ') == NULL)
		{
			sprintf(tStr, "\n%s", irc_nochannel);
			makeNormal(cStr, strlen(tStr));
			concatTab(activeTab, tStr, cStr, UNIMPORTANT);
			
			return true;
		}
		
		char *chan = strchr(inputBuffer, ' ') + 1;
		
		if(isChan(chan))
		{		
			sprintf(tStr, "JOIN %s\r\nMODE %s\r\n", chan, chan);
			sendData(ircSocket, tStr, strlen(tStr));
		}
		else
		{
			sprintf(tStr, "\n%s", irc_invalidchannel);
			makeNormal(cStr, strlen(tStr));
			concatTab(activeTab, tStr, cStr, UNIMPORTANT);
		}
		
		return true;
	}
	
	if(strcmp(tStr, "/a") == 0 || strcmp(tStr, "/away") == 0 || strncmp(tStr, "/a ", 3) == 0 || strncmp(tStr, "/away ", 6) == 0) // away command
	{
		if(strchr(inputBuffer, ' ') == NULL) // mark away as off
		{
			strcpy(tStr, "AWAY\r\n");
			sendData(ircSocket, tStr, strlen(tStr));
			
			return true;
		}
		
		char *msg = strchr(inputBuffer, ' ') + 1;
		
		sprintf(tStr, "AWAY %s\r\n", msg);
		sendData(ircSocket, tStr, strlen(tStr));
		
		return true;
	}
	
	if(strcmp(tStr, "/me") == 0 || strncmp(tStr, "/me ", 4) == 0 || strcmp(tStr, "/action") == 0 || strncmp(tStr, "/action ", 8) == 0) // action ctcp command
	{
		if(strchr(inputBuffer, ' ') == NULL)
		{
			sprintf(tStr, "\n%s", irc_noaction);
			makeNormal(cStr, strlen(tStr));
			concatTab(activeTab, tStr, cStr, UNIMPORTANT);
			
			return true;
		}
		
		char *action = strchr(inputBuffer, ' ') + 1;
		
		if(activeTab == 0)
		{
			sprintf(tStr, "\n%s", irc_notinchannel);
			makeNormal(cStr, strlen(tStr));
			concatTab(0, tStr, cStr, NOUPDATE);		
		}
		else
		{		
			sprintf(tStr, ":%s PRIVMSG %s :%cACTION %s%c\r\n", displayNick, tabs[activeTab].name, 1, action, 1);
			sendData(ircSocket, tStr, strlen(tStr));
			
			sprintf(cStr, "\n%s", irc_actionsent);
			osprintf(tStr, cStr, displayNick, action, getCurrentMode(tabs[activeTab].screen, displayNick));
			memset(cStr, 0, 1024);
			formatText(tStr, cStr, false, false, ircColorACTION);
			concatTab(activeTab, tStr, cStr, NOUPDATE);
		}
		
		return true;
	}	
	
	if(strcmp(tStr, "/pm") == 0 || strncmp(tStr, "/pm ", 4) == 0 || strcmp(tStr, "/query") == 0 || strncmp(tStr, "/query ", 7) == 0) // query command
	{
		if(strchr(inputBuffer, ' ') == NULL)
		{
			sprintf(tStr, "\n%s", irc_invalidnick);
			makeNormal(cStr, strlen(tStr));
			concatTab(activeTab, tStr, cStr, UNIMPORTANT);
			
			return true;
		}
		
		char *nick = strchr(inputBuffer, ' ') + 1;
		
		if(!isChan(nick))
		{	
			// check if the window is open
			
			if(tabFromName(nick) == -1)
				addTab(nick); // doesn't exist
			
			activeTab = tabFromName(nick); // set focus
			createIRCButtons();
		}
		else
		{
			sprintf(tStr, "\n%s", irc_invalidnick);
			makeNormal(cStr, strlen(tStr));
			concatTab(activeTab, tStr, cStr, UNIMPORTANT);			
		}
		
		return true;
	}	
	
	if(strcmp(tStr, "/n") == 0 || strncmp(tStr, "/n ", 3) == 0 || strcmp(tStr, "/notice") == 0 || strncmp(tStr, "/notice ", 8) == 0) // notice command
	{
		if(strchr(inputBuffer, ' ') == NULL)
		{
			sprintf(tStr, "\n%s", irc_invalidnick);
			makeNormal(cStr, strlen(tStr));
			concatTab(activeTab, tStr, cStr, UNIMPORTANT);	
			
			return true;
		}
		
		char *nick = strchr(inputBuffer, ' ') + 1;
		
		if(strchr(nick, ' ') == NULL)
		{			
			sprintf(tStr, "\n%s", irc_nomessage);
			makeNormal(cStr, strlen(tStr));
			concatTab(activeTab, tStr, cStr, UNIMPORTANT);	
			
			return true;
		}
		
		char *message = strchr(nick, ' ') + 1;
		(strchr(nick, ' '))[0] = 0;
		
		sprintf(tStr, ":%s NOTICE %s :%s\r\n", displayNick, nick, message);
		sendData(ircSocket, tStr, strlen(tStr));
		sprintf(cStr, "\n%s", irc_noticesent);
		osprintf(tStr, cStr, nick, message);
		memset(cStr, 0, 1024);
		concatCommand(cStr, makeCommand(false, false, ircColorNOTICE), strlen(tStr));
		concatTab(activeTab, tStr, cStr, NOUPDATE);		
		
		return true;
	}
	
	if(strcmp(tStr, "/m") == 0 || strncmp(tStr, "/m ", 3) == 0 || strcmp(tStr, "/msg") == 0 || strncmp(tStr, "/msg ", 5) == 0) // msg command
	{
		if(strchr(inputBuffer, ' ') == NULL)
		{
			sprintf(tStr, "\n%s", irc_invalidnick);
			makeNormal(cStr, strlen(tStr));
			concatTab(activeTab, tStr, cStr, UNIMPORTANT);	
			
			return true;
		}
		
		char *nick = strchr(inputBuffer, ' ') + 1;
		
		if(strchr(nick, ' ') == NULL)
		{			
			sprintf(tStr, "\n%s", irc_nomessage);
			makeNormal(cStr, strlen(tStr));
			concatTab(activeTab, tStr, cStr, UNIMPORTANT);	
			
			return true;
		}
		
		char *message = strchr(nick, ' ') + 1;
		(strchr(nick, ' '))[0] = 0;
		
		sprintf(tStr, ":%s PRIVMSG %s :%s\r\n", displayNick, nick, message);
		sendData(ircSocket, tStr, strlen(tStr));
		sprintf(cStr, "\n%s", irc_messagesent);
		osprintf(tStr, cStr, nick, message);
		memset(cStr, 0, 1024);
		concatCommand(cStr, makeCommand(false, false, ircColorNOTICE), strlen(tStr));
		concatTab(activeTab, tStr, cStr, NOUPDATE);		
		
		return true;
	}
	
	if(strcmp(tStr, "/d") == 0 || strncmp(tStr, "/d ", 3) == 0 || strcmp(tStr, "/describe") == 0 || strncmp(tStr, "/describe ", 10) == 0) // describe command
	{
		if(strchr(inputBuffer, ' ') == NULL)
		{
			sprintf(tStr, "\n%s", irc_invalidnick);
			makeNormal(cStr, strlen(tStr));
			concatTab(activeTab, tStr, cStr, UNIMPORTANT);	
			
			return true;
		}
		
		char *nick = strchr(inputBuffer, ' ') + 1;
		
		if(strchr(nick, ' ') == NULL)
		{			
			sprintf(tStr, "\n%s", irc_nomessage);
			makeNormal(cStr, strlen(tStr));
			concatTab(activeTab, tStr, cStr, UNIMPORTANT);	
			
			return true;
		}
		
		char *message = strchr(nick, ' ') + 1;
		(strchr(nick, ' '))[0] = 0;
		
		sprintf(tStr, ":%s PRIVMSG %s :%cACTION %s%c\r\n", displayNick, nick, 1, message, 1);
		sendData(ircSocket, tStr, strlen(tStr));
		
		sprintf(cStr, "\n%s", irc_messagesent);
		osprintf(tStr, cStr, nick, message);
		memset(cStr, 0, 1024);
		concatCommand(cStr, makeCommand(false, false, ircColorNOTICE), strlen(tStr));
		concatTab(activeTab, tStr, cStr, NOUPDATE);		
		
		return true;
	}
	
	if(strcmp(tStr, "/c") == 0 || strcmp(tStr, "/close") == 0) // close pm window command		
	{		
		if(activeTab != 0 && !isChan(tabs[activeTab].name))
		{
			removeTab(activeTab);
			activeTab--;
		}
		else
		{
			sprintf(tStr, "\n%s", irc_cannotclose);
			makeNormal(cStr, strlen(tStr));
			concatTab(activeTab, tStr, cStr, UNIMPORTANT);
		}
		
		return true;
	}
	
	if(strcmp(tStr, "/nn") == 0 || strncmp(tStr, "/nn ", 4) == 0 || strcmp(tStr, "/nick") == 0 || strncmp(tStr, "/nick ", 6) == 0) // nick command
	{
		if(strchr(inputBuffer, ' ') == NULL)
		{
			sprintf(tStr, "\n%s", irc_invalidnick);
			makeNormal(cStr, strlen(tStr));
			concatTab(activeTab, tStr, cStr, UNIMPORTANT);	
			
			return true;
		}
		
		connectMode = C_NICKCHANGE;
		
		char *nn = strchr(inputBuffer, ' ') + 1;
		
		if(strlen(nn) == 0)
		{
			sprintf(tStr, "\n%s", irc_invalidnick);
			makeNormal(cStr, strlen(tStr));
			concatTab(activeTab, tStr, cStr, UNIMPORTANT);	
			
			return true;
		}
		
		char tNick[NICK_LEN];
		memset(tNick, 0, NICK_LEN);
		checkNickLen(nn, tNick);
		
		sprintf(tStr, "NICK %s\r\n", tNick);
		sendData(ircSocket, tStr, strlen(tStr));
		
		return true;
	}
	
	if(strcmp(tStr, "/nl") == 0 || strcmp(tStr, "/nicklist") == 0) // list nicks, only in channel
	{
		if(!isChan(tabs[activeTab].name))
			return true;
		
		sortNicks(activeTab);
		dispNicks(activeTab);
		
		return true;
	}
	
	if(strcmp(tStr, "/w") == 0 || strncmp(tStr, "/w ", 3) == 0 || strcmp(tStr, "/whois") == 0 || strncmp(tStr, "/whois ", 7) == 0) // whois command
	{
		if(strchr(inputBuffer, ' ') == NULL)
		{
			sprintf(tStr, "\n%s", irc_invalidnick);
			makeNormal(cStr, strlen(tStr));
			concatTab(activeTab, tStr, cStr, UNIMPORTANT);	
			
			return true;
		}
		
		char *whoisNick = strchr(inputBuffer, ' ') + 1;
		
		sprintf(tStr, "WHOIS %s\r\n", whoisNick);
		sendData(ircSocket, tStr, strlen(tStr));
		
		return true;
	}
	
	if(strcmp(tStr, "/cls") == 0) // clear current screen
	{
		clearBuffer(tabs[activeTab].screen);
		
		return true;
	}
	
	if(strcmp(tStr, "/topic") == 0 || strncmp(tStr, "/topic ", 7) == 0) // topic set/request
	{
		if(strchr(inputBuffer, ' ') == NULL) // no channel specified, lets assume they meant this one
		{
			if(isChan(tabs[activeTab].name))
			{
				sprintf(tStr, "TOPIC %s\r\n", tabs[activeTab].name);
				sendData(ircSocket, tStr, strlen(tStr));
			}
			else
			{
				sprintf(tStr, "\n%s", irc_nochannel);
				makeNormal(cStr, strlen(tStr));
				concatTab(activeTab, tStr, cStr, UNIMPORTANT);				
			}
			
			return true;
		}
		
		char *whichChannel = strchr(inputBuffer, ' ') + 1;
		
		if(strchr(whichChannel, ' ') == NULL) // just a topic request
		{
			sprintf(tStr, "TOPIC %s\r\n", whichChannel);
			sendData(ircSocket, tStr, strlen(tStr));
			
			return true;
		}
		
		char *topic = strchr(whichChannel, ' ') + 1;
		
		*strchr(whichChannel, ' ') = 0;
		
		sprintf(tStr, "TOPIC %s :%s\r\n", whichChannel, topic);
		sendData(ircSocket, tStr, strlen(tStr));
		
		return true;		
	}
	
	if(strcmp(tStr, "/mode") == 0 || strncmp(tStr, "/mode ", 6) == 0) // mode command
	{		
		if(strchr(inputBuffer, ' ') == NULL) // place empty message if its not there, dont check due to complexity of command
		{
			int x = strlen(inputBuffer);
			inputBuffer[x] = ' ';
			inputBuffer[x+1] = 0;
		}
		
		char *modeMsg = strchr(inputBuffer, ' ') + 1;
		
		sprintf(tStr, "MODE %s\r\n", modeMsg);
		sendData(ircSocket, tStr, strlen(tStr));
		
		return true;
	}
	
	if(strcmp(tStr, "/invite") == 0 || strncmp(tStr, "/invite ", 8) == 0) // invite command
	{
		if(strchr(inputBuffer, ' ') == NULL)
		{
			sprintf(tStr, "\n%s", irc_invalidnick);
			makeNormal(cStr, strlen(tStr));
			concatTab(activeTab, tStr, cStr, UNIMPORTANT);	
			
			return true;
		}
		
		char *nick = strchr(inputBuffer, ' ') + 1;
		
		if(strchr(nick, ' ') == NULL)
		{			
			sprintf(tStr, "\n%s", irc_nochannel);
			makeNormal(cStr, strlen(tStr));
			concatTab(activeTab, tStr, cStr, UNIMPORTANT);	
			
			return true;
		}
		
		char *channel = strchr(nick, ' ') + 1;
		(strchr(nick, ' '))[0] = 0;
		
		sprintf(tStr, "INVITE %s %s\r\n", nick, channel);
		sendData(ircSocket, tStr, strlen(tStr));		
		
		return true;
	}	
	
	if(strcmp(tStr, "/kick") == 0 || strncmp(tStr, "/kick ", 6) == 0) // kick command
	{
		if(strchr(inputBuffer, ' ') == NULL)
		{
			sprintf(tStr, "\n%s", irc_invalidnick);
			makeNormal(cStr, strlen(tStr));
			concatTab(activeTab, tStr, cStr, UNIMPORTANT);	
			
			return true;
		}
		
		char *nick = strchr(inputBuffer, ' ') + 1;
		
		if(strchr(nick, ' ') == NULL)
		{
			// user typed /kick nick, lets hope he's in a channel
			
			if(isChan(tabs[activeTab].name))
			{
				sprintf(tStr, "KICK %s %s\r\n", tabs[activeTab].name, nick);
				sendData(ircSocket, tStr, strlen(tStr));
			}
			else
			{			
				sprintf(tStr, "\n%s", irc_nochannel);
				makeNormal(cStr, strlen(tStr));
				concatTab(activeTab, tStr, cStr, UNIMPORTANT);				
			}
			
			return true;
		}
		
		char *channel = nick;
		nick = strchr(nick, ' ') + 1;
		*strchr(channel, ' ') = 0;
		
		// channel contains the potential channel, potential nick
		// nick contains the rest of everything
		
		if(!isChan(channel)) // its the nick
		{
			if(isChan(tabs[activeTab].name))
			{
				sprintf(tStr, "KICK %s %s :%s\r\n", tabs[activeTab].name, channel, nick);
				sendData(ircSocket, tStr, strlen(tStr));
			}
			else
			{			
				sprintf(tStr, "\n%s", irc_nochannel);
				makeNormal(cStr, strlen(tStr));
				concatTab(activeTab, tStr, cStr, UNIMPORTANT);				
			}
			
			return true;
		}
		
		// channel is the channel
		// nick contains nick and everything else
		
		if(strchr(nick, ' ') == NULL) // no message
		{
			sprintf(tStr, "KICK %s %s\r\n", channel, nick);
			sendData(ircSocket, tStr, strlen(tStr));
			
			return true;
		}
		
		char *kickMsg = strchr(nick, ' ') + 1;
		*strchr(nick, ' ') = 0;
		
		sprintf(tStr, "KICK %s %s :%s\r\n", channel, nick, kickMsg);
		sendData(ircSocket, tStr, strlen(tStr));
		
		return true;
	}
	
	if(strcmp(tStr, "/motd") == 0 || strncmp(tStr, "/motd ", 6) == 0) // motd command
	{		
		if(strchr(inputBuffer, ' ') == NULL)
		{
			sprintf(tStr, "MOTD\r\n");
			sendData(ircSocket, tStr, strlen(tStr));			
			
			return true;
		}
		
		char *motdServer = strchr(inputBuffer, ' ') + 1;
		
		sprintf(tStr, "MOTD %s\r\n", motdServer);
		sendData(ircSocket, tStr, strlen(tStr));			
		
		return true;
	}	
	
	if(strcmp(tStr, "/whowas") == 0 || strncmp(tStr, "/whowas ", 8) == 0) // whowas command
	{
		if(strchr(inputBuffer, ' ') == NULL)
		{
			sprintf(tStr, "\n%s", irc_invalidnick);
			makeNormal(cStr, strlen(tStr));
			concatTab(activeTab, tStr, cStr, UNIMPORTANT);	
			
			return true;
		}
		
		char *whoWas = strchr(inputBuffer, ' ') + 1;
		
		sprintf(tStr, "WHOWAS %s\r\n", whoWas);
		sendData(ircSocket, tStr, strlen(tStr));
		
		return true;	
	}
	
	if(strcmp(tStr, "/version") == 0 || strncmp(tStr, "/version ", 9) == 0) // ctcp version command
	{
		if(strchr(inputBuffer, ' ') == NULL)
		{
			sprintf(tStr, "\n%s", irc_invalidnick);
			makeNormal(cStr, strlen(tStr));
			concatTab(activeTab, tStr, cStr, UNIMPORTANT);	
			
			return true;
		}
		
		char *nick = strchr(inputBuffer, ' ') + 1;
		
		sprintf(tStr, ":%s PRIVMSG %s :%cVERSION%c\r\n", displayNick, nick, 1, 1);
		sendData(ircSocket, tStr, strlen(tStr));
		sprintf(cStr, "\n%s", irc_ctcpsent);
		osprintf(tStr, cStr, nick, "VERSION");
		memset(cStr, 0, 1024);
		concatCommand(cStr, makeCommand(false, false, ircColorCTCP), strlen(tStr));
		concatTab(activeTab, tStr, cStr, NOUPDATE);		
		
		return true;
	}		
	
	if(strcmp(tStr, "/time") == 0 || strncmp(tStr, "/time ", 6) == 0) // ctcp time command
	{
		if(strchr(inputBuffer, ' ') == NULL)
		{
			sprintf(tStr, "\n%s", irc_invalidnick);
			makeNormal(cStr, strlen(tStr));
			concatTab(activeTab, tStr, cStr, UNIMPORTANT);	
			
			return true;
		}
		
		char *nick = strchr(inputBuffer, ' ') + 1;
		
		sprintf(tStr, ":%s PRIVMSG %s :%cTIME%c\r\n", displayNick, nick, 1, 1);
		sendData(ircSocket, tStr, strlen(tStr));
		sprintf(cStr, "\n%s", irc_ctcpsent);
		osprintf(tStr, cStr, nick, "TIME");
		memset(cStr, 0, 1024);
		concatCommand(cStr, makeCommand(false, false, ircColorCTCP), strlen(tStr));
		concatTab(activeTab, tStr, cStr, NOUPDATE);		
		
		return true;
	}		
	
	if(strcmp(tStr, "/ping") == 0 || strncmp(tStr, "/ping ", 6) == 0) // ctcp ping command
	{
		if(strchr(inputBuffer, ' ') == NULL)
		{
			sprintf(tStr, "\n%s", irc_invalidnick);
			makeNormal(cStr, strlen(tStr));
			concatTab(activeTab, tStr, cStr, UNIMPORTANT);	
			
			return true;
		}
		
		char *nick = strchr(inputBuffer, ' ') + 1;
		
		sprintf(tStr, ":%s PRIVMSG %s :%cPING%c\r\n", displayNick, nick, 1, 1);
		sendData(ircSocket, tStr, strlen(tStr));
		sprintf(cStr, "\n%s", irc_ctcpsent);
		osprintf(tStr, cStr, nick, "PING");
		memset(cStr, 0, 1024);
		concatCommand(cStr, makeCommand(false, false, ircColorCTCP), strlen(tStr));
		concatTab(activeTab, tStr, cStr, NOUPDATE);		
		
		return true;
	}	
	
	if(strcmp(tStr, "/ctcp") == 0 || strncmp(tStr, "/ctcp ", 6) == 0) // regular ctcp command
	{
		if(strchr(inputBuffer, ' ') == NULL)
		{
			sprintf(tStr, "\n%s", irc_invalidnick);
			makeNormal(cStr, strlen(tStr));
			concatTab(activeTab, tStr, cStr, UNIMPORTANT);	
			
			return true;
		}
		
		char *nick = strchr(inputBuffer, ' ') + 1;
		
		if(strchr(nick, ' ') == NULL)
		{			
			sprintf(tStr, "\n%s", irc_nomessage);
			makeNormal(cStr, strlen(tStr));
			concatTab(activeTab, tStr, cStr, UNIMPORTANT);	
			
			return true;
		}
		
		char *ctcpCommand = strchr(nick, ' ') + 1;
		*strchr(nick, ' ') = 0;
		
		if(strchr(ctcpCommand, ' ') == NULL) // just a single command
		{
			strupr(ctcpCommand);
			
			sprintf(tStr, ":%s PRIVMSG %s :%c%s%c\r\n", displayNick, nick, 1, ctcpCommand, 1);
			sendData(ircSocket, tStr, strlen(tStr));
			sprintf(cStr, "\n%s", irc_ctcpsent);
			osprintf(tStr, cStr, nick, ctcpCommand);
			memset(cStr, 0, 1024);
			concatCommand(cStr, makeCommand(false, false, ircColorCTCP), strlen(tStr));
			concatTab(activeTab, tStr, cStr, NOUPDATE);		
			
			return true;
		}
		
		char *ctcpMessage = strchr(ctcpCommand, ' ') + 1;
		*strchr(ctcpCommand, ' ') = 0;
		
		strupr(ctcpCommand);
		char tmpCTCP[256];
		sprintf(tmpCTCP, "%s %s", ctcpCommand, ctcpMessage);
		
		sprintf(tStr, ":%s PRIVMSG %s :%c%s%c\r\n", displayNick, nick, 1, tmpCTCP, 1);
		sendData(ircSocket, tStr, strlen(tStr));
		sprintf(cStr, "\n%s", irc_ctcpsent);
		osprintf(tStr, cStr, nick, tmpCTCP);
		memset(cStr, 0, 1024);
		concatCommand(cStr, makeCommand(false, false, ircColorCTCP), strlen(tStr));
		concatTab(activeTab, tStr, cStr, NOUPDATE);		
		
		return true;
	}		
	
	if(strcmp(tStr, "/raw") == 0 || strncmp(tStr, "/raw ", 5) == 0 || strcmp(tStr, "/quote") == 0 || strncmp(tStr, "/quote ", 7) == 0) // raw server command
	{
		if(strchr(inputBuffer, ' ') == NULL)
		{
			sprintf(tStr, "\n%s", irc_noraw);
			makeNormal(cStr, strlen(tStr));
			concatTab(activeTab, tStr, cStr, UNIMPORTANT);	
			
			return true;
		}
		
		char *raw = strchr(inputBuffer, ' ') + 1;
		
		sprintf(tStr, "%s\r\n", raw);
		sendData(ircSocket, tStr, strlen(tStr));
		
		return true;
	}
	
	if(tStr[0] == '/')
	{
		sprintf(tStr, "\n%s", irc_unknown);
		makeNormal(cStr, strlen(tStr));
		concatTab(activeTab, tStr, cStr, UNIMPORTANT);	
		
		return true;
	}
	
	return false;
}

// handle parsing input here
void sendBuffer()
{	
	if(ircMode == IRC_DISCONNECTED || ircMode == IRC_CONNECTING || ircMode == IRC_FAILEDCONNECT) // don't accept server while not connected
		return;
	
	if(inputBuffer[0] == 0)
		return;
	
	addBackBuffer();
	convertColorCodes(inputBuffer);
	
	int x = 0;
	int y = 0;
	bool lastSpace = false;
	
	// strip leading spaces
	while(inputBuffer[x] == ' ')
		x++;
	
	// strip multi spaces
	while(inputBuffer[x] != 0)
	{
		if(inputBuffer[x] == ' ')
		{
			if(!lastSpace)
			{
				inputBuffer[y] = inputBuffer[x];
				y++;
			}
			
			lastSpace = true;
		}
		else
		{
			inputBuffer[y] = inputBuffer[x];
			y++;
			
			lastSpace = false;
		}
		
		x++;
	}
	
	inputBuffer[y] = 0;
	
	if(!connected)
	{
		if(commandIsServer())
			checkCommand();
		
		resetKBCursor();
		memset(inputBuffer, 0, MAX_INPUT+1);
		strUpdated = true;
		
		return;
	}
	
	char tStr[1024];
	char cStr[1024];
	
	if(activeTab == 0) // console
	{	
		if(!checkCommand())
		{
			sprintf(tStr, "\n%s", irc_notinchannel);
			makeNormal(cStr, strlen(tStr));
			concatTab(0, tStr, cStr, NOUPDATE);
		}
	}
	else // regular message
	{	
		if(!checkCommand())
		{
			sprintf(tStr, ":%s PRIVMSG %s :%s\r\n", displayNick, tabs[activeTab].name, inputBuffer);
			sendData(ircSocket, tStr, strlen(tStr));
			sprintf(cStr, "\n%s", irc_channelsent);
			osprintf(tStr, cStr, displayNick, inputBuffer, getCurrentMode(tabs[activeTab].screen, displayNick));
			memset(cStr, 0, 1024);
			formatText(tStr, cStr, false, false, ircColorNORMAL);
			concatTab(activeTab, tStr, cStr, NOUPDATE);
		}
	}
	
	resetKBCursor();
	memset(inputBuffer, 0, MAX_INPUT+1);
	strUpdated = true;
}

void parseScriptFile(char *fName)
{
	if(DRAGON_FileExists(fName) != FE_FILE)
	{
		return;
	}
	
	DRAGON_FILE *fp = DRAGON_fopen(fName, "r");
	
	if(fp == NULL)
	{
		return;
	}
	
	if(DRAGON_flength(fp) == 0)
	{
		return;
	}
	
	while(!DRAGON_feof(fp))
	{
		memset(inputBuffer, 0, MAX_INPUT);
		DRAGON_fgets(inputBuffer, MAX_INPUT, fp);
		sendBuffer();
	}
	
	DRAGON_fclose(fp);
}

void autoPerform()
{
	DRAGON_chdir("/");
	
	// check if the autoperform.txt file is in the dsorganize data root
	// if so, perform each line as a command
	
	char fName[256];
	sprintf(fName, "%sautoperform.txt", d_base);
	parseScriptFile(fName);
	
	if(strlen(curNetwork) > 0)
	{
		// check if the [network].autoperform.txt file is in the dsorganize data root
		// if so, perform each line as a command
		
		char fName[256];
		sprintf(fName, "%s%s.autoperform.txt", d_base, curNetwork);
		parseScriptFile(fName);
	}
}

void stripIdent(char *nick)
{
	if(strchr(nick, '!') != NULL)
		((char *)(strchr(nick, '!')))[0] = 0;
}

void killSpaces(char *msg)
{
	if(strchr(msg, ' ') != NULL)
		(strchr(msg, ' '))[0] = 0;
}

void removeBadCharacters(char *msg)
{
	// this function will remove any character < 32 on the ascii map that's not a control character
	
	for( u32 x = 0; x < strlen(msg); x++ )
	{
		switch(msg[x])
		{
			case CONTROL_CTCP:
			case CONTROL_BOLD:
			case CONTROL_COLOR:
			case CONTROL_UNDERLINE:
			case CONTROL_RESET:
				// ignore these as they are commands				
				break;
			default:
				// replace with a question mark if less than 32
				if(msg[x] < 32)
				{
					msg[x] = '?';
				}
				
				break;
		}
	}
}

void parseMessage(char *msg)
{
	char tStr[1024];
	char cStr[1024];
	char *msg2 = msg;
	
	char fromNick[128];	
	char command[128];
	int tLen = 0;
	
	memset(fromNick, 0, 128);
	memset(command, 0, 128);
	
	removeBadCharacters(msg);
	
	if(msg[0] == ':') // nick in front of command
	{
		msg++;
		
		tLen = strchr(msg, ' ') - msg;
		memcpy(fromNick, msg, tLen);
		
		msg += tLen + 1;
	}
	
	if(strchr(msg, ' ') != 0)
	{
		tLen = strchr(msg, ' ') - msg;
		memcpy(command, msg, tLen);
		
		msg += tLen + 1;
	}
	else
	{
		strcpy(command,msg);
		msg = NULL;
	}
	
	if(strcmp(command, "PING") == 0) // ping request
	{
		if(msg != NULL)
		{
			msg++;
			
			sprintf(tStr, "PONG :%s\r\n", msg);
			sendData(ircSocket, tStr, strlen(tStr));
		}
		else
		{
			sprintf(tStr, "PONG\r\n");
			sendData(ircSocket, tStr, strlen(tStr));
		}
		return;
	}
	
	// msg now contains the rest of the command, fromnick is the sender, command is the irc command
	
	if(strcmp(command, "NOTICE") == 0) // notice
	{
		char toNick[128];
		memset(toNick, 0, 128);
		
		tLen = strchr(msg, ' ') - msg;
		memcpy(toNick, msg, tLen);
		
		msg += tLen + 2;
		
		stripIdent(fromNick);
		
		// check for ctcp stuff here.....
		
		if(msg[0] == CONTROL_CTCP && msg[strlen(msg)-1] == CONTROL_CTCP)
		{
			// it is CTCP
			msg++;
			msg[strlen(msg)-1] = 0;
			
			if(strchr(msg, ' ') == NULL)
			{
				sprintf(cStr, "\n%s", irc_ctcpreply);
				osprintf(tStr, cStr, fromNick, msg, "");
				memset(cStr, 0, 1024);
				concatCommand(cStr, makeCommand(false, false, ircColorCTCP), strlen(tStr));
				concatTab(0, tStr, cStr, UNIMPORTANT);			
			}
			else
			{
				char *ctcpReply = strchr(msg, ' ') + 1;
				*strchr(msg, ' ') = 0;
				
				sprintf(cStr, "\n%s", irc_ctcpreply);
				osprintf(tStr, cStr, fromNick, msg, ctcpReply);
				memset(cStr, 0, 1024);
				concatCommand(cStr, makeCommand(false, false, ircColorCTCP), strlen(tStr));
				concatTab(0, tStr, cStr, UNIMPORTANT);
			}
			
			return;			
		}
		
		if(strcmp(toNick, "AUTH") == 0) // notice to go in console
		{
			sprintf(cStr, "\n%s", irc_receivenotice);
			osprintf(tStr, cStr, curServer, msg);
			memset(cStr, 0, 1024);
			formatText(tStr, cStr, false, false, ircColorNOTICE);
			concatTab(0, tStr, cStr, UNIMPORTANT);
			return;
		}
		else
		{			
			if(strlen(fromNick) > 0)
			{
				sprintf(cStr, "\n%s", irc_receivenotice);				
				osprintf(tStr, cStr, fromNick, msg);
				memset(cStr, 0, 1024);
			}
			else
			{
				sprintf(cStr, "\n%s", irc_receiveauth);				
				osprintf(tStr, cStr, msg);
				memset(cStr, 0, 1024);
			}
			
			formatText(tStr, cStr, false, false, ircColorNOTICE);
			concatTab(activeTab, tStr, cStr, IMPORTANT);
			return;
		}
	}
	
	if(strcmp(command, "PRIVMSG") == 0)
	{
		char toNick[128];
		memset(toNick, 0, 128);
		
		tLen = strchr(msg, ' ') - msg;
		memcpy(toNick, msg, tLen);
		
		msg += tLen + 2;
		
		stripIdent(fromNick);
		strlwr(toNick);
		
		// check for ctcp stuff here.....
		
		if(msg[0] == CONTROL_CTCP && msg[strlen(msg)-1] == CONTROL_CTCP)
		{
			// it is CTCP
			msg++;
			msg[strlen(msg)-1] = 0;
			
			if(strcmp(msg,"VERSION") == 0) // ctcp version
			{
				sprintf(tStr, res_aboutIRC, displayNick, fromNick, CONTROL_CTCP, CONTROL_CTCP, '\r', '\n');
				sendData(ircSocket, tStr, strlen(tStr));
				
				sprintf(cStr, "\n%s", irc_version);
				osprintf(tStr, cStr, fromNick);
				memset(cStr, 0, 1024);
				concatCommand(cStr, makeCommand(false, false, ircColorCTCP), strlen(tStr));
				concatTab(0, tStr, cStr, UNIMPORTANT);
				return;
			}
			
			if(strcmp(msg,"TIME") == 0) // ctcp time
			{
				sprintf(tStr, ":%s NOTICE %s :%cTIME %s %s %02d %02d%c%02d%c%02d %04d%c\r\n", displayNick, fromNick, CONTROL_CTCP, days[getDayOfWeek()+7], months[getMonth()+11], getDay(), getHour(true), ':', getMinute(), ':', getSecond(), getYear(), CONTROL_CTCP);
				sendData(ircSocket, tStr, strlen(tStr));
				
				sprintf(cStr, "\n%s", irc_time);
				osprintf(tStr, cStr, fromNick);
				memset(cStr, 0, 1024);
				concatCommand(cStr, makeCommand(false, false, ircColorCTCP), strlen(tStr));
				concatTab(0, tStr, cStr, UNIMPORTANT);
				return;
			}
			
			if(strncmp(msg, "PING", 4) == 0) // ctcp ping
			{
				if(strchr(msg, ' ') == NULL) // no specific ping
					sprintf(tStr, ":%s NOTICE %s :%cPING%c\r\n", displayNick, fromNick, CONTROL_CTCP, CONTROL_CTCP);
				else
					sprintf(tStr, ":%s NOTICE %s :%cPING %s%c\r\n", displayNick, fromNick, CONTROL_CTCP, strchr(msg, ' ') + 1, CONTROL_CTCP);
				sendData(ircSocket, tStr, strlen(tStr));
				
				sprintf(cStr, "\n%s", irc_ping);
				osprintf(tStr, cStr, fromNick);
				memset(cStr, 0, 1024);
				concatCommand(cStr, makeCommand(false, false, ircColorCTCP), strlen(tStr));
				concatTab(0, tStr, cStr, UNIMPORTANT);
				return;
			}
			
			if(strncmp(msg, "ACTION", 6) == 0) // action command
			{
				strcpy(tStr, displayNick);
				strlwr(tStr);
				
				msg += 7;
				
				if(strcmp(tStr,toNick) == 0) // pm
				{
					if(tabFromName(fromNick) == -1) // haven't got an open window
						addTab(fromNick);
					
					if(tabFromName(fromNick) != activeTab && notifyPresent)
					{
						char tStr[256];
						
						sprintf(tStr, "%snotify.wav", d_base);
						loadWavToMemory();
						loadSound(tStr);
					}
					
					sprintf(cStr, "\n%s", irc_actionreceived);
					osprintf(tStr, cStr, fromNick, msg, 0);
					memset(cStr, 0, 1024);
					formatText(tStr, cStr, false, false, ircColorACTION);
					concatTab(tabFromName(fromNick), tStr, cStr, IMPORTANT);
				}
				else // channel
				{
					if(tabFromName(toNick) != -1) // ignore if channel doesn't exist
					{
						char tColor = ircColorACTION;
						
						if(cistrstr(msg, displayNick))
						{
							tColor = ircColorHIGHLIGHT;
							
							if(notifyPresent)
							{
								char tStr[256];
								
								sprintf(tStr, "%snotify.wav", d_base);
								loadWavToMemory();
								loadSound(tStr);
							}
						}
						sprintf(cStr, "\n%s", irc_actionreceived);
						osprintf(tStr, cStr, fromNick, msg, getCurrentMode(tabs[tabFromName(toNick)].screen, fromNick));
						memset(cStr, 0, 1024);
						formatText(tStr, cStr, false, false, tColor);
						concatTab(tabFromName(toNick), tStr, cStr, IMPORTANT);			
					}
				}
				return;
			}
			
			sprintf(cStr, "\n%s", irc_unknownctcp);
			osprintf(tStr, cStr, fromNick, msg);
			memset(cStr, 0, 1024);
			concatCommand(cStr, makeCommand(false, false, ircColorCTCP), strlen(tStr));
			concatTab(0, tStr, cStr, UNIMPORTANT);
			
			return;
		}
		
		strcpy(tStr, displayNick);
		strlwr(tStr);
		
		if(strcmp(tStr,toNick) == 0) // pm
		{
			if(tabFromName(fromNick) == -1) // haven't got an open window
				addTab(fromNick);
			
			sprintf(cStr, "\n%s", irc_channelreceived);
			osprintf(tStr, cStr, fromNick, msg, 0);
			memset(cStr, 0, 1024);
			formatText(tStr, cStr, false, false, ircColorNORMAL);
			concatTab(tabFromName(fromNick), tStr, cStr, IMPORTANT);
			
			if(tabFromName(fromNick) != activeTab && notifyPresent)
			{
				char tStr[256];
				
				sprintf(tStr, "%snotify.wav", d_base);
				loadWavToMemory();
				loadSound(tStr);
			}
		}
		else // channel
		{
			if(tabFromName(toNick) != -1) // ignore if channel doesn't exist
			{	
				char tColor = ircColorNORMAL;
				
				if(cistrstr(msg, displayNick))
				{
					tColor = ircColorHIGHLIGHT;
					
					if(notifyPresent)
					{
						char tStr[256];
						
						sprintf(tStr, "%snotify.wav", d_base);
						loadWavToMemory();
						loadSound(tStr);
					}
				}
				
				sprintf(cStr, "\n%s", irc_channelreceived);
				osprintf(tStr, cStr, fromNick, msg, getCurrentMode(tabs[tabFromName(toNick)].screen, fromNick));
				memset(cStr, 0, 1024);
				formatText(tStr, cStr, false, false, tColor);
				concatTab(tabFromName(toNick), tStr, cStr, IMPORTANT);			
			}
		}
		
		return;
	}
	
	if(strcmp(command, "JOIN") == 0)
	{
		if(msg[0] == ':')
			msg++;
		
		killSpaces(msg);
		stripIdent(fromNick);
		
		if(strlen(fromNick) == 0 || areEqual(fromNick, displayNick, COMPARE_INFINITE))
		{			
			// itsa me!
			if(tabFromName(msg) != -1) // already open
				return;
			
			addTab(msg);
			activeTab = tabFromName(msg);
			createIRCButtons();
			isTopic = false;
		}
		else
		{
			// someone else has joined
			addNick(tabs[tabFromName(msg)].screen, fromNick);
			
			sprintf(cStr, "\n%s", irc_join);
			osprintf(tStr, cStr, fromNick);
			memset(cStr, 0, 1024);
			concatCommand(cStr, makeCommand(false, false, ircColorNICK), strlen(tStr));
			concatTab(tabFromName(msg), tStr, cStr, UNIMPORTANT);				
		}
		
		return;
	}
	
	if(strcmp(command, "PART") == 0)
	{
		stripIdent(fromNick);
		
		if(strlen(fromNick) == 0 || areEqual(fromNick, displayNick, COMPARE_INFINITE))
			return;
		
		if(strchr(msg, ' ') == 0) // no part msg
		{
			subNick(tabs[tabFromName(msg)].screen, fromNick);
			
			sprintf(cStr, "\n%s", irc_partnomessage);
			osprintf(tStr, cStr, fromNick);
			memset(cStr, 0, 1024);
			concatCommand(cStr, makeCommand(false, false, ircColorNICK), strlen(tStr));
			concatTab(tabFromName(msg), tStr, cStr, UNIMPORTANT);
			
			return;
		}
		
		char toNick[128];
		memset(toNick, 0, 128);
		
		tLen = strchr(msg, ' ') - msg;
		memcpy(toNick, msg, tLen);
		
		subNick(tabs[tabFromName(toNick)].screen, fromNick);
		
		msg += tLen + 2;
		
		sprintf(cStr, "\n%s", irc_partmessage);
		osprintf(tStr, cStr, fromNick, msg);
		memset(cStr, 0, 1024);
		formatText(tStr, cStr, false, false, ircColorNICK);
		concatTab(tabFromName(toNick), tStr, cStr, UNIMPORTANT);
		
		return;
	}
	
	if(strcmp(command, "KICK") == 0)
	{
		stripIdent(fromNick);
		
		char channel[128];
		memset(channel, 0, 128);
		
		tLen = strchr(msg, ' ') - msg;
		memcpy(channel, msg, tLen);
		
		msg += tLen + 1;
		
		char toNick[128];
		memset(toNick, 0, 128);
		
		if(strchr(msg, ' ') == NULL)
		{
			strcpy(toNick, msg);
			msg = NULL;
		}
		else
		{
			tLen = strchr(msg, ' ') - msg;
			memcpy(toNick, msg, tLen);
			
			msg += tLen + 1;
		}
		
		setGlobalError(ERROR_IRC_KICKREASON);
		char *reason = (char *)safeMalloc(MAX_REASON + 1); // checked
		setGlobalError(ERROR_NONE);
		
		if(msg == NULL)
			strncpy(reason, fromNick, MAX_REASON);
		else if(msg[0] != ':')
			strncpy(reason, fromNick, MAX_REASON);
		else
		{
			msg++;
			strncpy(reason, msg, MAX_REASON);
		}
		
		if(areEqual(toNick, displayNick, COMPARE_INFINITE)) // you've been kicked
		{
			sprintf(cStr, "\n%s", irc_youkicked);
			osprintf(tStr, cStr, channel, fromNick, reason);
			memset(cStr, 0, 1024);
			formatText(tStr, cStr, false, false, ircColorNICK);
			concatTab(0, tStr, cStr, UNIMPORTANT);
			
			int tab = tabFromName(channel);
			
			if(tab != -1)
			{
				if(tab == activeTab)
					activeTab--;
				removeTab(tab);
			}
		}
		else // someone else is kicked
		{
			sprintf(cStr, "\n%s", irc_kicked);
			osprintf(tStr, cStr, toNick, fromNick, reason);
			memset(cStr, 0, 1024);
			formatText(tStr, cStr, false, false, ircColorNICK);
			
			int tab = tabFromName(channel);
			
			if(tab != -1)
			{
				concatTab(tab, tStr, cStr, UNIMPORTANT);
				subNick(tabs[tab].screen, toNick);
			}
		}
		
		free(reason);
		
		return;
	}
	
	if(strcmp(command, "QUIT") == 0)
	{
		stripIdent(fromNick);
		
		if(strlen(fromNick) == 0 || areEqual(fromNick, displayNick, COMPARE_INFINITE))
		{
			// itsa me, ignore it, but this should never happen....ever.
			
			return;
		}
		
		if(msg == NULL) // no quit msg
		{
			sprintf(cStr, "\n%s", irc_quitnomessage);
			osprintf(tStr, cStr, fromNick);
			memset(cStr, 0, 1024);
			concatCommand(cStr, makeCommand(false, false, ircColorNICK), strlen(tStr));
		}
		else
		{		
			msg++;
			
			sprintf(cStr, "\n%s", irc_quitmessage);
			osprintf(tStr, cStr, fromNick, msg);
			memset(cStr, 0, 1024);
			formatText(tStr, cStr, false, false, ircColorNICK);
		}
		
		for(int i=1;i<tabCount;i++)
		{
			if(isChan(tabs[i].name))
			{
				if(isNickInScreen(tabs[i].screen, fromNick))
				{
					concatTab(i, tStr, cStr, UNIMPORTANT);
					subNick(tabs[i].screen, fromNick);
				}
			}
		}
		
		return;
	}
	
	if(strcmp(command, "TOPIC") == 0)
	{
		stripIdent(fromNick);
		
		char toNick[128];
		memset(toNick, 0, 128);
		
		tLen = strchr(msg, ' ') - msg;
		memcpy(toNick, msg, tLen);
		
		msg += tLen + 2;
		
		sprintf(cStr, "\n%s", irc_settopic);
		osprintf(tStr, cStr, fromNick, msg);
		memset(cStr, 0, 1024);
		formatText(tStr, cStr, false, false, ircColorTOPIC);
		concatTab(tabFromName(toNick), tStr, cStr, UNIMPORTANT);
		
		return;
	}
	
	if(strcmp(command, "NICK") == 0)
	{
		stripIdent(fromNick);
		
		if(areEqual(fromNick, displayNick, COMPARE_INFINITE))
		{
			// nick changed by command or forcibly
			
			msg++;			
			strcpy(displayNick, msg);
			
			sprintf(cStr, "\n%s", irc_younick);
			osprintf(tStr, cStr, msg);
			memset(cStr, 0, 1024);
			formatText(tStr, cStr, false, false, ircColorNICK);
			
			if(tabCount > 1)
			{			
				for(int i=1;i<tabCount;i++)
				{
					if(isChan(tabs[i].name))
					{
						changeNick(tabs[i].screen, fromNick, msg);
						concatTab(i, tStr, cStr, UNIMPORTANT);
					}
				}
			}
			else
				concatTab(0, tStr, cStr, UNIMPORTANT);
			
			return;
		}
		
		// someone else changed
		
		msg++;			
		sprintf(cStr, "\n%s", irc_nick);
		osprintf(tStr, cStr, fromNick, msg);
		memset(cStr, 0, 1024);
		formatText(tStr, cStr, false, false, ircColorNICK);
		
		if(tabCount > 1)
		{		
			for(int i=1;i<tabCount;i++)
			{
				if(isChan(tabs[i].name))
				{
					if(isNickInScreen(tabs[i].screen, fromNick))
					{
						changeNick(tabs[i].screen, fromNick, msg);
						concatTab(i, tStr, cStr, UNIMPORTANT);
					}
				}
				else
				{
					if(areEqual(tabs[i].name, fromNick, COMPARE_INFINITE)) // change it
						strcpy(tabs[i].name, msg);
				}
			}
		}
		
		return;
	}
	
	if(strcmp(command, "INVITE") == 0)
	{
		stripIdent(fromNick);
		
		tLen = strchr(msg, ' ') - msg;		
		msg += tLen + 2;
		
		sprintf(cStr, "\n%s", irc_invite);
		osprintf(tStr, cStr, fromNick, msg);
		memset(cStr, 0, 1024);
		formatText(tStr, cStr, false, false, ircColorSERVER);
		concatTab(0, tStr, cStr, UNIMPORTANT);
		
		return;
	}
	
	if(strcmp(command, "ERROR") == 0)
	{
		msg ++;
		
		sprintf(cStr, "\n%s", irc_error);
		osprintf(tStr, cStr, msg);
		memset(cStr, 0, 1024);
		concatCommand(cStr, makeCommand(false, false, ircColorSERVER), strlen(tStr));
		concatTab(0, tStr, cStr, UNIMPORTANT);
		
		return;
	}
	
	if(strcmp(command, "MODE") == 0 || strcmp(command, "324") == 0)
	{
		if(strcmp(command, "324") == 0)
		{
			tLen = strchr(msg, ' ') - msg;
			msg += tLen + 1;
		}
		
		stripIdent(fromNick);
		
		char toNick[128];
		memset(toNick, 0, 128);
		
		tLen = strchr(msg, ' ') - msg;
		memcpy(toNick, msg, tLen);
		
		msg += tLen + 1;
		
		if(isChan(toNick)) // channel mode
		{	
			if(tabFromName(toNick) == -1) // make sure we don't screw up
				return;
			
			if(strcmp(command, "324") != 0) // only display if it's not automated
			{
				sprintf(cStr, "\n%s", irc_mode);
				osprintf(tStr, cStr, fromNick, msg);
				memset(cStr, 0, 1024);
				concatCommand(cStr, makeCommand(false, false, ircColorMODE), strlen(tStr));
				concatTab(tabFromName(toNick), tStr, cStr, UNIMPORTANT);
			}
			
			char modes[128];
			memset(modes, 0, 128);
			
			if(strchr(msg, ' ') != NULL)
			{
				tLen = strchr(msg, ' ') - msg;
				memcpy(modes, msg, tLen);
				
				msg += tLen + 1; // all things after this are things relating to the mode
			}
			else			
			{
				strcpy(modes, msg);
			}
			
			int modeDir = 0;
			
			char *modeSet = modes;
			
			while(modeSet[0] != 0)
			{
				if(modeSet[0] == '+')
					modeDir = 1;
				else if(modeSet[0] == '-')
					modeDir = -1;
				else // mode
				{
					if(strchr(ircModes, modeSet[0]) != NULL) // its a nick mode
					{
						char param[128];
						memset(param, 0, 128);
						
						if(strchr(msg, ' ') != NULL)
						{							
							tLen = strchr(msg, ' ') - msg;
							memcpy(param, msg, tLen);
							
							msg += tLen + 1; // move to next item
						}
						else // last mode, safe to copy
						{
							strcpy(param, msg);
						}
						
						changeModes(tabs[tabFromName(toNick)].screen, param, modeSet[0], modeDir);
					}
					else if(modeSet[0] == 'l') // channel limit
					{
						if(modeDir == -1)
						{
							tabs[tabFromName(toNick)].screen->hasLimit = false;
						}
						else
						{
							tabs[tabFromName(toNick)].screen->hasLimit = true;
							
							if(strchr(msg, ' ') != NULL)
							{
								char param[128];
								memset(param, 0, 128);
								
								tLen = strchr(msg, ' ') - msg;
								memcpy(param, msg, tLen);
								
								msg += tLen + 1; // move to next item
								
								int i = 0;
								tabs[tabFromName(toNick)].screen->limit = 0;
								
								while(param[i] != 0)
								{
									if(isNumerical(param[i]))
									{
										tabs[tabFromName(toNick)].screen->limit *= 10;
										tabs[tabFromName(toNick)].screen->limit += param[i] - '0';
									}
									
									i++;
								}
							}
							else // last mode, safe to copy
							{
								int i = 0;
								tabs[tabFromName(toNick)].screen->limit = 0;
								
								while(msg[i] != 0)
								{
									if(isNumerical(msg[i]))
									{
										tabs[tabFromName(toNick)].screen->limit *= 10;
										tabs[tabFromName(toNick)].screen->limit += msg[i] - '0';
									}
									
									i++;
								}
							}
						}					
					}
					else if(modeSet[0] == 'k') // channel key
					{
						if(modeDir == -1)
						{
							if(strchr(msg, ' ') != NULL) // returns the key even if we are removing it for some reason							
							{
								tLen = strchr(msg, ' ') - msg;								
								msg += tLen + 1; // move to next item
							}
							
							tabs[tabFromName(toNick)].screen->hasKey = false;
						}
						else
						{
							tabs[tabFromName(toNick)].screen->hasKey = true;
							
							if(strchr(msg, ' ') != NULL)
							{
								char param[128];
								memset(param, 0, 128);
								
								tLen = strchr(msg, ' ') - msg;
								memcpy(param, msg, tLen);
								
								msg += tLen + 1; // move to next item
								
								strcpy(tabs[tabFromName(toNick)].screen->key, param);
							}
							else // last mode, safe to copy
							{
								strcpy(tabs[tabFromName(toNick)].screen->key, msg);
							}
						}
					}
					else if(modeSet[0] == 'b') // ban list
					{
						if(strchr(msg, ' ') != NULL)
						{	
							tLen = strchr(msg, ' ') - msg;
							msg += tLen + 1; // move to next item
						}
					}
					else if(modeSet[0] == 'e') // exception list
					{
						if(strchr(msg, ' ') != NULL)
						{	
							tLen = strchr(msg, ' ') - msg;
							msg += tLen + 1; // move to next item
						}
					}
					else if(modeSet[0] == 'I') // invite list
					{
						if(strchr(msg, ' ') != NULL)
						{	
							tLen = strchr(msg, ' ') - msg;
							msg += tLen + 1; // move to next item
						}
					}
					else // it's nothing that requires a secondary param, safe to append
						appendMode(tabs[tabFromName(toNick)].screen->modes, modeSet[0], modeDir);					
				}
				
				modeSet++;
			}
			
			return;
		}
		else
		{
			msg++; 
			
			int modeDir = 0;
			
			char *modeSet = msg;
			
			while(modeSet[0] != 0)
			{
				if(modeSet[0] == '+')
					modeDir = 1;
				else if(modeSet[0] == '-')
					modeDir = -1;
				else // mode
				{
					appendMode(myModes, modeSet[0], modeDir);					
				}
				
				modeSet++;
			}
			
			sprintf(cStr, "\n%s", irc_mode);
			osprintf(tStr, cStr, fromNick, msg);
			memset(cStr, 0, 1024);
			concatCommand(cStr, makeCommand(false, false, ircColorMODE), strlen(tStr));
			concatTab(0, tStr, cStr, UNIMPORTANT);
			
			return;
		}
	}
	
	if(strlen(command) == 3)
	{
		if(isNumerical(command[0]) && isNumerical(command[1]) && isNumerical(command[2])) // check for numerical reply
		{
			int numericReply = 0;
			
			numericReply += (command[0] - '0') * 100;
			numericReply += (command[1] - '0') * 10;
			numericReply += (command[2] - '0');
			
			char toNick[128];
			memset(toNick, 0, 128);
			
			tLen = strchr(msg, ' ') - msg;
			memcpy(toNick, msg, tLen);
			
			msg += tLen + 1;
			
			stripIdent(fromNick);
			
			switch(numericReply)
			{
				case 1: // ignore
					strcpy(displayNick, toNick);
					concatTab(0, "\n", NULL, UNIMPORTANT);
					break;
				case 2: // ignore
				case 3: // ignore				
					break;
				case 4: 
				case 5:
					if(setNetwork(msg))
					{
						sprintf(cStr, "\n%s", irc_network);
						osprintf(tStr, cStr, curNetwork);
						memset(cStr, 0, 1024);
						concatCommand(cStr, makeCommand(false, false, ircColorSERVER), strlen(tStr));
						concatTab(0, tStr, cStr, UNIMPORTANT);
					}
					if(setNickLen(msg))
					{
						sprintf(cStr, "\n%s", irc_nicklength);
						osprintf(tStr, cStr, maxNickLen);
						memset(cStr, 0, 1024);
						concatCommand(cStr, makeCommand(false, false, ircColorSERVER), strlen(tStr));
						concatTab(0, tStr, cStr, UNIMPORTANT);
					}
					if(parseModes(msg))
					{					
						sprintf(cStr, "\n%s", irc_supportedmodes);
						osprintf(tStr, cStr, ircModes, ircSymbols);
						memset(cStr, 0, 1024);
						concatCommand(cStr, makeCommand(false, false, ircColorSERVER), strlen(tStr));
						concatTab(0, tStr, cStr, UNIMPORTANT);
					}
					
					break;
				case 7: // ignore
				case 8: // ignore
				case 251: // ignore
				case 252: // ignore
				case 253: // ignore
				case 254: // ignore
				case 255: // ignore
				case 265: // ignore
				case 266: // ignore
					break;
				case 375:
					sprintf(tStr, "\n\n%s", irc_motdstart);
					concatTab(0, tStr, NULL, UNIMPORTANT);
					break;
				case 376:
					sprintf(tStr, "\n%s\n", irc_motdend);
					concatTab(0, tStr, NULL, UNIMPORTANT);
					toPerform = true;
					break;
				case 422:
					sprintf(tStr, "\n%s\n", irc_nomotd);
					concatTab(0, tStr, NULL, UNIMPORTANT);
					toPerform = true;
					break;
				case 372: // motd text
					msg++;
					sprintf(tStr, "\n%s", msg);
					formatText(tStr, cStr, false, false, ircColorNORMAL);
					concatTab(0, tStr, cStr, UNIMPORTANT);
					break;
				case 433:
					if(connectMode == C_SENTNICK)
					{
						char tNick[NICK_LEN];
						memset(tNick, 0, NICK_LEN);
						checkNickLen(usealtNickName, tNick);
						
						sprintf(tStr, "NICK %s\r\n", tNick);
						sendData(ircSocket, tStr, strlen(tStr));
						
						connectMode = C_SENTALTNICK;
						return;
					}
					
					if(connectMode == C_SENTALTNICK)
					{
						sprintf(tStr, "\n%s", irc_altnickinuse);
						concatCommand(cStr, makeCommand(false, false, ircColorSERVER), strlen(tStr));
						concatTab(0, tStr, cStr, UNIMPORTANT);
						
						closeServerConnection();
						
						return;
					}
					
					if(connectMode == C_NICKCHANGE)
					{
						sprintf(tStr, "\n%s", irc_nickinuse);
						concatCommand(cStr, makeCommand(false, false, ircColorSERVER), strlen(tStr));
						concatTab(0, tStr, cStr, UNIMPORTANT);
					}
					
					break;
				case 331: // no topic set
					*strchr(msg, ' ') = 0;
					
					sprintf(cStr, "\n%s", irc_notopic);
					osprintf(tStr, cStr);
					memset(cStr, 0, 1024);
					concatCommand(cStr, makeCommand(false, false, ircColorTOPIC), strlen(tStr));
					concatTab(tabFromName(msg), tStr, cStr, UNIMPORTANT);
					
					break;
				case 332: // channel topic					
					memset(toNick, 0, 128);
					
					tLen = strchr(msg, ' ') - msg;
					memcpy(toNick, msg, tLen);
					
					msg += tLen + 2;
					
					if(tabFromName(toNick) != -1) // ignore if channel doesn't exist
					{
						sprintf(cStr, "\n%s", irc_topic);
						osprintf(tStr, cStr, msg);
						memset(cStr, 0, 1024);
						formatText(tStr, cStr, false, false, ircColorTOPIC);
						concatTab(tabFromName(toNick), tStr, cStr, UNIMPORTANT);
						isTopic = true;
					}
					break;
				case 341: // successful invite
					memset(toNick, 0, 128);
					
					tLen = strchr(msg, ' ') - msg;
					memcpy(toNick, msg, tLen);
					msg += tLen + 1;
					
					sprintf(cStr, "\n%s", irc_341);
					osprintf(tStr, cStr, toNick, msg);
					memset(cStr, 0, 1024);
					concatCommand(cStr, makeCommand(false, false, ircColorSERVER), strlen(tStr));
					concatTab(0, tStr, cStr, UNIMPORTANT);
					
					break;
				case 353: // channel nick list
					msg += 2;
					
					memset(toNick, 0, 128);
					
					tLen = strchr(msg, ' ') - msg;
					memcpy(toNick, msg, tLen);
					
					msg += tLen + 2;
					
					if(!isTopic)
					{
						sprintf(cStr, "\n%s", irc_notopic);
						osprintf(tStr, cStr);
						memset(cStr, 0, 1024);
						concatCommand(cStr, makeCommand(false, false, ircColorTOPIC), strlen(tStr));
						concatTab(tabFromName(toNick), tStr, cStr, UNIMPORTANT);
						isTopic = true;
					}
					
					while(strchr(msg, ' ') != NULL)
					{
						char addNickName[NICK_LEN];
						
						memset(addNickName, 0, NICK_LEN);
						
						tLen = strchr(msg, ' ') - msg;
						memcpy(addNickName, msg, tLen);
						
						msg += tLen + 1;
						
						addNick(tabs[tabFromName(toNick)].screen, addNickName);;
					}
					
					if(strlen(msg) > 0)
						addNick(tabs[tabFromName(toNick)].screen, msg);
					
					break;
				case 366: // end channel nick list					
					memset(toNick, 0, 128);
					
					tLen = strchr(msg, ' ') - msg;
					memcpy(toNick, msg, tLen);
					
					sortNicks(tabFromName(toNick));
					dispNicks(tabFromName(toNick));
					break;
				case 311: // whois user
				{
					char uName[NICK_LEN];
					memset(uName, 0, NICK_LEN);
					tLen = strchr(msg, ' ') - msg;
					memcpy(uName, msg, tLen);
					
					msg += tLen + 1;
					
					char ident[30];
					memset(ident, 0, 30);
					tLen = strchr(msg, ' ') - msg;
					memcpy(ident, msg, tLen);
					
					msg += tLen + 1;
					
					char host[100];
					memset(host, 0, 100);
					tLen = strchr(msg, ' ') - msg;
					memcpy(host, msg, tLen);
					
					msg += tLen + 4;
					
					sprintf(cStr, "\n\n%s", irc_311);
					osprintf(tStr, cStr, uName, ident, host, msg);
					memset(cStr, 0, 1024);
					concatCommand(cStr, makeCommand(false, false, ircColorSERVER), strlen(tStr));
					concatTab(0, tStr, cStr, UNIMPORTANT);
					
					break;
				}
				case 314: // whois user
				{
					char uName[NICK_LEN];
					memset(uName, 0, NICK_LEN);
					tLen = strchr(msg, ' ') - msg;
					memcpy(uName, msg, tLen);
					
					msg += tLen + 1;
					
					char ident[30];
					memset(ident, 0, 30);
					tLen = strchr(msg, ' ') - msg;
					memcpy(ident, msg, tLen);
					
					msg += tLen + 1;
					
					char host[100];
					memset(host, 0, 100);
					tLen = strchr(msg, ' ') - msg;
					memcpy(host, msg, tLen);
					
					msg += tLen + 4;
					
					sprintf(cStr, "\n\n%s", irc_314);
					osprintf(tStr, cStr, uName, ident, host, msg);
					memset(cStr, 0, 1024);
					concatCommand(cStr, makeCommand(false, false, ircColorSERVER), strlen(tStr));
					concatTab(0, tStr, cStr, UNIMPORTANT);
					
					break;
				}
				case 317: // idle time, ignore now
				case 250: // ignore now, highest connect count apparently
				case 378: // ignore for now, 'info'
				case 329: // channel create date
				case 333: // who knows
				case 451: // register first
				case 338: // something with whois
				case 439: // hold on a second
				case 042: // unknown reply from rizon
					break;				
				case 431:
					sprintf(tStr, "\n%s", irc_nonickgiven);
					concatCommand(cStr, makeCommand(false, false, ircColorSERVER), strlen(tStr));
					concatTab(0, tStr, cStr, UNIMPORTANT);
					break;			
				case 432:
					sprintf(tStr, "\n%s", irc_erroneousnick);
					concatCommand(cStr, makeCommand(false, false, ircColorSERVER), strlen(tStr));
					concatTab(0, tStr, cStr, UNIMPORTANT);
					break;		
				case 465:
					sprintf(tStr, "\n%s", irc_serverbanned);
					concatCommand(cStr, makeCommand(false, false, ircColorSERVER), strlen(tStr));
					concatTab(0, tStr, cStr, UNIMPORTANT);
					
					closeServerConnection();
					break;
				case 403: // channel does not exist
					memset(toNick, 0, 128);
					
					tLen = strchr(msg, ' ') - msg;
					memcpy(toNick, msg, tLen);
					
					sprintf(cStr, "\n%s", irc_403);
					osprintf(tStr, cStr, toNick);
					concatCommand(cStr, makeCommand(false, false, ircColorSERVER), strlen(tStr));
					concatTab(0, tStr, cStr, UNIMPORTANT);
					break;
				case 404: // cannot speak
					memset(toNick, 0, 128);
					
					tLen = strchr(msg, ' ') - msg;
					memcpy(toNick, msg, tLen);
					
					sprintf(tStr, "\n%s", irc_cantsend);
					concatCommand(cStr, makeCommand(false, false, ircColorSERVER), strlen(tStr));
					concatTab(tabFromName(toNick), tStr, cStr, UNIMPORTANT);
					break;					
				case 475:
					memset(toNick, 0, 128);
					
					tLen = strchr(msg, ' ') - msg;
					memcpy(toNick, msg, tLen);
					
					sprintf(cStr, "\n%s", irc_475);
					osprintf(tStr, cStr, toNick);
					memset(cStr, 0, 1024);
					concatCommand(cStr, makeCommand(false, false, ircColorSERVER), strlen(tStr));
					concatTab(0, tStr, cStr, UNIMPORTANT);
					break;				
				case 477:
					memset(toNick, 0, 128);
					
					tLen = strchr(msg, ' ') - msg;
					memcpy(toNick, msg, tLen);
					
					sprintf(cStr, "\n%s", irc_477);
					osprintf(tStr, cStr, toNick);
					memset(cStr, 0, 1024);
					concatCommand(cStr, makeCommand(false, false, ircColorSERVER), strlen(tStr));
					concatTab(0, tStr, cStr, UNIMPORTANT);
					break;
				case 473:
					memset(toNick, 0, 128);
					
					tLen = strchr(msg, ' ') - msg;
					memcpy(toNick, msg, tLen);
					
					sprintf(cStr, "\n%s", irc_473);
					osprintf(tStr, cStr, toNick);
					memset(cStr, 0, 1024);
					concatCommand(cStr, makeCommand(false, false, ircColorSERVER), strlen(tStr));
					concatTab(0, tStr, cStr, UNIMPORTANT);
					break;
				case 471:
					memset(toNick, 0, 128);
					
					tLen = strchr(msg, ' ') - msg;
					memcpy(toNick, msg, tLen);
					
					sprintf(cStr, "\n%s", irc_471);
					osprintf(tStr, cStr, toNick);
					memset(cStr, 0, 1024);
					concatCommand(cStr, makeCommand(false, false, ircColorSERVER), strlen(tStr));
					concatTab(0, tStr, cStr, UNIMPORTANT);
					break;
				case 474:
					memset(toNick, 0, 128);
					
					tLen = strchr(msg, ' ') - msg;
					memcpy(toNick, msg, tLen);
					
					sprintf(cStr, "\n%s", irc_474);
					osprintf(tStr, cStr, toNick);
					memset(cStr, 0, 1024);
					concatCommand(cStr, makeCommand(false, false, ircColorSERVER), strlen(tStr));
					concatTab(0, tStr, cStr, UNIMPORTANT);
					break;
				case 461: // not enough parameters
					*strchr(msg, ' ') = 0;					
					
					sprintf(cStr, "\n%s", irc_461);
					osprintf(tStr, cStr, msg);
					memset(cStr, 0, 1024);
					concatCommand(cStr, makeCommand(false, false, ircColorSERVER), strlen(tStr));
					concatTab(0, tStr, cStr, UNIMPORTANT);
					
					break;
				case 442: // you aren't on that channel
					*strchr(msg, ' ') = 0;					
					
					sprintf(cStr, "\n%s", irc_442);
					osprintf(tStr, cStr, msg);
					memset(cStr, 0, 1024);
					concatCommand(cStr, makeCommand(false, false, ircColorSERVER), strlen(tStr));
					concatTab(0, tStr, cStr, UNIMPORTANT);
					
					break;
				case 443: // user already on that channel
					memset(toNick, 0, 128);
					
					tLen = strchr(msg, ' ') - msg;
					memcpy(toNick, msg, tLen);
					msg += tLen + 1;
					
					*strchr(msg, ' ') = 0;					
					
					sprintf(cStr, "\n%s", irc_443);
					osprintf(tStr, cStr, toNick, msg);
					memset(cStr, 0, 1024);
					concatCommand(cStr, makeCommand(false, false, ircColorSERVER), strlen(tStr));
					concatTab(0, tStr, cStr, UNIMPORTANT);
					
					break;
				case 482: // you aren't operator
					*strchr(msg, ' ') = 0;					
					
					sprintf(cStr, "\n%s", irc_482);
					osprintf(tStr, cStr);
					memset(cStr, 0, 1024);
					concatCommand(cStr, makeCommand(false, false, ircColorSERVER), strlen(tStr));
					concatTab(tabFromName(msg), tStr, cStr, UNIMPORTANT);
					
					break;
				case 312: // whois server
				{
					char uName[NICK_LEN];
					memset(uName, 0, NICK_LEN);
					tLen = strchr(msg, ' ') - msg;
					memcpy(uName, msg, tLen);
					
					msg += tLen + 1;
					
					char server[128];
					memset(server, 0, 128);
					tLen = strchr(msg, ' ') - msg;
					memcpy(server, msg, tLen);
					
					msg += tLen + 2;
					
					sprintf(cStr, "\n%s", irc_312);
					osprintf(tStr, cStr, uName, server, msg);
					memset(cStr, 0, 1024);
					concatCommand(cStr, makeCommand(false, false, ircColorSERVER), strlen(tStr));
					concatTab(0, tStr, cStr, UNIMPORTANT);
					
					break;
				}
				case 313: // oper server
				{
					char uName[NICK_LEN];
					memset(uName, 0, NICK_LEN);
					tLen = strchr(msg, ' ') - msg;
					memcpy(uName, msg, tLen);
					
					sprintf(cStr, "\n%s", irc_313);
					osprintf(tStr, cStr, uName);
					memset(cStr, 0, 1024);
					concatCommand(cStr, makeCommand(false, false, ircColorSERVER), strlen(tStr));
					concatTab(0, tStr, cStr, UNIMPORTANT);
					
					break;
				}
				case 310: // available for help
				{
					char uName[NICK_LEN];
					memset(uName, 0, NICK_LEN);
					tLen = strchr(msg, ' ') - msg;
					memcpy(uName, msg, tLen);
					
					sprintf(cStr, "\n%s", irc_310);
					osprintf(tStr, cStr, uName);
					memset(cStr, 0, 1024);
					concatCommand(cStr, makeCommand(false, false, ircColorSERVER), strlen(tStr));
					concatTab(0, tStr, cStr, UNIMPORTANT);
					
					break;
				}
				case 671: // using secure connection
				{
					char uName[NICK_LEN];
					memset(uName, 0, NICK_LEN);
					tLen = strchr(msg, ' ') - msg;
					memcpy(uName, msg, tLen);
					
					sprintf(cStr, "\n%s", irc_671);
					osprintf(tStr, cStr, uName);
					memset(cStr, 0, 1024);
					concatCommand(cStr, makeCommand(false, false, ircColorSERVER), strlen(tStr));
					concatTab(0, tStr, cStr, UNIMPORTANT);
					
					break;
				}
				case 307: // registered
				case 320:
				{
					char uName[NICK_LEN];
					memset(uName, 0, NICK_LEN);
					tLen = strchr(msg, ' ') - msg;
					memcpy(uName, msg, tLen);
					
					sprintf(cStr, "\n%s", irc_307);
					osprintf(tStr, cStr, uName);
					memset(cStr, 0, 1024);
					concatCommand(cStr, makeCommand(false, false, ircColorSERVER), strlen(tStr));
					concatTab(0, tStr, cStr, UNIMPORTANT);
					
					break;
				}
				case 301: // whois away status
				{
					char uName[NICK_LEN];
					memset(uName, 0, NICK_LEN);
					tLen = strchr(msg, ' ') - msg;
					memcpy(uName, msg, tLen);
					
					msg += tLen + 2;
					
					sprintf(cStr, "\n%s", irc_301);
					osprintf(tStr, cStr, uName, msg);
					memset(cStr, 0, 1024);
					concatCommand(cStr, makeCommand(false, false, ircColorSERVER), strlen(tStr));
					concatTab(0, tStr, cStr, UNIMPORTANT);
					
					break;
				}
				case 319: // whois channels
				{
					char uName[NICK_LEN];
					memset(uName, 0, NICK_LEN);
					tLen = strchr(msg, ' ') - msg;
					memcpy(uName, msg, tLen);
					
					msg += tLen + 2;
					
					sprintf(cStr, "\n%s", irc_319);
					osprintf(tStr, cStr, uName, msg);
					memset(cStr, 0, 1024);
					concatCommand(cStr, makeCommand(false, false, ircColorSERVER), strlen(tStr));
					concatTab(0, tStr, cStr, UNIMPORTANT);
					
					break;
				}
				case 318: // end of whois				
					sprintf(cStr, "\n%s", irc_endwhois);
					osprintf(tStr, cStr);
					memset(cStr, 0, 1024);
					concatCommand(cStr, makeCommand(false, false, ircColorSERVER), strlen(tStr));
					concatTab(0, tStr, cStr, UNIMPORTANT);					
					
					break;
				case 369: // end of whowas
					sprintf(cStr, "\n%s", irc_endwhowas);
					osprintf(tStr, cStr);
					memset(cStr, 0, 1024);
					concatCommand(cStr, makeCommand(false, false, ircColorSERVER), strlen(tStr));
					concatTab(0, tStr, cStr, UNIMPORTANT);					
					
					break;
				case 305: // not away any longer
					sprintf(tStr, "\n%s", irc_back);
					concatCommand(cStr, makeCommand(false, false, ircColorSERVER), strlen(tStr));
					concatTab(0, tStr, cStr, UNIMPORTANT);
					break;
				case 306: // away
					sprintf(tStr, "\n%s", irc_away);
					concatCommand(cStr, makeCommand(false, false, ircColorSERVER), strlen(tStr));
					concatTab(0, tStr, cStr, UNIMPORTANT);
					break;
				case 401: // no such user/channel
					memset(toNick, 0, 128);
					
					tLen = strchr(msg, ' ') - msg;
					memcpy(toNick, msg, tLen);
					
					sprintf(cStr, "\n%s", irc_nouser);
					osprintf(tStr, cStr, toNick);
					concatCommand(cStr, makeCommand(false, false, ircColorSERVER), strlen(tStr));
					concatTab(0, tStr, cStr, UNIMPORTANT);
					break;
				case 406: // there was no user
					memset(toNick, 0, 128);
					
					tLen = strchr(msg, ' ') - msg;
					memcpy(toNick, msg, tLen);
					
					sprintf(cStr, "\n%s", irc_406);
					osprintf(tStr, cStr, toNick);
					concatCommand(cStr, makeCommand(false, false, ircColorSERVER), strlen(tStr));
					concatTab(0, tStr, cStr, UNIMPORTANT);
					break;
				case 408: // +c mode is on
				{					
					char channel[128];
					memset(channel, 0, 128);
					tLen = strchr(msg, ' ') - msg;
					memcpy(channel, msg, tLen);
					
					msg += tLen + 2;
					
					sprintf(tStr, "\n%s", msg);
					memset(cStr, 0, 1024);
					formatText(tStr, cStr, false, false, ircColorSERVER);					
					concatTab(tabFromName(channel), tStr, cStr, UNIMPORTANT);
					
					break;
				}
				case 421: // unknown command
					tLen = strchr(msg, ' ') - msg;
					
					char unknownCMD[128];
					memset(unknownCMD, 0, 128);						
					memcpy(unknownCMD, msg, tLen);
					
					sprintf(cStr, "\n%s", irc_421);
					osprintf(tStr, cStr, unknownCMD);
					memset(cStr, 0, 1024);
					concatCommand(cStr, makeCommand(false, false, ircColorSERVER), strlen(tStr));
					concatTab(0, tStr, cStr, UNIMPORTANT);
					
					break;
				default:
					// debug only
					
					concatBuffer(console, "\nUNSUPPORTED COMMAND: '", NULL);	
					concatBuffer(console, msg2, NULL);	
					concatBuffer(console, "'", NULL);
					break;
			}
			
			return;
		}
	}
	
	// debug only
	
	concatBuffer(console, "\nUNSUPPORTED COMMAND: '", NULL);	
	concatBuffer(console, msg2, NULL);	
	concatBuffer(console, "'", NULL);
}

bool receiveBuffer()
{
	int bufferLen = strlen(serverBuffer);
	if(bufferLen == 0)
	{
		return false;
	}
	
	// remove any null messages
	while(serverBuffer[0] == '\r' && serverBuffer[1] == '\n')
	{
		memmove(serverBuffer, serverBuffer + 2, SB_SIZE - 2);
		
		serverBuffer[SB_SIZE - 1] = 0;
		serverBuffer[SB_SIZE - 2] = 0;
	}
	
	// search for current message
	int messageLength = -1;	
	for(int i=0;i<bufferLen;i++)
	{
		if(serverBuffer[i] == '\r')
		{
			// possible match
			if(serverBuffer[i+1] == '\n')
			{
				// set message length and exit
				messageLength = i;
				break;
			}
		}
	}
	
	if(messageLength == -1)
	{
		// we haven't recieved the whole message yet
		return false;
	}
	
	// temporary storage for the message
	setGlobalError(ERROR_IRC_PARSEMESSAGE);
	char *inBuffer = (char *)safeMalloc(messageLength + 1); // checked
	setGlobalError(ERROR_NONE);
	
	memcpy(inBuffer, serverBuffer, messageLength);
	
	// erase the message from the queue
	memmove(serverBuffer, serverBuffer + messageLength + 2, SB_SIZE - (messageLength + 2));
	memset(serverBuffer + (SB_SIZE - (messageLength + 2)), 0, messageLength + 2);
	
	// send message to parser
	parseMessage(inBuffer);
	
	// free memory and exit
	free(inBuffer);
	return true;
}

void addAndRecieve(char *toAdd, int addLen)
{
	if(strlen(toAdd) + strlen(serverBuffer) >= SB_SIZE)
	{
		fatalHalt(l_fatalirc);
	}
	
	if(addLen <= 0)
	{
		return;
	}
	
	strncat(serverBuffer, toAdd, addLen);
	memset(toAdd, 0, addLen);
	
	while(receiveBuffer());
}

void closeServerConnection()
{
	if(!connected)
		return;
	
	char tStr[1024];
	sprintf(tStr, "QUIT :%s\r\n", quitMessage); // substitute for quit message here
	setBlocking(ircSocket);
	sendData(ircSocket, tStr, strlen(tStr));
	
	// wait 100ms 
	for(int i=0;i<6;i++)
	{
		vblankWait(); 
	}
	
	shutdown(ircSocket, 2); // request shutdown for both read/write 
	
	// wait 100ms 
	for(int i=0;i<6;i++)
	{
		vblankWait(); 
	}
	
	wifiClose(ircSocket);
	
	char cStr[1024];
	sprintf(tStr, "\n%s", l_ircdisconnect);	
	concatCommand(cStr, makeCommand(true, false, ircColorDSO), strlen(tStr));	
	concatBuffer(console, tStr, cStr);		

	ircMode = IRC_CLOSED;
	
	connected = false;
	
	memset(quitMessage, 0, 256);
	strcpy(quitMessage, "DSOrganize IRC");
}

void connectToServer(char *s)
{
	if(connected == true)
		closeServerConnection();
	
	// format nicknames
	
	PERSONAL_DATA p = *PersonalData;
	char name[20];
	
	memset(name, 0, 20);
	uint16 x;
	
	for(x=0;x<p.nameLen;x++)
		name[x] = (char)(p.name[x] & 0xFF);

	// nick
	if(strlen(nickName) > 0)
		strcpy(useNickName, nickName);
	else
		strcpy(useNickName, name);
	
	// altnick
	if(strlen(altnickName) > 0)
		strcpy(usealtNickName, altnickName);
	else
	{
		strcpy(usealtNickName, name);
		strcat(usealtNickName, "1");
	}
	
	// ok go
	
	strcpy(lastServer, s);
	
	if(strchr(s,':') == NULL)
		strcat(s, ":6667");
	
	char tStr[100];
	strcpy(tStr, "http://");
	strcat(tStr, s);
	
	URL_TYPE site;
	
	if(!urlFromString(tStr, &site))
	{
		return;
	}
	
	fillIPFromServer(&site);
	strcpy(curServer,site.server);
	
	ircSocket = wifiConnectIP(site.serverIP, site.serverPort, false);
	destroyURL(&site);
	
	char cStr[1024];
	sprintf(tStr, "\n%s %s", l_ircconnectingto, s);	
	concatCommand(cStr, makeCommand(true, false, ircColorDSO), strlen(tStr));	
	concatBuffer(console, tStr, cStr);
	
	memset(myModes, 0, 20);
	memset(ircModes, 0, 10);
	memset(ircSymbols, 0, 10);
	memset(curNetwork, 0, 128);
	memset(displayNick, 0, NICK_LEN);
	ircCount = 0;
	connectMode = C_STARTED;	
	connected = true;
	maxNickLen = 0;
	
	waitCount = 0;
	cancelOnWait = true;
}

void ircActivateTab(int x, int y)
{
	setGlobalError(ERROR_IRC_ACTIVATETAB);
	int *widths = (int *) trackMalloc(sizeof(int) * tabCount, "tabs calc"); // checked
	setGlobalError(ERROR_NONE);
	
	for(int i=0;i<tabCount;i++)
		widths[i] = getStringWidth(tabs[i].name, font_gautami_10) + 10;	
	
	int startTab = 0;
	int tmpWidths = 0;
	
	for(int i=0;i<tabCount;i++)
		tmpWidths += widths[i];
	
	if(tmpWidths > 207)
	{	
		if(activeTab > 0)
		{
			tmpWidths = 0;
			int tmpTab = 0;
			
			for(int i=0;i<activeTab;i++)
				tmpWidths += widths[i];
				
			bool keepGoing = true;
			
			while(tmpWidths > (128 - (widths[activeTab] / 2)) && keepGoing)
			{
				tmpWidths -= widths[tmpTab];
				tmpTab++;
				
				if(tmpTab == activeTab)
					tmpWidths = 0;
				
				int curLocation = 24;
				
				keepGoing = false;
				for(int i=tmpTab;i<tabCount;i++)
				{
					if(curLocation + widths[i] > 231)
						keepGoing = true;
					
					curLocation += widths[i];
				}
			}
			
			startTab = tmpTab;
		}
	}
	else
		startTab = 0;
	
	int curLocation = 24;
	
	for(int i=startTab;i<tabCount;i++)
	{
		if(curLocation + widths[i] > 231)
			break;
			
		if(x >= curLocation + 1 && x <= curLocation + widths[i])
		{
			activeTab = i;
			createIRCButtons();
			break;
		}
		
		curLocation += widths[i];
	}
	
	trackFree(widths);
}

void patchGautami()
{
	font_gautami_10[28] = irc_color;
	font_gautami_10[29] = irc_bold;
	font_gautami_10[31] = irc_underline;
	
	int16 *offsets = (int16 *)font_gautami_10[259];
	
	offsets[28] = -2;
	offsets[29] = -2;
	offsets[31] = -2;
}

void resetIRCMode()
{
	if(serverBuffer)
	{
		trackFree(serverBuffer);
		serverBuffer = NULL;
	}
	
	if(tcpTemp)
	{
		trackFree(tcpTemp);
		tcpTemp = NULL;
	}
	
	if(isIRCFixed())
	{
		ircLines = 11;
	}
	else
	{
		ircLines = 13;
	}
	
	if(inputBuffer)
	{
		free(inputBuffer);
		inputBuffer = NULL;
	}
	
	notifyPresent = false;
	ignoreCommands = false;
	
	DRAGON_chdir(d_base);
	
	if(DRAGON_FileExists("notify.wav") == FE_FILE)
		notifyPresent = true;
	
	DRAGON_chdir("/");
	
	setGlobalError(ERROR_IRC_INPUTBUFFER);
	inputBuffer = (char *)safeMalloc(MAX_INPUT+1); // checked
	setGlobalError(ERROR_NONE);
	
	patchGautami();
	
	ampm[0] = ampm[0]; // get ridda warning
	memset(inputBuffer, 0, MAX_INPUT+1);
	pointedText = inputBuffer;
	
	initBackBuffers();
	strUpdated = true;
	ircMode = IRC_DISCONNECTED;
	console = createBuffer(5, 21, 250, 186);
	createTabs();
	
	char tStr[64];
	char cStr[64];
	sprintf(tStr, "%s\n", l_helpavailable);
	formatText(tStr, cStr, true, false, 4);
	concatTab(0, tStr, cStr, IMPORTANT);			
	strcpy(lastServer, ircServer); // set up last server
	reconnect = false;
	
	if(strchr(ircServer,':') == NULL)
		strcat(ircServer, ":6667");
	
	setGlobalError(ERROR_IRC_RECEIVEBUFFER);
	serverBuffer = (char *)trackMalloc(SB_SIZE, "irc recv buffer"); // checked
	setGlobalError(ERROR_IRC_TEMPRECV);
	tcpTemp = (char *)safeMalloc(IRC_RECEIVESIZE); // checked
	setGlobalError(ERROR_NONE);
	
	memset(serverBuffer, 0, SB_SIZE);
	memset(quitMessage, 0, 256);
	strcpy(quitMessage, "DSOrganize IRC");
	
	// Set default color
	ircColors2[0] = genericTextColor;
	ircColors2[1] = genericTextColor;
}

void drawIRCScreen()
{	
	drawTopIRCScreen();
	drawBottomIRCScreen();
}

void sideTabAction(int x, int y)
{
	createIRCButtons();
}

void exitIRC()
{
	disconnectWifi();
	destroyTabs();
	destroySideTabs();
	destroyBuffer(console);
	destroyRecvBuffer();
	
	if(inputBuffer)
	{
		free(inputBuffer);
		inputBuffer = NULL;
	}
	
	if(tcpTemp)
	{
		trackFree(tcpTemp);
		tcpTemp = NULL;
	}
	
	ircMode = IRC_NOTHING;
}

void initIRC()
{
	resetIRCMode();
	resetKBCursor();
	
	createIRC();
}

void ircMoveCursor(int x, int y)
{	
	switch(getControlID())
	{
		case 0: // up button			
			ircUp();
			break;
		case 1: // down button
			ircDown();
			break;
		case 2: // left button
			moveKBCursorRelative(CURSOR_BACKWARD);
			break;
		case 3: // right button
			if(getKBCursor() < inputLength())
			{
				moveKBCursorRelative(CURSOR_FORWARD);	
			}
			break;
	}
	
	invalidateInput();
}

void createIRCButtons()
{
	deleteControl(CONTROL_BACKWARD);
	deleteControl(CONTROL_FORWARD);
	deleteControl(CONTROL_KEYBOARD);
	deleteControl(CONTROL_COLORS);
	deleteControl(CONTROL_LIST);
	
	if((activeTab > 0 && !isChan(tabs[activeTab].name)) || activeTab == 0) // pm
	{
		if(getActiveSideTab() == IRC_TAB_NICKS)
			resetSideTabs();
		
		setSideTabEnabled(IRC_TAB_NICKS, false);		
	}
	else
	{
		setSideTabEnabled(IRC_TAB_NICKS, true);
	}
	
	switch(getActiveSideTab())
	{
		case IRC_TAB_KEYBOARD:
		{
			switch(ircMode)
			{
				case IRC_DISCONNECTED:
				case IRC_CONNECTING:
					addDefaultButtons(NULL, NULL, l_swap, ircBack);
					break;
				case IRC_FAILEDCONNECT:
				case IRC_CLOSEDCONNECTION:
				case IRC_CLOSED:
					addDefaultButtons(l_reconnect, ircForward, l_swap, ircBack);
					break;
				case IRC_CONNECTED:
				case IRC_READY:
					addDefaultButtons(l_send, ircForward, l_swap, ircBack);
					break;
			}
			
			addKeyboard(10, 0, true, false, SCREEN_BG, editIRCAction, NULL);
			
			break;
		}
		case IRC_TAB_COLORS:
		{
			switch(ircMode)
			{
				case IRC_DISCONNECTED:
				case IRC_CONNECTING:
				case IRC_FAILEDCONNECT:
				case IRC_CLOSEDCONNECTION:
				case IRC_CLOSED:
					break;
				case IRC_CONNECTED:
				case IRC_READY:
					addDefaultButtons(l_send, ircForward, NULL, NULL);
					break;
			}
			
			addCustom(CONTROL_COLORS, COLORS_LEFT, COLORS_TOP, 200, 140, drawColors);
			setControlCallbacks(CONTROL_COLORS, handleColorsClick, NULL, NULL);
			setControlAbsoluteCoords(CONTROL_COLORS, true);
			
			break;
		}
		case IRC_TAB_NICKS:
		{
			switch(ircMode)
			{
				case IRC_DISCONNECTED:
				case IRC_CONNECTING:
				case IRC_FAILEDCONNECT:
				case IRC_CLOSEDCONNECTION:
				case IRC_CLOSED:
					break;
				case IRC_CONNECTED:
				case IRC_READY:	
					addDefaultButtons(l_append, ircForward, l_query, ircBack);
					break;
			}
			
			addListBox(LIST_LEFT + 18, LIST_TOP + 23, LIST_RIGHT, LIST_BOTTOM - 9, getIRCNickCount(), NULL, IRCListCallback, NULL);
			setControlCallbacks(CONTROL_LIST, ircForward, NULL, NULL);
			break;
		}
	}
}

void createIRC()
{
	clearControls();
	
	registerScreenUpdate(drawIRCScreen, exitIRC);
	registerHelpScreen("irc.html", true);
	setControlAlwaysDraw(CONTROL_SCREEN, true);
	
	addHotKey(0xC0, KEY_START, ircStartPressed, 0, NULL);
	addSideBar(sideTabAction);
	
	addSideTab(IRC_TAB_KEYBOARD, tab_keyboard);
	addSideTab(IRC_TAB_COLORS, tab_colors);
	addSideTab(IRC_TAB_NICKS, tab_nicks);
	
	setSideTabEnabled(IRC_TAB_NICKS, false);
	setDefaultSideTab(IRC_TAB_KEYBOARD);
	
	createIRCButtons();
	
	// for dpad
	addHotKey(0, KEY_UP, ircMoveCursor, KEY_UP, NULL);
	addHotKey(1, KEY_DOWN, ircMoveCursor, KEY_DOWN, NULL);
	addHotKey(2, KEY_LEFT, ircMoveCursor, KEY_LEFT, NULL);
	addHotKey(3, KEY_RIGHT, ircMoveCursor, KEY_RIGHT, NULL);

	setControlRepeat(0, 15, 4);
	setControlRepeat(1, 15, 4);
	setControlRepeat(2, 15, 4);
	setControlRepeat(3, 15, 4);
	
	addCustom(CONTROL_TABS, 25, 0, 230, TOPAREA - 1, drawTabs);
	setControlCallbacks(CONTROL_TABS, ircActivateTab, NULL, NULL);
	setControlAbsoluteCoords(CONTROL_TABS, true);
	
	newControl(CONTROL_IRCL, 0, 0, 24, TOPAREA - 1, CT_HITBOX, NULL);
	setControlCallbacks(CONTROL_IRCL, ircLButton, NULL, NULL);
	setControlHotkey(CONTROL_IRCL, KEY_L, 0);
	
	newControl(CONTROL_IRCR, 231, 0, 255, TOPAREA - 1, CT_HITBOX, NULL);
	setControlCallbacks(CONTROL_IRCR, ircRButton, NULL, NULL);
	setControlHotkey(CONTROL_IRCR, KEY_R, 0);
}

int scrollOffset(int tab)
{
	if(tabs[tab].screen->numPts == 0)
		return 0;
	else
		return tabs[tab].screen->crPts[tabs[tab].screen->curPt];
}

void drawTopIRCScreen()
{
	// run the send irc command if needed
	
	if(queuedEnter)
	{
		sendBuffer();
		queuedEnter = false;
	}
	
	// actions
	
	char tStr[1024];
	char cStr[1024];
	
	switch(ircMode)
	{
		case IRC_DISCONNECTED: // start connecting
			connectWifi(); // request connect
			ircMode = IRC_CONNECTING;
			createIRCButtons();
			
			sprintf(tStr, "\n%s", l_associating);
			concatCommand(cStr, makeCommand(true, false, ircColorDSO), strlen(tStr));
			
			concatBuffer(console, tStr, cStr);
			break;
		case IRC_CONNECTING: // test status
		{
			int i = Wifi_AssocStatus(); // check status
			if(i==ASSOCSTATUS_ASSOCIATED)
			{
				strcpy(tStr, l_hotspot);
				concatCommand(cStr, makeCommand(true, false, ircColorDSO), strlen(tStr));
				
				concatBuffer(console, tStr, cStr);
				ircMode = IRC_CONNECTED;
				createIRCButtons();
			}
			
			if(i==ASSOCSTATUS_CANNOTCONNECT) 
			{
				ircMode = IRC_FAILEDCONNECT;
				createIRCButtons();
			}
			
			break;
		}
		case IRC_CONNECTED:
			// have autoconnect here in the future
			
			if(autoConnect || reconnect)
			{
				connectToServer(lastServer);	
				reconnect = false;
			}
			
			ircMode = IRC_READY;
			createIRCButtons();
			
			break;
		case IRC_FAILEDCONNECT:
			sprintf(tStr, "%s\n", l_ircfailed);
			concatCommand(cStr, makeCommand(true, false, ircColorDSO), strlen(tStr));
			
			concatBuffer(console, tStr, cStr);
			
			ircMode = IRC_CLOSED;
			createIRCButtons();
			
			break;
		case IRC_CANCELCONNECTION:
			sprintf(tStr, "%s\n", l_irccancelled);
			concatCommand(cStr, makeCommand(true, false, ircColorDSO), strlen(tStr));
			
			concatBuffer(console, tStr, cStr);
			
			disconnectWifi();			
			ircMode = IRC_CLOSED;
			createIRCButtons();
			
			break;			
		case IRC_CLOSEDCONNECTION:
			closeServerConnection();
			
			destroyTabs();
			createTabs();
			
			break;
		case IRC_CLOSED:			
			if(reconnect) // reconnect to new server if needed
			{
				reconnect = false;
				char tmpServer[128];
				strcpy(tmpServer, curServer);
				
				connectToServer(tmpServer);	
				ircMode = IRC_READY;
				createIRCButtons();
			}
			
			break;
		case IRC_READY:
			// check for recieved
			
			if(connected)
			{
				if(cancelOnWait)
				{
					if(waitCount > (60 * 20)) // 20 seconds time
					{
						cancelOnWait = false;
						wifiClose(ircSocket);
						sprintf(tStr, "\n%s", l_irccannotconnect);
						concatCommand(cStr, makeCommand(true, false, ircColorDSO), strlen(tStr));
						
						concatBuffer(console, tStr, cStr);
						
						destroyTabs();
						createTabs();
						
						connected = false;
						ircMode = IRC_CLOSED;
						createIRCButtons();
						
						break;
					}
				}
				
				if(toPerform)
				{
					autoPerform();
					toPerform = false;
				}
				
				int i = recvData(ircSocket,tcpTemp,IRC_RECEIVESIZE);
				
				if(i > 0)
				{
					// mark that we recieved something
					cancelOnWait = false;
					
					do
					{
						addAndRecieve(tcpTemp, i);
						
						i = recvData(ircSocket,tcpTemp,IRC_RECEIVESIZE);
					} while(i > 0);
				}
				
				while(receiveBuffer());
				
				if(i == 0) 
				{
					// server closed connection
					wifiClose(ircSocket);
					sprintf(tStr, "\n%s", l_ircserverclosed);
					concatCommand(cStr, makeCommand(true, false, ircColorDSO), strlen(tStr));
					
					concatBuffer(console, tStr, cStr);
					
					destroyTabs();
					createTabs();
					
					connected = false;
					ircMode = IRC_CLOSED;
					createIRCButtons();
				}
				else
				{
					char tStr[128];
					
					switch(connectMode)
					{
						case C_STARTED:
						{
							PERSONAL_DATA p = *PersonalData;
							char name[20];
							
							memset(name, 0, 20);
							
							for(uint16 x=0;x<p.nameLen;x++)
								name[x] = (char)(p.name[x] & 0xFF);
							
							// fix for undernet
							char tmpNickName[NICK_LEN];							
							memcpy(tmpNickName, useNickName, NICK_LEN);
							strlwr(tmpNickName);
							
							sprintf(tStr, "USER %s localhost localhost :%s\r\n", tmpNickName, name);
							sendData(ircSocket, tStr, strlen(tStr));
							
							connectMode = C_SENTUSER;						
							break;
						}
						case C_SENTUSER:
						{
							char tNick[NICK_LEN];
							memset(tNick, 0, NICK_LEN);
							checkNickLen(useNickName, tNick);
							
							sprintf(tStr, "NICK %s\r\n", tNick);
							sendData(ircSocket, tStr, strlen(tStr));
							
							connectMode = C_SENTNICK;
							break;
						}
					}
					
					if(ircCount > 1800 && strlen(displayNick) > 0) // 30 seconds, send pong
					{
						ircCount = 0;
						sprintf(tStr, "PONG %s\r\n", curServer);
						sendData(ircSocket, tStr, strlen(tStr));
					}
				}
			}
			
			break;
	}
	
	// display
	setFont(font_gautami_10);
	setColor(ircScreenTextColor);
	
	// caption box
	fb_drawFilledRect(3, 3, 252, 17, ircScreenBorderColor, ircScreenFillColor);
	
	fb_setClipping(5, 5, 250, 15);
	
	// display here what needs to go up there
	
	if(activeTab == CONSOLE)
	{
		if(connected)
		{
			if(strlen(displayNick) == 0)
			{
				strcpy(tStr, l_notconnected);
			}
			else
			{
				sprintf(tStr, "%s", displayNick);
				
				if(strlen(myModes) != 0)
				{
					strcat(tStr, " [");
					strcat(tStr, myModes);
					strcat(tStr, "]");
				}
				
				if(strlen(curNetwork) != 0)
				{
					strcat(tStr, " ");
					strcat(tStr, l_ircon);
					strcat(tStr, " ");
					strcat(tStr, curNetwork);
				}
			}
		}
		else
		{
			strcpy(tStr, l_notconnected);
		}
	}
	else if(isChan(tabs[activeTab].name)) // channel
	{
		sprintf(tStr, "%s", tabs[activeTab].name);
		
		strcat(tStr, " [");
		sprintf(cStr, "%d", getIRCNickCount());
		strcat(tStr, cStr);
		strcat(tStr, "]");
		
		if(strlen(tabs[activeTab].screen->modes) != 0)
		{
			strcat(tStr, " [+");
			strcat(tStr, tabs[activeTab].screen->modes);
			strcat(tStr, "]");
		}
		
		if(tabs[activeTab].screen->hasLimit)
		{
			sprintf(cStr, " %s: %d", l_limit, tabs[activeTab].screen->limit);
			strcat(tStr, cStr);
		}
		
		if(tabs[activeTab].screen->hasKey)
		{
			sprintf(cStr, " %s: %s", l_key, tabs[activeTab].screen->key);
			strcat(tStr, cStr);
		}
	}
	else // pm window
	{
		sprintf(tStr, "%s %s", l_conversation, tabs[activeTab].name);	
	}
	
	fb_dispString(0, 1, tStr);
	
	// normal box
	fb_drawFilledRect(3,19,252,188,ircScreenBorderColor,ircScreenFillColor);
	
	fb_setClipping(5, 21, 250, 188);
	
	setFont(setIRCFont());
	setCallBack(ircCallback);	
	fb_dispString(0, 1, tabs[activeTab].screen->buffer + scrollOffset(activeTab));
	clearCallBack();
	
	// reset
	setBold(false);
	setUnderline(false);
	setColor(genericTextColor);
	
	tabs[activeTab].isUpdated = NOUPDATE; // make sure it doesn't hilight the tab for this screen.
}

int calcString(char *out)
{
	setFont(font_gautami_10);
	
	if(!strUpdated)
	{
		int width = 0;
		int i = textBlock;
		
		while(width < T_THRESHOLD)
		{
			width += getCharWidth(pointedText[i]);
			i++;
			
			if(pointedText[i] == 0)
				break;
		}
		
		memcpy(out, pointedText + textBlock, i - textBlock);
		out[i - textBlock] = 0;
		
		return getKBCursor() - textBlock;		
	}
	
	strUpdated = false;
	
	if(getStringWidth(pointedText, font_gautami_10) < T_THRESHOLD)
	{
		textBlock = 0;
		strcpy(out,pointedText);
		return getKBCursor();
	}
	
	if(getKBCursor() < textBlock) // we've moved back
	{
		textBlock = getKBCursor();
		
		int j = 0;
		int i = textBlock;
		int width = 0;
		
		while(width < T_THRESHOLD)
		{
			width += getCharWidth(pointedText[i]);
			
			out[j] = pointedText[i];
			
			i++;
			j++;
		}
		
		out[j] = 0;
		
		return 0; // cursor - textblock
	}
	
	int i = textBlock;
	int width = 0;
	
	while(width < T_THRESHOLD)
	{
		if(pointedText[i] == 0)
		{
			// weve reached the end and we don't span yet!
			int j = 1;
			
			while(width < T_THRESHOLD)
				width += getCharWidth(pointedText[textBlock - j]);
			
			textBlock -= j;
			
			strcpy(out, pointedText + textBlock);
			
			return getKBCursor() - textBlock; // we can return because if the cursor is not less and it took up less on the screen, it has to be on
		}
		
		width += getCharWidth(pointedText[i]);
		i++;
	}
	
	if(getKBCursor() <= i) // cursor is on the screen!
	{
		memcpy(out, pointedText + textBlock, i - textBlock);
		out[i - textBlock] = 0;
		
		return getKBCursor() - textBlock;
	}
	
	// cursor isn't on the screen
	
	textBlock++;
	
	while(true)
	{
		width = 0;
		i = textBlock;
		
		while(width < T_THRESHOLD)
		{
			width += getCharWidth(pointedText[i]);
			i++;
			
			if(pointedText[i] == 0)
				break;
		}
		
		if(getKBCursor() <= i) // found it
		{
			memcpy(out, pointedText + textBlock, i - textBlock);
			out[i - textBlock] = 0;
			
			return getKBCursor() - textBlock;
		}
		
		textBlock++;
	}
	
	return 0; // should never get here
}

void drawBottomIRCScreen()
{	
	// draw the text input buffer
	char tmpText[128];
	memset(tmpText, 0, 128);
	int tmpCur = calcString(tmpText); // figure out where to start and end and where the cursor goes
	
	bool noBlink = false;
	
	if(keysHeld() & KEY_TOUCH)
	{
		touchPosition t = touchReadXYNew();
		
		if(t.px > 13 & t.py > 19 & t.px < 241 & t.py < 33)
		{
			int tCur = getTouchCursor();
			
			if(tCur == -2)
				tmpCur = strlen(tmpText);
			else if(tCur == -1)
				tmpCur = 0;
			else
				tmpCur = tCur;
			
			moveKBCursorAbsolute(tmpCur + textBlock);
			
			noBlink = true;
		}
		
		setCallBack(cursorPosCall);
	}
	
	if(blinkOn() || noBlink)
	{
		if(isInsert())
			setCursorProperties(cursorNormalColor, -2, -3, -1);
		else
			setCursorProperties(cursorOverwriteColor, -2, -3, -1);
			
		showCursor();
		setCursorPos(tmpCur);		
	}
	
	setFakeHighlightEX(textBlock);
	bg_disableClipping();
	
	drawTextBox(13, 19, 241, 33, tmpText, false, false, false);
	
	clearCallBack();
	bg_enableClipping();
	
	clearHighlight();
	hideCursor();
	
	if(getActiveSideTab() == IRC_TAB_KEYBOARD)
	{
		char tBStr[64];
		
		switch(ircMode)
		{
			case IRC_DISCONNECTED:
			case IRC_CONNECTING:
				sprintf(tBStr,"%c\a\a%s", BUTTON_START, l_stop);
				break;
			case IRC_FAILEDCONNECT:
			case IRC_CLOSEDCONNECTION:
			case IRC_CLOSED:
				sprintf(tBStr,"%c\a\a%s", BUTTON_START, l_home);
				break;
			case IRC_CONNECTED:
			case IRC_READY:	
				if(connected)
				{
					sprintf(tBStr,"%c\a\a%s", BUTTON_START, l_disconnect);
				}
				else
				{
					sprintf(tBStr,"%c\a\a%s", BUTTON_START, l_home);
				}				
				break;
		}
		
		bg_setDefaultClipping();
		setColor(genericTextColor);			
		bg_dispString(centerOnPt((SELECT_RIGHT + CLEAR_LEFT) / 2, tBStr, font_gautami_10), 143, tBStr);
	}
}

void ircForward(int x, int y)
{	
	switch(getActiveSideTab())
	{
		case IRC_TAB_KEYBOARD:
		case IRC_TAB_COLORS:
		{
			switch(ircMode)
			{
				case IRC_DISCONNECTED:
				case IRC_CONNECTING:
					break;
				case IRC_FAILEDCONNECT:
				case IRC_CLOSEDCONNECTION:
				case IRC_CLOSED:
					// reconnect
					
					if(Wifi_AssocStatus()==ASSOCSTATUS_ASSOCIATED)
						ircMode = IRC_CONNECTED;
					else
						ircMode = IRC_DISCONNECTED;
					reconnect = true;
					
					createIRCButtons();
					
					break;
				case IRC_CONNECTED:
				case IRC_READY:
					// send
					
					sendBuffer();			
					break;
			}
			break;
		}
		case IRC_TAB_NICKS:
		{
			if(getIRCNickCount() == 0)
			{
				break;
			}
			
			// check if the window is open
			for(int i=0;i<(int)strlen(tabs[activeTab].screen->nicks[getCursor()].nick);i++)
			{
				editIRCAction(tabs[activeTab].screen->nicks[getCursor()].nick[i]);
			}
			
			break;
		}
	}
}

void destroyRecvBuffer()
{
	if(serverBuffer)
		trackFree(serverBuffer);
	serverBuffer = NULL;
}

void ircStartPressed(int x, int y)
{
	switch(ircMode)
	{
		case IRC_DISCONNECTED:
		case IRC_CONNECTING:
			// cancel connect
			
			ircMode = IRC_CANCELCONNECTION;
			createIRCButtons();
			
			break;
		case IRC_FAILEDCONNECT:
		case IRC_CLOSEDCONNECTION:
		case IRC_CLOSED:
			// home
			returnHome();
			
			break;
		case IRC_CONNECTED:
		case IRC_READY:
			// disconnect
			
			if(connected)
			{
				ircMode = IRC_CLOSEDCONNECTION;
				activeTab = 0;
				createIRCButtons();
			}
			else
			{
				returnHome();
			}
			
			break;
		case IRC_NOTHING:
		{
			if(getIRCNickCount() == 0)
			{
				break;
			}
			
			// check if the window is open
			
			if(tabFromName(tabs[activeTab].screen->nicks[getCursor()].nick) == -1)
				addTab(tabs[activeTab].screen->nicks[getCursor()].nick); // doesn't exist
			
			activeTab = tabFromName(tabs[activeTab].screen->nicks[getCursor()].nick); // set focus
			createIRCButtons();
			
			break;
		}
	}
}

void ircBack(int x, int y)
{
	switch(getActiveSideTab())
	{
		case IRC_TAB_KEYBOARD:
		{
			toggleKeyboard(0,0);
			break;
		}
		case IRC_TAB_NICKS:
		{	
			if(getIRCNickCount() == 0)
			{
				break;
			}
			
			// check if the window is open
			
			if(tabFromName(tabs[activeTab].screen->nicks[getCursor()].nick) == -1)
				addTab(tabs[activeTab].screen->nicks[getCursor()].nick); // doesn't exist
			
			activeTab = tabFromName(tabs[activeTab].screen->nicks[getCursor()].nick); // set focus
			createIRCButtons();
			
			break;
		}
	}
}

void ircLButton(int x, int y)
{
	if(activeTab > 0)
	{
		activeTab--;
		
		resetKBCursor();
		createIRCButtons();
	}
}

void ircRButton(int x, int y)
{
	if(activeTab < tabCount-1)
	{
		activeTab++;
		
		resetKBCursor();
		createIRCButtons();
	}
}

void ircUp()
{	
	switch(getActiveSideTab())
	{
		case IRC_TAB_KEYBOARD:
		case IRC_TAB_COLORS:
		{
			if(keysHeld() & KEY_X) // page up/down
			{
				if(tabs[activeTab].screen->curPt > 0)
				{
					tabs[activeTab].screen->curPt-=(ircLines - 1);
					if(tabs[activeTab].screen->curPt < 0)
						tabs[activeTab].screen->curPt = 0;
						
					tabs[activeTab].screen->autoScroll = false;
				}
				
				return;
			}
			
			if(keysHeld() & KEY_Y) // back buffer
			{
				scrollUpBuffer();
				return;
			}
			
			if(tabs[activeTab].screen->curPt > 0)
			{
				tabs[activeTab].screen->curPt--;
				tabs[activeTab].screen->autoScroll = false;
			}
		}
		case IRC_TAB_NICKS:
		{
			moveCursorRelative(CURSOR_BACKWARD);
			break;
		}
	}
}

void ircDown()
{
	switch(getActiveSideTab())
	{
		case IRC_TAB_KEYBOARD:
		case IRC_TAB_COLORS:
		{
			if(keysHeld() & KEY_X) // page up/down
			{	
				tabs[activeTab].screen->curPt+=(ircLines - 1);
				if(tabs[activeTab].screen->curPt >= tabs[activeTab].screen->maxPt)
				{
					tabs[activeTab].screen->curPt = tabs[activeTab].screen->maxPt;
					tabs[activeTab].screen->autoScroll = true;
				}
				return;
			}
			
			if(keysHeld() & KEY_Y) // back buffer
			{
				scrollDownBuffer();
				return;
			}
			
			if(tabs[activeTab].screen->curPt < tabs[activeTab].screen->maxPt)
			{
				tabs[activeTab].screen->curPt++;
				if(tabs[activeTab].screen->curPt == tabs[activeTab].screen->maxPt)
					tabs[activeTab].screen->autoScroll = true;
			}
			break;
		}
		case IRC_TAB_NICKS:
		{
			if(getCursor() < getIRCNickCount() - 1)
				moveCursorRelative(CURSOR_FORWARD);
			break;
		}
	}
}

void invalidateInput()
{
	strUpdated = true;
}

int getIRCNickCount()
{
	return tabs[activeTab].screen->nickCount;
}

void editIRCAction(char c)
{
	if(c == 0) return;
	
	if(c == RET)
	{
		if(ircMode == IRC_READY)
		{
			if(isControl())
			{	
				toggleControl();
				ignoreCommands = true;
			}
			
			queuedEnter = true;
		}
		
		setPressedChar(RET);
		return;
	}
	
	if(ircMode == IRC_CLOSED)
	{
		ircMode = IRC_READY;
		createIRCButtons();
	}
	
	setPressedChar(c);
	
	if(c == CLEAR_KEY)
	{
		memset(inputBuffer, 0, MAX_INPUT+1);
		strUpdated = true;
		resetKBCursor();
		return;
	}
	if(c == CAP)
	{
		toggleCaps();
		return;
	}
	
	genericAction(inputBuffer, MAX_INPUT - 1, c);	
	resetBackBuffers();
	strUpdated = true;
}

