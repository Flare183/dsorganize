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
 
#ifndef _GENERAL_INCLUDED
#define _GENERAL_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

// sound stuff

#define SND_GOFORWARD 0
#define SND_GOBACK 1
#define SND_GETPREV 2
#define SND_GETNEXT 3
#define SND_TOGGLEHOLD 5
#define SND_MODE 6
#define SND_VOLUMEUP 7
#define SND_VOLUMEDN 8
#define SND_PREVINTERNAL 9
#define SND_NEXTINTERNAL 10
#define SND_SEEK 11

// going to screens
#define GO_CONFIG 12

#define COMPARE_INFINITE -1

//#define SCREENSHOT_MODE
//#define MALLOC_TRACK
//#define DEBUG_MODE
//#define PROFILING

#define vblankWait() { while(DISP_Y != 192); while(DISP_Y == 192); }

#include "html.h"

typedef struct
{
	u32 memLocation;
	u32 size;
	char description[32];
} MALLOC_LIST;

void enableLoading(char *text);
void disableLoading();
void enableWorking();
void disableWorking();

void setProgress(u32 max);
void setHTMLProgress(HTML_RENDERED *htmlPage);
void updateProgress(u32 cur);
void updateProgressRelative(int cur);
void clearProgress();

void startvBlank();
void vBlank();

void makeDirectories();
void launchNDS(char *file);
void launchNDSAlt(char *file);
int centerOnPt(int pt, char *str, uint16 **FP);
int format12(int x);
char properCaps(char c);
void abbreviateString(char *str, int maxLen, uint16 **FP);
void separateExtension(char *str, char *ext);
bool checkHelp();
void returnHome();
void quickSwap(int *x, int *y);
void gotoApplication(int app, bool transPos);
void defaultLocations();
bool isDataDir();
bool isNumerical(char c);
void osprintf(char *out, const char* fmt, ...);
int cursorPosCall(int pos, u32 c, int pass, int xPos, int yPos);
int getTouchCursor();
char *cistrstr(char *haystack, char *needle);
u32 getAniHeartBeat();
void disableHalt();
void takeScreenshot();
int getRepCount();
void setRepCount(int newVal);
void findDataDirectory();
void strccat(char *str, char c);
bool areEqual(char *domain1, char *domain2, int length);
char makeLower(char c);

// fix for games & music
touchPosition touchReadXYNew();

// debug only
void setGlobalError(u32 sE);
void debugPrint(char *str);
void debugPrintHalt(char *str);
void writeDebug(const char *s, ...);
void fatalHalt(char *str);
void debugInit();

#define assert(x,y) { \
	if(x) { \
		writeDebug(y); \
	} \
}

#ifdef DEBUG_MODE
#define dprintf iprintf
#else
void dprintf(const char *d1, ...);
#endif
	
void *trackMalloc(u32 length, char *desc);
void trackFree(void *tmp);
void *trackRealloc(void *tmp, u32 length);

void *safeMalloc(size_t size);
void safeFree(void *p);

#ifdef MALLOC_TRACK
void printMallocList();
#endif

// in arm9.cpp
void checkKeys();
void touchDown(int px, int py);
void touchUp();

#ifdef __cplusplus
}
#endif

#endif
