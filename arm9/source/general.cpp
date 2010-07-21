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
#include <nds\arm9\console.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <libfb/libcommon.h>
#include <libdt/libdt.h>
#include <libpicture.h>
#include "fatwrapper.h"
#include "lcdfunctions.h"
#include "colors.h"
#include "general.h"
#include "language.h"
#include "settings.h"
#include "globals.h"
#include "resources.h"
#include "keyboard.h"
#include "controls.h"
#include "graphics.h"
#include "boot.h"
#include "sound.h"
#include "soundIPC.h"
#include "nds_loader_arm9.h"
#include "autopatch.h"
#include "splash.h"
#include "fonts.h"
#include "browser.h"
#include "calendar.h"
#include "dayview.h"
#include "address.h"
#include "configuration.h"
#include "pictureviewer.h"
#include "soundrecorder.h"
#include "todo.h"
#include "help.h"
#include "scribble.h"
#include "database.h"
#include "irc.h"
#include "home.h"
#include "webbrowser.h"
#include "calculator.h"

int locations[13];
bool allowSwapBuffer = true;

static bool foundCursor = false;
static bool loading = false;
static bool haltOnOutOfMem = true;
static bool useDataDir;
static char loadText[64];
static int tCursor = 0;
static int cowAnim = 0;
static int cowCount = 0;
static int direction = 1;
static u32 globalError = 0;
static u32 webAniHeartBeat = 0;
static u32 maxSize;
static u32 curSize;

extern int normalBoot;
extern int altBoot;
extern bool disablePatching;

extern bool isQueued();

#ifdef MALLOC_TRACK
	static MALLOC_LIST mallocList[100];
	static int mallocListPtr = 0;
#endif

// for directories
char d_base[60];
char d_day[60];
char d_help[60];
char d_lang[60];
char d_reminder[60];
char d_scribble[60];
char d_todo[60];
char d_vcard[60];
char d_icons[60];
char d_res[60];
char d_cache[60];
char d_cookies[60];
char d_home[60];

bool isDataDir()
{
	return useDataDir;
}

void setGlobalError(u32 sE)
{
	globalError = sE;
}

void enableLoading(char *text)
{
	memset(loadText, 0, 64);
	strncpy(loadText, text, 63);
	loading = true;
}

void disableLoading()
{
	loading = false;
}

void setProgress(u32 max)
{
	curSize = 0;
	maxSize = max;
}

void setHTMLProgress(HTML_RENDERED *htmlPage)
{
	setProgressVars(&curSize, &maxSize, htmlPage);	
	
	curSize = 0;
	maxSize = 0;
}

void updateProgress(u32 cur)
{
	curSize = cur;
}

void updateProgressRelative(int cur)
{
	curSize += cur;
}

void clearProgress()
{
	maxSize = 0;
	curSize = 0;
}

void quickSwap(int *x, int *y)
{
	if (x != y) 
	{
		*x ^= *y;
		*y ^= *x;
		*x ^= *y;
	}
}

void findDataDirectory()
{
	useDataDir = false;
	
	DRAGON_chdir("/");
	if(DRAGON_FileExists("data") == FE_DIR) // detected homebrew data dir
	{
		DRAGON_chdir("/data/");
		
		if(DRAGON_FileExists("DSOrganize") == FE_DIR)
			useDataDir = true;
		else
			DRAGON_chdir("/");
	}
	
}

void makeDirectories() // for creating the right paths to the data dir
{
	if(useDataDir)
	{
		strcpy(d_base, "/data/DSOrganize/");
		strcpy(d_day, "/data/DSOrganize/day/");
		strcpy(d_help, "/data/DSOrganize/help/");
		strcpy(d_lang, "/data/DSOrganize/lang/");
		strcpy(d_reminder, "/data/DSOrganize/reminder/");
		strcpy(d_scribble, "/data/DSOrganize/scribble/");
		strcpy(d_todo, "/data/DSOrganize/todo/");
		strcpy(d_vcard, "/data/DSOrganize/vcard/");
		strcpy(d_icons, "/data/DSOrganize/icons/");
		strcpy(d_res, "/data/DSOrganize/resources/");
		strcpy(d_cache, "/data/DSOrganize/cache/");
		strcpy(d_cookies, "/data/DSOrganize/cookies/");
		strcpy(d_home, "/data/DSOrganize/home/");
	}
	else
	{
		strcpy(d_base, "/DSOrganize/");
		strcpy(d_day, "/DSOrganize/day/");
		strcpy(d_help, "/DSOrganize/help/");
		strcpy(d_lang, "/DSOrganize/lang/");
		strcpy(d_reminder, "/DSOrganize/reminder/");
		strcpy(d_scribble, "/DSOrganize/scribble/");
		strcpy(d_todo, "/DSOrganize/todo/");
		strcpy(d_vcard, "/DSOrganize/vcard/");
		strcpy(d_icons, "/DSOrganize/icons/");
		strcpy(d_res, "/DSOrganize/resources/");
		strcpy(d_cache, "/DSOrganize/cache/");
		strcpy(d_cookies, "/DSOrganize/cookies/");
		strcpy(d_home, "/DSOrganize/home/");
	}
}

