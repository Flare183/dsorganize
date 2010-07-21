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
 
#ifndef _WEBBROWSER_INCLUDED
#define _WEBBROWSER_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#define WEBSTATE_INIT 0
#define WEBSTATE_CONNECTING 1
#define WEBSTATE_CONNECTED 2
#define WEBSTATE_FAILEDCONNECT 3
#define WEBSTATE_DOWNLOADING 4
#define WEBSTATE_FINISHED 5
#define WEBSTATE_FAILEDSERVER 6
#define WEBSTATE_LOADHOMEPAGE 7

#define FAKE_RET 31
#define MAX_URL 16384

#define TAB_KEYBOARD 0
#define TAB_FAVORITES 1
#define TAB_IRCURLS 2
#define TAB_SPECIALKEYS 3

#define DLQ_IMAGE 0
#define DLQ_CSS 1

#define WEB_WIDTH 238

#define MAX_EXT 64
#define MAX_SEARCH_INPUT 514 // 512 + 2 for the character and space
#define MAX_FAVORITES_INPUT 512

typedef struct
{
	u32 pos;
	u32 type;
	bool isDownloading;
} DLQUEUE_TYPE;

typedef struct
{
	u32 position;
	int content;
	char url[512];
	char file[256];
} HISTORY_TYPE;

typedef struct
{
	char triggerChar;
	char url[512];
} SEARCH_TYPE;

typedef struct
{
	int numEntries; // only the first will ever be set
	char caption[64];
	char url[512];
} FAVORITE_TYPE;

void initWebBrowser();
void initCapture();
void addURLToCapture(char *url);
void escapeIllegals(char *str);
void constructValidURL(char *tURL, char *outStr);
void setRedirect(char *redir);
void setWebState(int s);
void setPageFile(char *tSearch);
void setDownloadFlag();
char *getPageExtension();
char *getCurrentURL();

#ifdef __cplusplus
}
#endif

#endif
