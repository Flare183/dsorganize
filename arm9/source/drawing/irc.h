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
 
#ifndef _IRC_INCLUDED
#define _IRC_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#define IRC_DISCONNECTED 0
#define IRC_CONNECTING 1
#define IRC_CONNECTED 2
#define IRC_FAILEDCONNECT 3
#define IRC_READY 4
#define IRC_CLOSED 5
#define IRC_CLOSEDCONNECTION 6
#define IRC_CANCELCONNECTION 7
#define IRC_NOTHING 999

#define IRC_TIMEOUT 180
#define IRC_RECEIVESIZE 8192

#define NICK_LEN 64
#define MAX_INPUT 512
#define SB_SIZE 10240
#define T_THRESHOLD 218
#define MAX_BUFFER_LINES 100
#define MAX_URL_IRC 512
#define MAX_REASON 1024

#define NOUPDATE 0
#define UNIMPORTANT 1
#define IMPORTANT 2

#define C_STARTED 0
#define C_SENTUSER 1
#define C_SENTNICK 2
#define C_SENTALTNICK 3
#define C_NICKCHANGE 4

// parsing
#define CONTROL_CTCP 1
#define CONTROL_BOLD 2
#define CONTROL_COLOR 3
#define CONTROL_UNDERLINE 31
#define CONTROL_RESET 15 // to match pirch and other clients

// tabs
#define IRC_TAB_KEYBOARD 100
#define IRC_TAB_COLORS 101
#define IRC_TAB_NICKS 102

// colors
#define COLORS_LEFT 24
#define COLORS_TOP 37

#define CONSOLE 0

#define CONTROL_COLORS 0xD0
#define CONTROL_TABS 0xD1
#define CONTROL_IRCL 0xD2
#define CONTROL_IRCR 0xD3

typedef struct
{
	char nick[NICK_LEN];
	uint16 modes;
} NICK_LIST;

typedef struct
{
	// this is the chat buffer
	char *buffer;
	char *colorBuffer;	
	u32 length;
	
	// this is for the list of nicks
	NICK_LIST *nicks;
	u32 nickCount;
	
	char modes[20]; // channel modes	
	char key[60];
	u32 limit;
	
	// this is for wordwrap
	int *crPts;
	int numPts;
	int curPt;
	int maxPt;
	
	// these are the physical coords of the window
	int x;
	int y;
	int w;
	int h;
	
	bool autoScroll;
	bool hasKey;
	bool hasLimit;
} SCREEN_BUFFER;

typedef struct
{
	char name[64];
	int isUpdated;
	SCREEN_BUFFER *screen;
} TAB_ARRAY;

void initIRC();
void invalidateInput();
int calcString(char *out);
int inputLength();
void incrimentIRCCount();

#ifdef __cplusplus
}
#endif

#endif