u32 getAniHeartBeat()
{
	return webAniHeartBeat;
}

void startvBlank()
{
	if(getMode() == INITFAT || getMode() == INITPLUGIN || getMode() == DISPLAYCOW)
	{
		cowAnim++;
		
		if(cowAnim >= 15)
		{
			cowAnim = 0;
			cowCount++;
			
			if(cowCount > 3)
				cowCount = 0;
			
			drawStartSplash();
			
			if(getMode() != DISPLAYCOW)
			{
				setFont(font_arial_11);
				setColor(0xFFFF);
				setBold(true);
				
				bg_setClipping(5,5,250,181);
				bg_dispString(0,0,l_fatinit);
				
				setBold(false);
				
				setFont(font_arial_9);
				
				if(getMode() == INITFAT)
				{
					char tStr[64];
					sprintf(tStr, "Trying %s...", DRAGON_tryingInterface()); // \n\n%d, curSize);
					
					bg_dispString(0,25,tStr);
				}
				else
				{
					char tStr[64];
					sprintf(tStr, "Initialized %s", DRAGON_tryingInterface()); // \n\n%d, curSize);
					
					bg_dispString(0,25,tStr);
					
					bg_dispString(0,40,"Initializing settings...");
					
					bg_drawRect(10, 65, 15, 70, 0xFFFF);
				}
				
				bg_swapBuffers();
			}
			
			uint16 *fourCows[] = { s_cow1, s_cow2, s_cow3, s_cow4 };
			fb_dispSprite(112, 154, fourCows[cowCount], RGB15(0,0,31));
			fb_swapBuffers();
		}
	}
}

void vBlank() // in case i ever need it... oops apparently I did.
{
	rand(); // make sure we get real random stuff
	
	webAniHeartBeat++;
	
	incrimentBCount();
	incrimentIRCCount();
	
	updateControlRepeats();
	updatePictureAnimations();
	updateRecordingCount();
	
	if(loading)
	{
		setFont(font_gautami_10);
		setColor(genericTextColor);
		fb_dispString(5, 5, loadText);
		
		if(curSize == 0)
		{
			direction = 1;
		}
		else if(curSize >= (u32)((getScreenWidth() - 24) >> 2))
		{
			direction = -1;
		}
		
		fb_drawFilledRect(5, getScreenHeight()-16, getScreenWidth()-6, getScreenHeight()-6, loadingBorderColor, loadingFillColor);
		fb_drawRect(6 + (curSize << 2), getScreenHeight()-15, 17 + (curSize << 2), getScreenHeight()-7, loadingBarColor);
		
		curSize += direction;
		
		fb_swapBuffers();
	}
	else if(maxSize > 0)
	{
		// we gotta display the bar graph of how far we are
		double percent = (double)curSize / (double)maxSize;
		int pixel = (int)((255 - 12) * percent);
		
		fb_drawFilledRect(5, 191-15, 255-5, 191-5, loadingBorderColor, loadingFillColor);
		fb_drawRect(6, 191-14, 6 + pixel, 191-6, loadingBarColor);
		
		fb_swapBuffers();
	}
	else if(isWorking())
	{
		drawAnswerScreen();
		fb_swapBuffers();
	}
}

void disableHalt()
{
	haltOnOutOfMem = false;
}

touchPosition touchReadXYNew() 
{
	touchPosition touchPos;

	touchPos.x = IPC->touchX;
	touchPos.y = IPC->touchY;

	touchPos.px = IPC->touchXpx;
	touchPos.py = IPC->touchYpx;
	
	return touchPos;
}

void *safeMalloc(size_t size)
{
	void *tmp = malloc(size);
	
	if(!tmp)
	{
		if(!haltOnOutOfMem)
		{
			haltOnOutOfMem = true;
			return NULL;
		}
		
		char err_str[512];		
		sprintf(err_str, "%s\n\nError %X", l_fatalerror, globalError);		
		
		fatalHalt(err_str);
	}
	
	memset(tmp, 0, size);
	
	// make sure next time will halt
	haltOnOutOfMem = true;
	
	return tmp;
}

void safeFree(void *p)
{
	if(!p)
	{
		char err_str[512];
		sprintf(err_str, "%s\n\nError %X", l_fatalfree, globalError);
		
		fatalHalt(err_str);
	}
	
	free(p);
}

bool isGBA(char *fName)
{
	char str[256];
	char ext[256];
	
	strcpy(str, fName);
	separateExtension(str, ext);
	separateExtension(str, ext);
	
	strlwr(ext);
	
	if(strcmp(ext, ".sc") == 0 || strcmp(ext, ".ds") == 0)
	{
		return true;
	}
	
	return false;
}

void launchNDSMethod1(char *file)
{	
	vramSetBankD(VRAM_D_LCD);
	REG_IE = 0;
	REG_IME = 0;
	REG_IF = 0xFFFF;	// Acknowledge interrupt
	
	bool doGBABoot = isGBA(file);
	
	char shortFile[256];
	char directory[256];
	
	strcpy(directory,file);
	
	char *whereSlash = directory;
	
	while(strchr(whereSlash, '/') != NULL)
		whereSlash = strchr(whereSlash, '/') + 1;
	
	whereSlash[0] = 0;	
	
	DRAGON_chdir("/");
	DRAGON_FILE *fp = DRAGON_fopen(file, "r");
	
	DRAGON_GetAlias(shortFile);
	DRAGON_fclose(fp);
	
	strcat(directory, shortFile);
	SendArm7Command(ARM7COMMAND_BOOT_MIGHTYMAX, 0);
	exec(directory, doGBABoot, false);
}

void launchNDSMethod2(char *file)
{	
	vramSetBankD(VRAM_D_LCD);
	REG_IE = 0;
	REG_IME = 0;
	REG_IF = 0xFFFF;	// Acknowledge interrupt
	
	// get cluster
	
	u32 fCluster;
	
	DRAGON_FILE *fFile = DRAGON_fopen(file, "r");
	fCluster = fFile->firstCluster;
	DRAGON_fclose(fFile);
	
	// get loader
	
	DRAGON_chdir("/");
	char cCommand[256];
	sprintf(cCommand, "%sload.bin", d_res);
	DRAGON_FILE *stub = DRAGON_fopen(cCommand,"r");
	u32 tLen = DRAGON_flength(stub);
	
	char *buffer = (char *)safeMalloc(tLen);
	
	DRAGON_fread(buffer,tLen,1,stub);
	DRAGON_fclose(stub);
	
	fb_setClipping(5,5,250,187);
	
	setColor(genericTextColor);			
	setFont(font_arial_11);
	fb_dispString(0,0, l_launchingchishm);
	fb_swapBuffers();
	
	runNds(buffer, tLen, fCluster, false);

	while(1);
}

void launchNDS(char *file)
{
	if(!disablePatching)
		patchFile(file);
	
	switch(normalBoot)
	{
		case BOOT_MIGHTYMAX:
			launchNDSMethod1(file);
			break;
		case BOOT_CHISHM:
			launchNDSMethod2(file);
			break;
	}
}

void launchNDSAlt(char *file)
{
	if(!disablePatching)
		patchFile(file);
	
	switch(altBoot)
	{
		case BOOT_MIGHTYMAX:
			launchNDSMethod1(file);
			break;
		case BOOT_CHISHM:
			launchNDSMethod2(file);
			break;
	}
}

int centerOnPt(int pt, char *str, uint16 **FP)
{
	return pt - (getStringWidth(str, FP)/2);
}

int format12(int x)
{
	if( x > 12 )
		x -= 12;
	
	if( x == 0 )
		x = 12;
		
	return x;
}

char properCaps(char c)
{
	if(c >= 'A' && c <= 'Z' && !isCaps())
		return c - ('A' - 'a');
	return c;
}

void abbreviateString(char *str, int maxLen, uint16 **FP)
{
	if(getStringWidth(str, FP) <= maxLen)
		return;
		
	strcat(str,"...");
	while(getStringWidth(str, FP) > maxLen)
	{
		str[strlen(str)-4] = '.';
		str[strlen(str)-1] = 0;
	}		
}

void separateExtension(char *str, char *ext)
{
	int x = 0;
	
	for(int y = strlen(str) - 1; y > 0; y--)
	{
		if(str[y] == '.')
		{
			// found last dot
			x = y;
			break;
		}
		if(str[y] == '/')
		{
			// found a slash before a dot, no ext
			ext[0] = 0;
			return;
		}
	}
	
	if(x > 0)
	{
		int y = 0;
		while(str[x] != 0)
		{
			ext[y] = str[x];
			str[x] = 0;
			x++;
			y++;
		}
		ext[y] = 0;
	}
	else
		ext[0] = 0;	
}

bool checkHelp()
{
	#ifndef MALLOC_TRACK
	#ifndef SCREENSHOT_MODE
	
	#ifdef DUMP_ARM7
	if(keysDown() & KEY_SELECT)
	{
		scanKeys();
		
		soundIPC->interlaced = (s16 *)trackMalloc(64*1024, "dump");
		SendArm7Command(ARM7COMMAND_DUMP, 0);
	}
	#else
	if(((keysDown() & KEY_SELECT) || (keysHeld() & KEY_SELECT)) && getMode() != ABOUT && !isQueued())
	{
		scanKeys();
		
		if(isHelpPage())
		{
			drawHelpScreen();
			
			vblankWait();
			if(allowSwapBuffer)
				fb_swapBuffers();
			bg_swapBuffers();
			
			return true;
		}
	}
	#endif
	#endif
	#else
	if(keysDown() & KEY_SELECT)
	{
		scanKeys();
		printMallocList();
	}
	#endif
	
	return false;
}

void debugPrint(char *str)
{
	// To ensure it comes out on any debugging screens i have up
	dprintf(str);
	
	fb_setDefaultClipping();
	fb_dispString(3,3,str);
	
	vblankWait();	
	fb_swapBuffers();
}

void debugPrintHalt(char *str)
{
	irqSet(IRQ_VBLANK, 0);
	fb_swapBuffers();
	
	fb_setBGColor(0xFFFF);
	setColor(0);
	
	debugPrint(str);
	
	while(1)
	{
		vblankWait();
		scanKeys();
		
		if(keysDown() & KEY_SELECT)
		{
			while(keysHeld() & KEY_SELECT)
				scanKeys();
			
			irqSet(IRQ_VBLANK, vBlank);
			return;
		}
	}
	
}

void fatalHalt(char *str)
{	
	debugPrintHalt(str);
	
	while(1) 
	{ 
		// empty, halting processor
	}
}

void *trackMalloc(u32 length, char *desc)
{
	void *tmp = safeMalloc(length);
	
	#ifdef MALLOC_TRACK
	
	// add here	
	if(mallocListPtr < 100)
	{	
		mallocList[mallocListPtr].memLocation = (u32)tmp;
		mallocList[mallocListPtr].size = (u32)length;
		strcpy(mallocList[mallocListPtr].description, desc);
		
		mallocListPtr++;	
	}
	
	// return normal
	#endif
	
	return tmp;
}

void trackFree(void *tmp)
{
	#ifdef MALLOC_TRACK
	
	// subtract here	
	for(int i=0;i<mallocListPtr;i++)
	{
		if(mallocList[i].memLocation == (u32)tmp)
		{
			for(int j=i;j<mallocListPtr - 1;j++)
			{
				mallocList[j].memLocation = mallocList[j+1].memLocation;
				mallocList[j].size = mallocList[j+1].size;
				strcpy(mallocList[j].description, mallocList[j+1].description);
			}
			
			mallocListPtr--;
			break;
		}
	}
	
	// do normal
	#endif
	
	safeFree(tmp);
}

void *trackRealloc(void *tmp, u32 length)
{
	#ifdef MALLOC_TRACK
	u32 old = (u32)tmp;
	#endif
	
	void *newTmp = realloc(tmp,length);
	
	if(!newTmp)
	{
		debugPrintHalt(l_fatalerror);
		
		while(1) 
		{ 
			// empty, halting processor
		}
	}
	
	#ifdef MALLOC_TRACK
	for(int i=0;i<mallocListPtr;i++)
	{
		if(mallocList[i].memLocation == old)
		{
			if(newTmp)
			{
				mallocList[i].size = (u32)length;
				mallocList[i].memLocation = (u32)newTmp;
				break;
			}
			else
			{
				// handle realloc of null
				for(int j=i;j<mallocListPtr;j++)
				{
					mallocList[j].memLocation = mallocList[j+1].memLocation;
					mallocList[j].size = mallocList[j+1].size;
					strcpy(mallocList[j].description, mallocList[j+1].description);
				}
				
				mallocListPtr--;
				break;
			}
		}
	}
	#endif
	
	return newTmp;
}

#ifdef MALLOC_TRACK
void printMallocList()
{
	fb_swapBuffers();
	
	setFont(font_gautami_10);
	setColor(0);
	
	if(mallocListPtr > 0)
	{
		for(int i=0;i<mallocListPtr;i++)
		{
			fb_dispString((i%3) * 86,(i/3)*11, mallocList[i].description);
			
			//char str[30];
			//sprintf(str, "%d", mallocList[i].size);
			
			//fb_dispString((i%3) * 86,(i/3)*22+11, str);
		}
	}
	else
		fb_dispString(0,0,"Nothing in malloc list");
	
	vblankWait();
	fb_swapBuffers();
	
	while(keysHeld())
		scanKeys();	
}
#endif

void defaultLocations()
{
	locations[0] = 0;
	locations[1] = 1;
	locations[2] = 2;
	locations[3] = 3;
	locations[4] = 4;
	locations[5] = 5;
	locations[6] = 6;
	locations[7] = 7;
	locations[8] = 8;
	locations[9] = 9;
	locations[10] = 10;
	locations[11] = 11;
	locations[12] = 12;
}

void gotoApplication(int app, bool transPos)
{
	int whichApp;

	if(transPos)
	{
		whichApp = locations[app];
	}
	else
	{
		whichApp = app;
	}
	
	// Ensure that they all start with fresh cursor
	resetCursor();
	
	switch(whichApp)
	{
		case 0:			
			setMode(CALENDAR);
			initCalendar();
			break;
		case 1:
			setMode(DAYVIEW);
			initDayView();
			break;
		case 2:
			setMode(ADDRESS);
			initAddress();
			break;
		case 3:
			setMode(TODOLIST);
			initTodo();
			break;
		case 4:
			setMode(SCRIBBLEPAD);
			initScribble();
			break;
		case 5:
			setMode(BROWSER);
			initBrowser();
			break;
		case 6:
			setMode(CALCULATOR);
			initCalc();
			break;
		case 7:
			setMode(IRC);
			initIRC();
			break;
		case 8:
			setMode(WEBBROWSER);
			initWebBrowser();
			break;
		case 9:
			setMode(HOMEBREWDATABASE);
			initDatabase();
			break;
		case 10:
		case 11:
			break;
		case GO_CONFIG:
			setMode(CONFIGURATION);
			initConfiguration();
			break;
	}
}

void returnHome()
{	
	setMode(HOME);
	initHome();
}

bool isNumerical(char c)
{
	if(c < '0')
		return false;
	if(c > '9')
		return false;
	
	return true;
}

void writeDebug(const char *s, ...)
{
	bool shouldFree = true;
	char *temp = (char *)malloc(8192);
	
	if(!temp)
	{
		shouldFree = false;
		temp = "Out of memory in writeDebug";
	}
	else
	{
		va_list marker;
		va_start(marker,s);
		vsprintf(temp,s,marker);
		va_end(marker);
	}
	
	DRAGON_preserveVars();
	DRAGON_chdir("/");
	DRAGON_FILE *fp = DRAGON_fopen("debug.txt", "a");
	DRAGON_fputs(temp, fp);
	DRAGON_fputc('\r', fp);
	DRAGON_fputc('\n', fp);
	DRAGON_fclose(fp);
	DRAGON_restoreVars();
	
	if(shouldFree)
	{
		safeFree(temp);
	}
}

#ifdef DEBUG_MODE
void debugInit()
{
	videoSetMode(MODE_0_2D | DISPLAY_BG0_ACTIVE);
	vramSetBankA(VRAM_A_MAIN_BG);
	vramSetBankB(VRAM_B_MAIN_BG);

	BG0_CR = BG_MAP_BASE(8);
	BG_PALETTE[255] = RGB15(31,31,31);	//by default font will be rendered with color 255

	consoleInitDefault((u16*)SCREEN_BASE_BLOCK(8), (u16*)CHAR_BASE_BLOCK(0), 16);
}
#else
void dprintf(const char *d1, ...)
{
	// dummy function to call instead of iprintf
}
#endif

// format of %sx, %dx, %cx where x is the variable number
// %c0 for control reset
// maximum of 9 ordered arguments, 1-9
void osprintf(char *out, const char* fmt, ...)
{
	void *vaArray[11];
	int numArgs = 0;
	int pos = 0;
	void *args;
	va_list marker;
	
	va_start(marker,fmt);

	args = va_arg(marker, void *);
	while(numArgs < 10)
	{
		vaArray[numArgs] = args;
		args = va_arg(marker, void *);		
		
		numArgs++;
	}
	
	va_end(marker);
	
	char c = 'A'; // A is for Apple!
	
	while(c != 0)
	{
		c = *fmt;
		
		fmt++;
		
		if(c == 0)
		{
			out[pos] = 0;
			break;
		}
		
		if(c == '%')
		{
			int i = 0;
			switch(*fmt)
			{
				case 'c':
				case 'C':
					fmt++;
					if(*fmt == '0')
					{
						out[pos] = CONTROL_RESET;
						pos++;
					}
					else
					{
						i = (*fmt) - '1';			
						if((u32)vaArray[i] != 0)
						{
							out[pos] = (u32)vaArray[i];
							pos++;
						}
					}
					
					break;
				case 'd':
				case 'D':
				{
					fmt++;
					i = (*fmt) - '1';
					
					char strTmp[18];
					sprintf(strTmp, "%d", (u32)vaArray[i]);
					
					char *strPtr = strTmp;
					
					while(*strPtr != 0)
					{
						out[pos] = *strPtr;
						pos++;
						strPtr++;
					}							
					
					break;
				}
				case 's':
				case 'S':
				{							
					fmt++;
					i = (*fmt) - '1';
					
					char *strPtr = (char *)vaArray[i];
					
					while(*strPtr != 0)
					{								
						out[pos] = *strPtr;
						pos++;
						strPtr++;
					}				
					
					break;
				}
				case 'h':
				{
					bool milT = false;
					
					fmt++;
					switch(*fmt)
					{
						case 'm':
						case 'M':
							milT = true;
					}
					
					char strTmp[3];
					sprintf(strTmp, "%d", getHour(milT));
					
					char *strPtr = strTmp;
					
					while(*strPtr != 0)
					{
						out[pos] = *strPtr;
						pos++;
						strPtr++;
					}
					
					break;
				}
				case 'H':
				{
					bool milT = false;
					
					fmt++;
					switch(*fmt)
					{
						case 'm':
						case 'M':
							milT = true;
					}
					
					char strTmp[3];
					sprintf(strTmp, "%02d", getHour(milT));
					
					char *strPtr = strTmp;
					
					while(*strPtr != 0)
					{
						out[pos] = *strPtr;
						pos++;
						strPtr++;
					}
					
					break;
				}
				case 'm':
				{
					char strTmp[3];
					sprintf(strTmp, "%d", getMinute());
					
					char *strPtr = strTmp;
					
					while(*strPtr != 0)
					{
						out[pos] = *strPtr;
						pos++;
						strPtr++;
					}
					
					break;
				}
				case 'M':
				{
					char strTmp[3];
					sprintf(strTmp, "%02d", getMinute());
					
					char *strPtr = strTmp;
					
					while(*strPtr != 0)
					{
						out[pos] = *strPtr;
						pos++;
						strPtr++;
					}
					
					break;
				}
				case 't':
				{
					char strTmp[3];
					sprintf(strTmp, "%d", getSecond());
					
					char *strPtr = strTmp;
					
					while(*strPtr != 0)
					{
						out[pos] = *strPtr;
						pos++;
						strPtr++;
					}
					
					break;
				}
				case 'T':
				{
					char strTmp[3];
					sprintf(strTmp, "%02d", getSecond());
					
					char *strPtr = strTmp;
					
					while(*strPtr != 0)
					{
						out[pos] = *strPtr;
						pos++;
						strPtr++;
					}
					
					break;
				}								
				case 'a':				
				case 'A':
				{
					char *strPtr = l_ampm[isPM()];
					
					while(*strPtr != 0)
					{
						out[pos] = *strPtr;
						pos++;
						strPtr++;
					}
					
					break;
				}							
				case 'k':				
				case 'K':
				{
					out[pos] = CONTROL_COLOR;
					pos++;
					
					break;
				}						
				case 'b':				
				case 'B':
				{
					out[pos] = CONTROL_BOLD;
					pos++;
					
					break;
				}				
				case 'u':				
				case 'U':
				{
					out[pos] = CONTROL_UNDERLINE;
					pos++;
					
					break;
				}
			}
			
			fmt++;
		}
		else if(c == '\\')
		{
			switch(*fmt)
			{
				case '\\':
					out[pos] = '\\';
					pos++;
					break;
				case 't':
					out[pos] = '\t';
					pos++;
					break;
				case 'n':
					out[pos] = '\n';
					pos++;
					break;
				case '%':
					out[pos] = '\%';
					pos++;
					break;
			}
			
			fmt++;
		}
		else
		{
			out[pos] = c;
			pos++;
		}
	}
}

int cursorPosCall(int pos, u32 c, int pass, int xPos, int yPos)
{
	if(!(keysHeld() & KEY_TOUCH))
		return 0;
	
	if(pass == 1)
		return 0;
	if(!pos)
	{
		foundCursor = false;
		tCursor = -1;
	}
	
	if(foundCursor)
		return 0;
		
	touchPosition t = touchReadXYNew();	
	int tWidth = (getCharWidth(c) >> 1)+1;
	
	if((t.px >= (xPos - tWidth)) && (t.px <= (xPos + tWidth)))
	{
		tCursor = pos;
		foundCursor = true;
	}
	
	return 0;
}

int getTouchCursor()
{
	int tRet = -2;
	
	if(foundCursor)
		tRet = tCursor;
	
	tCursor = -1;
	foundCursor = false;
	
	return tRet;
}

// Not written by me.  Can you tell by the proper use of comments?
char *cistrstr(char *haystack, char *needle)
{
	if ( !*needle )
	{
		return haystack;
	}
	
	for(;*haystack;++haystack)
	{
		if(toupper(*haystack) == toupper(*needle))
		{
			/* Matched starting char -- loop through remaining chars. */
			const char *h, *n;
			
			for(h = haystack, n = needle;*h && *n;++h, ++n)
			{
				if(toupper(*h) != toupper(*n))
				{
					break;
				}
			}
			
			if(!*n) /* matched all of 'needle' to null termination */
			{
				return haystack; /* return the start of the match */
			}
		}
	}
	
	return 0;
}

void strccat(char *str, char c)
{
	char tStr[2];
	
	tStr[0] = c;
	tStr[1] = 0;
	
	strcat(str, tStr);
}

#ifdef SCREENSHOT_MODE
void takeScreenshot()
{
	if(keysDown() & KEY_SELECT)
	{
		static PICTURE_DATA ssPicture;
		
		initImage(&ssPicture, 0, 0);
		createImage(&ssPicture, 256, 192 * 2, 0xFFFF);
		sprintf(ssPicture.fileName, "screenshot%d%d%d%d%d%d.png", getDay(), getMonth(), getYear(), getHour(true), getMinute(), getSecond());
		
		if(screensFlipped() || configFlipped())
		{
			memcpy((uint16 *)ssPicture.rawData, fb_backBuffer(), 256*192*2);
			memcpy((uint16 *)ssPicture.rawData + 256*192, bg_backBuffer(), 256*192*2);
		}
		else
		{
			memcpy((uint16 *)ssPicture.rawData, fb_backBuffer(), 256*192*2);
			memcpy((uint16 *)ssPicture.rawData + 256*192, bg_backBuffer(), 256*192*2);
		}
		
		DRAGON_chdir("/");
		saveImageFromHandle(&ssPicture);				
		freeImage(&ssPicture);
	}
}
#endif

bool areEqual(char *domain1, char *domain2, int length)
{
	// Bytes tested = nothing
	int count = 0;
	
	// Loop while they equal
	while(makeLower(*domain1) == makeLower(*domain2))
	{
		// Keep track of bytes tested
		count++;
		
		// We hit the end on both, they are the same
		if(*domain1 == 0 || count == length)
		{
			return true;
		}
		
		domain1++;
		domain2++;
	}
	
	// They didn't equal somewhere
	return false;
}

char makeLower(char c)
{
	if(c >= 'A' && c <= 'Z')
	{
		c += 'a' - 'A';
	}

	return c;
}
