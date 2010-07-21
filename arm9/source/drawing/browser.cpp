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
#include <libfb\libcommon.h>
#include <libdt\libdt.h>
#include "browser.h"
#include "address.h"
#include "pictureviewer.h"
#include "soundrecorder.h"
#include "texteditor.h"
#include "viewer.h"
#include "todo.h"
#include "soundplayer.h"
#include "database.h"
#include "fatwrapper.h"
#include "colors.h"
#include "settings.h"
#include "shortcuts.h"
#include "general.h"
#include "filerout.h"
#include "fonts.h"
#include "home.h"
#include "graphics.h"
#include "keyboard.h"
#include "globals.h"
#include "language.h"
#include "controls.h"
#include "help.h"

extern BROWSER_FILE *dirList;

static FILE_INFO curFile;
static char lText[128];
static char rText[128];	
static char fileName[256];
static char curDir[256];
static char copyFrom[256];
static char renameFrom[256];
static bool dir_create = false;
static bool copyFlag = false;
static bool cutFlag = false;
static int browserEntries = 0;
static int browserMode = 0;
static u32 bLoc[32];
static u32 bPos = 0;

extern bool showHidden;

// prototypes
void (*forwardAction)(int,int);
void renameScreenForward(int x, int y);
void drawMenu();
void renameFile();
void drawFileInfoScreen();
void createButtons();
void browserListCallback(int pos, int x, int y);
void toggleHidden(int which);
void browserForward();
void browserBack();
void deletePullUp();
void initPullUpForward(int x, int y);
void initPullUpBack(int x, int y);
void testPullUp(int px, int py);
bool isViewable(char *fName);

char *getFileName()
{
	return fileName;
}

void browserPushPos(u32 pos)
{
	if(bPos == 32)
		return;
	
	bLoc[bPos] = pos;
	
	bPos++;
}

u32 browserPopPos()
{
	if(!bPos)
		return 0;
	
	bPos--;
	
	return bLoc[bPos];
}

void browserResetPos()
{
	for(int i=0;i<32;i++)
		bLoc[i] = 0;
	
	bPos = 0;
}

void browserChangeDir(char *dir)
{
	freeDirList();
	
	strcpy(curDir, dir);	
	browserEntries = populateDirList(curDir);
	
	if(getCursor() >= browserEntries)
		moveCursorAbsolute(browserEntries - 1);
	
	setControlState(CONTROL_LIST, browserEntries);
}

bool setBrowserCursor(char *file)
{
	resetCursor();
	strlwr(file);
	
	if(browserEntries == 0)
		return false;
	
	while(getCursor() < browserEntries)
	{
		char strTmp[256];
		strcpy(strTmp, dirList[getCursor()].longName);
		strlwr(strTmp);
		
		if(strcmp(strTmp, file) == 0)
			return true;
		
		moveCursorRelative(CURSOR_FORWARD);
	}
	
	resetCursor();
	return false;
}

void browserMoveCursor(int x, int y)
{
	if(browserMode == CONFIRMDELETE)
	{
		return;
	}
	
	switch(getControlID())
	{
		case 0: // up button
			if(getCursor() > 0)
			{
				moveCursorRelative(-1);
			}
			break;
		case 1: // down button
			if(getCursor() < browserEntries - 1)
			{
				moveCursorRelative(1);
			}
			break;
		case 2: // left button
			moveCursorRelative(-8);
			break;
		case 3: // right button
			moveCursorRelative(8);
			
			if(getCursor() >= browserEntries)
			{
				moveCursorAbsolute(browserEntries - 1);
			}
			break;
	}
	
	// ensure that the button captions are properly set up
	createButtons();
}

void initBrowser()
{
	browserResetPos();
	
	browserMode = 0;	
	dir_create = false;
	copyFlag = false;
	cutFlag = false;
	
	strcpy(curDir,"/");
	
	createBrowser(true);
}

void browserPrevMode(int x, int y)
{
	if(browserMode != CONFIRMDELETE)
	{
		if(!browserMode)
		{
			browserMode = MAXBROWSERCYCLES;
		}
		--browserMode;
		
		createButtons();
	}
}

void browserNextMode(int x, int y)
{
	if(browserMode != CONFIRMDELETE)
	{
		++browserMode;
		if(browserMode == MAXBROWSERCYCLES)
		{
			browserMode = 0;
		}
		
		createButtons();
	}
}

void browserToggleHidden(int x, int y)
{
	toggleHidden(getCursor());
}

void browserAlternateLaunch(int x, int y)
{
	alternateLoadFile(getCursor());
}

void browserHotkeyForward(int x, int y)
{
	browserForward();
}

void browserHotkeyBack(int x, int y)
{
	browserBack();
}

void exitBrowser(int x, int y)
{
	freeDirList();
	destroyRandomList();
	deletePullUp();
	returnHome();
}

void createButtons()
{
	deleteControl(CONTROL_FORWARD);
	deleteControl(CONTROL_BACKWARD);
	
	switch(browserMode)
	{
		case 0:
			strcpy(rText, l_up);
			
			switch(dirList[getCursor()].fileType)
			{
				case DIRECTORY:
				case PICFILE:
				case HBFILE:
				case PKGFILE:
					strcpy(lText, l_open);
					break;
				case SOUNDFILE:
				case PLAYLISTFILE:
					strcpy(lText, l_play);
					break;
				case BINFILE:					
					if((dirList[getCursor()].fileSize == 98304) || (dirList[getCursor()].fileSize == 65536) || (dirList[getCursor()].fileSize == 131072) || (dirList[getCursor()].fileSize == 49152))
						strcpy(lText, l_open);
					else
						strcpy(lText, "N/A");
					break;
				case TXTFILE:
					if(isViewable(dirList[getCursor()].longName))
						strcpy(lText, l_view);
					else
						strcpy(lText, l_edit);
					break;		
				case NDSFILE:
				case SHORTCUTFILE:
					strcpy(lText, l_launch);
					break;
				default:
					strcpy(lText, "N/A");
					break;
			}
			
			break;
		case 1:			
			strcpy(lText, l_new);
			strcpy(rText, l_delete);
			
			break;
		case 2:
			strcpy(lText, l_rename);
			
			if(!copyFlag)
				strcpy(rText, l_copy);
			else
				strcpy(rText, l_paste);
			
			break;
		case 3:
			strcpy(lText, l_mkdir);
			
			if(!cutFlag)
				strcpy(rText, l_cut);
			else
				strcpy(rText, l_paste);
			break;
		case LOADBIN:
			if(dirList[getCursor()].fileType == BINFILE)
			{
				if((dirList[getCursor()].fileSize == 98304) || (dirList[getCursor()].fileSize == 65536) || (dirList[getCursor()].fileSize == 131072) || (dirList[getCursor()].fileSize == 49152))
					strcpy(lText, "N/A");
				else
					strcpy(lText, l_open);
			}
			else
				strcpy(lText, "N/A");
			
			strcpy(rText, l_back);
			
			break;
		case CONFIRMDELETE:
			strcpy(lText, l_confirm);
			strcpy(rText, l_back);
			
			break;
	}
	
	addDefaultButtons(lText, NULL, rText, NULL);
	setControlCallbacks(CONTROL_FORWARD, initPullUpForward, NULL, browserForward);
	setControlCallbacks(CONTROL_BACKWARD, initPullUpBack, NULL, browserBack);
}

void createBrowser(bool reload)
{
	if(reload)
	{
		browserChangeDir(curDir);
	}
	
	clearControls();
	
	registerScreenUpdate(drawFileInfoScreen, NULL);
	registerHelpScreen("browser.html", true);
	
	addHome(l_home, exitBrowser);
	addListBox(LIST_LEFT, LIST_TOP, LIST_RIGHT, LIST_BOTTOM, browserEntries, l_noentries, browserListCallback, NULL);
	setControlCallbacks(CONTROL_LIST, browserHotkeyForward, NULL, NULL);
	addDefaultLR(l_more, browserPrevMode, l_more, browserNextMode);
	
	createButtons();
	
	// for dpad
	addHotKey(0, KEY_UP, browserMoveCursor, KEY_UP, NULL);
	addHotKey(1, KEY_DOWN, browserMoveCursor, KEY_DOWN, NULL);
	addHotKey(2, KEY_LEFT, browserMoveCursor, KEY_LEFT, NULL);
	addHotKey(3, KEY_RIGHT, browserMoveCursor, KEY_RIGHT, NULL);
	
	setControlRepeat(0, 15, 4);
	setControlRepeat(1, 15, 4);
	setControlRepeat(2, 15, 4);
	setControlRepeat(3, 15, 4);
	
	// for delete and alernate launch
	addHotKey(4, KEY_X, browserToggleHidden, 0, NULL);
	addHotKey(5, KEY_Y, browserAlternateLaunch, 0, NULL);
	
	u32 aButton = KEY_A;
	u32 bButton = KEY_B;
	
	if(isABSwapped())
	{
		aButton = KEY_B;
		bButton = KEY_A;
	}
	
	// for a/b
	addHotKey(6, aButton, browserHotkeyForward, 0, NULL);
	addHotKey(7, bButton, browserHotkeyBack, 0, NULL);
}

bool isViewable(char *fName)
{
	char str[256];
	char str2[256];
	
	strcpy(str, fName);
	separateExtension(str,str2);
	
	strlwr(str2);
	
	if(strcmp(str2,".txt") == 0 || strcmp(str2,".htm") == 0 || strcmp(str2,".html") == 0 || strcmp(str2,".dss") == 0 || strcmp(str2,".pls") == 0 || strcmp(str2,".m3u") == 0)
		return true;
	
	return false;
}

void upOneDir()
{
	if(strlen(curDir) != 0 && strcmp(curDir, "/") != 0)
	{
		char str[256];
		
		strcpy(str, curDir);
		if(str[strlen(str)-1] == '/')
			str[strlen(str)-1] = 0;
		
		uint16 found = 0;
		for(uint16 findSlash=0;findSlash<strlen(str);findSlash++)
		{
			if(str[findSlash] == '/')
				found = findSlash;
		}
		
		found++;
		str[found] = 0;
		
		destroyRandomList();		
		moveCursorAbsolute(browserPopPos());		
		browserChangeDir(str);
	}
}

void loadFileWrapper(int which)
{
	if(dirList[which].fileType == SHORTCUTFILE)
	{
		SHORTCUT sc;
		
		char str[256];
		
		strcpy(str, curDir);
		
		int x = strlen(str)-1;
		
		if(str[x] == '/')
			str[x] = 0;
			
		strcat(str, "/");
		strcat(str, dirList[which].longName);
		
		if(loadShortcut(str, &sc))
			launchShortcut(&sc);		
		return;
	}
	
	loadFile(which);
}

void loadFile(int which)
{	
	// For shortcuts that require it
	moveCursorAbsolute(which);
	
	if(dirList[which].fileType == DIRECTORY)
	{
		if(wasShortcutLaunched())
		{
			resetShortcutLaunchedFlag();
			createBrowser(true);
		}
		
		if(strcmp(dirList[which].longName, ".") == 0)
			return;
			
		destroyRandomList();
		
		if(strcmp(dirList[which].longName, "..") == 0)
		{
			upOneDir();
			return;
		}
		
		char str[256];		
		strcpy(str, curDir);
		
		int x = strlen(str) - 1;
		if(str[x] != '/')
		{
			strcat(str, "/");
		}	
		
		strcat(str, dirList[which].longName);
		
		browserPushPos(getCursor());
		browserChangeDir(str);
		resetCursor();
		
		createButtons();
		
		return;
	}
	
	if(dirList[which].fileType == NDSFILE)
	{
		char str[256];
		
		strcpy(str, curDir);
		
		int x = strlen(str)-1;
		
		if(str[x] == '/')
			str[x] = 0;
			
		strcat(str, "/");
		strcat(str, dirList[which].longName);
		
		freeDirList();
		
		launchNDS(str);
		
		return;
	}
	
	if(dirList[which].fileType == TXTFILE)
	{
		strcpy(fileName, curDir);
		
		int x = strlen(fileName)-1;
		
		if(fileName[x] == '/')
			fileName[x] = 0;
			
		strcat(fileName, "/");
		strcat(fileName, dirList[which].longName);
		
		if(isViewable(fileName)) // view it, since y = edit
		{			
			freeDirList();
			
			setMode(VIEWER);
			initViewer();
		}
		else // edit it
		{			
			freeDirList();
			
			setMode(TEXTEDITOR);
			initTextEditor();
		}
		
		return;
	}
	
	if(dirList[which].fileType == PICFILE || dirList[which].fileType == BINFILE)
	{
		if(dirList[which].fileType == BINFILE)
		{
			if((dirList[which].fileSize == 98304) || (dirList[which].fileSize == 65536) || (dirList[which].fileSize == 131072) || (dirList[which].fileSize == 49152))
			{
				if(browserMode == LOADBIN)
				{
					createButtons();
					return;
				}
			}
			else
			{				
				if(browserMode != LOADBIN)
				{
					createButtons();
					return;
				}
			}
		}
		
		strcpy(fileName, curDir);
		
		int x = strlen(fileName)-1;
		
		if(fileName[x] == '/')
			fileName[x] = 0;
			
		strcat(fileName, "/");
		strcat(fileName, dirList[which].longName);
		
		if(browserMode == 0)
		{
			if(dirList[which].fileSize == 65536)
			{
				setBinName(fileName, 256, 256);
				browserMode = LOADBIN;
				
				createButtons();
				return;
			}
			if(dirList[which].fileSize == 49152)
			{
				setBinName(fileName, 256, 192);
				browserMode = LOADBIN;
				
				createButtons();
				return;
			}
		}
		
		setMode(PICTUREVIEWER);
		initPicture(false);
		
		if(browserMode == LOADBIN || dirList[which].fileType != BINFILE)
		{
			browserMode = 0;
		}
		
		return;
	}
	
	if(dirList[which].fileType == SOUNDFILE || dirList[which].fileType == PLAYLISTFILE)
	{
		char str[256];
		
		strcpy(str, curDir);
		
		int x = strlen(str)-1;
		
		if(str[x] == '/')
			str[x] = 0;
		
		strcat(str, "/");
		strcat(str, dirList[which].longName);
		
		strcpy(fileName, str);
		
		setMode(SOUNDPLAYER);
		initSound();
		
		return;
	}
	
	if(dirList[which].fileType == HBFILE)
	{
		char str[256];
		
		strcpy(str, curDir);
		
		int x = strlen(str)-1;
		
		if(str[x] == '/')
			str[x] = 0;
		
		strcat(str, "/");
		strcat(str, dirList[which].longName);
		
		char tmpStr[256];
		DRAGON_FILE *fp = DRAGON_fopen(str, "r");
		memset(tmpStr, 0, 256);
		DRAGON_fgets(tmpStr, 255, fp);
		DRAGON_fclose(fp);
		
		setMode(HOMEBREWDATABASE);
		customDB(tmpStr);
		
		return;
	}	
	
	if(dirList[which].fileType == PKGFILE)
	{
		char str[256];
		
		strcpy(str, curDir);
		
		int x = strlen(str)-1;
		
		if(str[x] == '/')
			str[x] = 0;
		
		strcat(str, "/");
		strcat(str, dirList[which].longName);
		
		DRAGON_FILE *fp = DRAGON_fopen(str, "r");
		
		char *tmpStr = (char *)safeMalloc(DRAGON_flength(fp) + 1);		
		DRAGON_fread(tmpStr, DRAGON_flength(fp), 1, fp);
		
		DRAGON_fclose(fp);
		
		setMode(HOMEBREWDATABASE);
		customPackage(tmpStr);		
		free(tmpStr);
		
		return;
	}
}

void alternateLoadFile(int which)
{
	if(dirList[which].fileType == TXTFILE || dirList[which].fileType == SHORTCUTFILE || dirList[which].fileType == PLAYLISTFILE)
	{
		strcpy(fileName, curDir);
		
		int x = strlen(fileName)-1;
		
		if(fileName[x] == '/')
			fileName[x] = 0;
			
		strcat(fileName, "/");
		strcat(fileName, dirList[which].longName);
		
		if(isViewable(fileName)) // edit it
		{
			if(dirList[which].fileType == SHORTCUTFILE) // ensure icon is redrawn after edit
			{
				strcpy(curFile.longName, "");
			}
			
			freeDirList();
			
			setMode(TEXTEDITOR);
			initTextEditor();
		}
		else
		{
			createButtons();
		}
		
		return;
	}	
	else if(dirList[which].fileType == NDSFILE)
	{
		strcpy(fileName, curDir);
		
		int x = strlen(fileName)-1;
		
		if(fileName[x] == '/')
			fileName[x] = 0;
			
		strcat(fileName, "/");
		strcat(fileName, dirList[which].longName);
		
		launchNDSAlt(fileName);
		
		return;
	}	
	else if(dirList[which].fileType == SOUNDFILE)
	{
		strcpy(fileName, curDir);
		
		int x = strlen(fileName)-1;
		
		if(fileName[x] == '/')
			fileName[x] = 0;
			
		strcat(fileName, "/");
		strcat(fileName, dirList[which].longName);
		
		char ext[256];
		separateExtension(fileName,ext);		
		strlwr(ext);
		
		if(strcmp(ext,".wav") == 0)
		{
			// Add the extension back to the end
			strcat(fileName, ext);
			
			setMode(SOUNDRECORDER);
			initSoundRecorder();
		}
		else
		{
			createButtons();
		}
		
		return;
	}
	else if(dirList[which].fileType == PKGFILE || dirList[which].fileType == HBFILE)
	{
		strcpy(fileName, curDir);
		
		int x = strlen(fileName)-1;
		
		if(fileName[x] == '/')
			fileName[x] = 0;
			
		strcat(fileName, "/");
		strcat(fileName, dirList[which].longName);
		
		freeDirList();
		
		setMode(TEXTEDITOR);
		initTextEditor();
		
		return;
	}
}

void toggleHidden(int which)
{
	if(!showHidden)
		return;
	
	char str[256];
	
	strcpy(str, curDir);
	
	int x = strlen(str)-1;
	
	if(str[x] == '/')
		str[x] = 0;
	
	strcat(str, "/");
	strcat(str, dirList[which].longName);
	
	u8 attr;
	
	if(dirList[which].hidden)
		attr = 0;
	else
		attr = ATTRIB_HID;
	
	DRAGON_SetFileAttributes(str, attr, ATTRIB_HID);
	browserChangeDir(curDir);
}

void flagPullUp(int x, int y)
{
	deleteControl(0xFF);
	deleteControl(0xF0);
	
	newControl(0xFF, 0, 0, 255, 192, CT_HITBOX, NULL);
	setControlCallbacks(0xFF, testPullUp, NULL, NULL);
	
	addCustom(0xF0, -1, -1, -1, -1, drawMenu);
	
	setControlEnabled(CONTROL_L, false);
	setControlEnabled(CONTROL_R, false);
	
	browserMode = 0;
	createButtons();
}

void deletePullUp()
{
	deleteControl(0xFF);
	deleteControl(0xF0);
	
	setControlEnabled(CONTROL_L, true);
	setControlEnabled(CONTROL_R, true);
}

void initPullUpForward(int x, int y)
{
	if(x < 0 || y < 0)
	{
		return;
	}
	
	if(browserMode < 4)
	{
		deleteControl(0xFF);
		newControl(0xFF, 10, 102, 244, 152, CT_HITBOX, NULL);
		setControlCallbacks(0xFF, NULL, flagPullUp, NULL);
	}
	else
	{
		browserForward();
	}
}

void initPullUpBack(int x, int y)
{
	if(x < 0 || y < 0)
	{
		return;
	}
	
	if(browserMode < 4)
	{
		deleteControl(0xFF);
		newControl(0xFF, 10, 102, 244, 152, CT_HITBOX, NULL);
		setControlCallbacks(0xFF, NULL, flagPullUp, NULL);
	}
	else
	{
		browserBack();
	}
}

void testPullUp(int px, int py)
{
	if(py < 102 || py > 182)
	{
		deletePullUp();
		return;
	}
	else
	{
		if(px < 10 || px > 244)
		{
			deletePullUp();
			return;
		}
		if(px > 126 && px < 130)
		{
			deletePullUp();
			return;
		}
	}
	
	if(px >= 23 && px <= 126)
	{
		if(py < 122)
			browserMode = 3;
		else if(py < 142)
			browserMode = 2;
		else if(py < 162)
			browserMode = 1;
		else if(py < 182)
			browserMode = 0;		
		
		deletePullUp();
		browserForward();
	}
	else if(px >= 130 && px <= 233)
	{
		if(py < 122)
			browserMode = 3;
		else if(py < 142)
			browserMode = 2;
		else if(py < 162)
			browserMode = 1;
		else if(py < 182)
			browserMode = 0;		
		
		deletePullUp();
		browserBack();
	}
	else
	{
		deletePullUp();
	}
}

void drawMenu()
{
	bg_drawRect(10, 102, 126, 162, widgetFillColor);	
	bg_drawRect(130, 102, 244, 162, widgetFillColor);
	
	bg_drawBox(10, 102, 126, 122, widgetBorderColor);	
	bg_drawBox(130, 102, 244, 122, widgetBorderColor);
	bg_drawBox(10, 122, 126, 142, widgetBorderColor);	
	bg_drawBox(130, 122, 244, 142, widgetBorderColor);
	bg_drawBox(10, 142, 126, 162, widgetBorderColor);	
	bg_drawBox(130, 142, 244, 162, widgetBorderColor);
	
	setFont(font_arial_9);
	setColor(widgetTextColor);
	
	bg_dispString(centerOnPt(getLLocation(), l_new, font_arial_9),145,l_new);	
	bg_dispString(centerOnPt(getRLocation(), l_delete, font_arial_9),145,l_delete);
	
	bg_dispString(centerOnPt(getLLocation(), l_rename, font_arial_9),125,l_rename);	
	if(!copyFlag)
		bg_dispString(centerOnPt(getRLocation(), l_copy, font_arial_9),125,l_copy);
	else
		bg_dispString(centerOnPt(getRLocation(), l_paste, font_arial_9),125,l_paste);
	
	bg_dispString(centerOnPt(getLLocation(), l_mkdir, font_arial_9),105,l_mkdir);	
	
	if(!cutFlag)
		bg_dispString(centerOnPt(getRLocation(), l_cut, font_arial_9),105,l_cut);
	else
		bg_dispString(centerOnPt(getRLocation(), l_paste, font_arial_9),105,l_paste);
}

void drawFileInfoScreen()
{
	setColor(genericTextColor);
	setFont(font_arial_9);	
	
	if(browserMode == LOADBIN)
	{
		fb_setClipping(0,0,255,191);
		fb_dispString(5, 175, l_choosepal);
	}
	
	if(browserEntries == 0)
		return;
	
	char str[256];
	
	if(strcmp(curFile.longName, dirList[getCursor()].longName) != 0)
	{
		getInfo(&dirList[getCursor()], curDir, &curFile);
	}	

	fb_drawFilledRect(5, 5 + 7, 5 + 32 + 1, 5 + 32 + 1 + 7, widgetBorderColor, 0xFFFF); //RGB15(25,25,25)
	fb_dispSprite(6, 6 + 7, curFile.iconData, curFile.transColor);
	if(dirList[getCursor()].fileType == NDSFILE)
	{
		// careful now, display nds data!
		
		strcpy(str, curFile.formattedName[0]);
		abbreviateString(str, 205, font_arial_9);
		fb_dispString(5 + 32 + 5 + 2, 5, str);
		
		strcpy(str, curFile.formattedName[1]);
		abbreviateString(str, 205, font_arial_9);		
		fb_dispString(5 + 32 + 5 + 2, 5 + 15, str);
		
		strcpy(str, curFile.formattedName[2]);
		abbreviateString(str, 205, font_arial_9);
		fb_dispString(5 + 32 + 5 + 2, 5 + 30, str);
	}
	else
	{
		int *pts = NULL;
		int numPts = getWrapPoints(5 + 32 + 5 + 2, 5 + 15, curFile.internalName, 0, 0, 250, 191, &pts, font_arial_9);
		free(pts);
		
		if(numPts< 3)
		{
			fb_setClipping(0,0,250,191);
			fb_dispString(5 + 32 + 5 + 2, 5 + 15, curFile.internalName);
		}
		else
		{
			fb_setClipping(0,0,250,5 + 45);
			fb_dispString(5 + 32 + 5 + 2, 5, curFile.internalName);
			fb_setClipping(0,0,250,191);
		}
	}
	
	if(dirList[getCursor()].fileType == DIRECTORY)
		sprintf(str, "%s: N/A", l_filesize);
	else
	{
		uint16 curExt = 0;    
		float fst = (float)dirList[getCursor()].fileSize;
		  
		while(fst >= 1024)
		{
			fst = fst / 1024;
			curExt++;
		}
		  
		switch(curExt)
		{
		    case 0:
                sprintf(str, "%s: %.1f b", l_filesize, fst);
				break;
		    case 1:
                sprintf(str, "%s: %.1f kb", l_filesize, fst);
				break;
		    case 2:
                sprintf(str, "%s: %.1f mb", l_filesize, fst);
				break;
		    case 3:
                sprintf(str, "%s: %.1f gb", l_filesize, fst);
				break;
		    case 4:
                sprintf(str, "%s: %.1f tb", l_filesize, fst);
				break;
		}
	}
	
	fb_dispString(5, 55, str);
	
	switch(dirList[getCursor()].fileType)
	{
		case DIRECTORY:
			sprintf(str, "%s: %s", l_filetype, l_directory);
			break;				
		case NDSFILE:
			sprintf(str, "%s: %s", l_filetype, l_ndsfile);			
			break;			
		case TXTFILE:
			sprintf(str, "%s: %s", l_filetype, l_txtfile);			
			break;			
		case VCFILE:
			sprintf(str, "%s: %s", l_filetype, l_vcffile);			
			break;			
		case DVFILE:
			sprintf(str, "%s: %s", l_filetype, l_dvpfile);			
			break;			
		case REMFILE:
			sprintf(str, "%s: %s", l_filetype, l_remfile);			
			break;
		case PICFILE:
			sprintf(str, "%s: %s", l_filetype, l_picfile);			
			break;
		case BINFILE:
			sprintf(str, "%s: %s", l_filetype, l_binfile);			
			break;
		case TODOFILE:
			sprintf(str, "%s: %s", l_filetype, l_todofile);			
			break;
		case SOUNDFILE:
			sprintf(str, "%s: %s", l_filetype, l_soundfile);			
			break;
		case PLAYLISTFILE:
			sprintf(str, "%s: %s", l_filetype, l_playlistfile);			
			break;
		case HBFILE:
			sprintf(str, "%s: %s", l_filetype, l_hbfile);			
			break;
		case PKGFILE:
			sprintf(str, "%s: %s", l_filetype, l_pkgfile);			
			break;
		case SHORTCUTFILE:
			sprintf(str, "%s: %s", l_filetype, l_shortcutfile);			
			break;
		case UNKNOWN:
			sprintf(str, "%s: %s", l_filetype, l_unknownfile);			
			break;
		default:
			sprintf(str, "%s: %s", l_filetype, l_unknownfile);
	}
	
	fb_dispString(5, 70, str);
	
	if(getMode() == BROWSER)
	{
		if(dirList[getCursor()].fileType == TXTFILE || dirList[getCursor()].fileType == SHORTCUTFILE || dirList[getCursor()].fileType == PLAYLISTFILE)
		{
			if(isViewable(dirList[getCursor()].longName))
			{
				if(showHidden)
					sprintf(str, "%c %s, %c %s", BUTTON_X, l_hide, BUTTON_Y, l_edit);
				else
					sprintf(str, "%c %s", BUTTON_Y, l_edit);
				
				fb_setDefaultClipping();
				fb_dispString(5, 175, str);
			}
			else
			{
				if(showHidden)
				{
					sprintf(str, "%c %s", BUTTON_X, l_hide);
					fb_setDefaultClipping();
					fb_dispString(5, 175, str);
				}
			}
		}
		else if(dirList[getCursor()].fileType == SOUNDFILE)
		{
			char str2[256];
			
			strcpy(str, dirList[getCursor()].longName);
			separateExtension(str,str2);
			
			strlwr(str2);
			
			if(strcmp(str2,".wav") == 0)
			{
				if(showHidden)
					sprintf(str, "%c %s, %c %s", BUTTON_X, l_hide, BUTTON_Y, l_record);
				else
					sprintf(str, "%c %s", BUTTON_Y, l_record);
				
				fb_setDefaultClipping();
				fb_dispString(5, 175, str);
			}
			else
			{
				if(showHidden)
				{
					sprintf(str, "%c %s", BUTTON_X, l_hide);
					fb_setDefaultClipping();
					fb_dispString(5, 175, str);
				}
			}
		}
		else if(dirList[getCursor()].fileType == NDSFILE)
		{
			if(showHidden)
				sprintf(str, "%c %s, %c %s", BUTTON_X, l_hide, BUTTON_Y, l_launchalt);
			else
				sprintf(str, "%c %s", BUTTON_Y, l_launchalt);
			
			fb_setDefaultClipping();
			fb_dispString(5, 175, str);
		}
		else if(dirList[getCursor()].fileType == PKGFILE || dirList[getCursor()].fileType == HBFILE)
		{
			if(showHidden)
				sprintf(str, "%c %s, %c %s", BUTTON_X, l_hide, BUTTON_Y, l_edit);
			else
				sprintf(str, "%c %s", BUTTON_Y, l_edit);
			
			fb_setDefaultClipping();
			fb_dispString(5, 175, str);
		}
		else
		{
			if(showHidden)
			{
				sprintf(str, "%c %s", BUTTON_X, l_hide);
				fb_setDefaultClipping();
				fb_dispString(5, 175, str);
			}
		}
	}
	else
	{
		fb_drawRect(5, 175, 216, 191, genericFillColor);
	}
}

void browserListCallback(int pos, int x, int y)
{
	char str[256];
	char ext[256];
	
	strcpy(str, dirList[pos].longName);
	
	separateExtension(str, ext);
	
	if(LIST_WIDTH - (19 + getStringWidth(ext, font_arial_9)) <= 10)
	{
		strcpy(str, dirList[pos].longName);
		abbreviateString(str, LIST_WIDTH - 19, font_arial_9);	
	}
	else
	{
		abbreviateString(str, LIST_WIDTH - (19 + getStringWidth(ext, font_arial_9)), font_arial_9);
		strcat(str, ext);
	}
	
	if(dirList[pos].hidden)
		setColor(browserHiddenColor);
	else
		setColor(listTextColor);
	
	bg_dispString(15, 0, str);
	
	uint16 *spriteList[] = { unknown, folder, ndsfile, txtfile, vcfile, dpfile, reminderfile, picfile, picfile, todofile, soundfile, hbfile, hbfile, playlistfile, shortcutfile };
	bg_dispSprite(x + 2, y + 2, spriteList[dirList[pos].fileType], 31775);
}

void browserSafeFileName()
{
	safeFileName(fileName);
}

void drawEditFileName()
{	
	fb_setDefaultClipping();
	setFont(font_gautami_10);
	
	fb_drawFilledRect(3, 165 + 8 - 24, 252, 165 + 23, widgetBorderColor, textEntryFillColor);	
	fb_dispString(3, 165 - 26, l_filename);
	
	setColor(textEntryTextColor);
	fb_setClipping(3, 165 + 8 - 26, 250, 165 + 26);
	
	if(blinkOn())
	{
		if(isInsert())
			setCursorProperties(cursorNormalColor, -2, -3, -1);
		else
			setCursorProperties(cursorOverwriteColor, -2, -3, -1);
			
		showCursor();
		setCursorPos(getKBCursor());		
	}
	
	setFakeHighlight();
	
	fb_dispString(3, 5, fileName);
	
	clearHighlight();
	hideCursor();	
}

void drawEditFileControls()
{
	char str[256];
	
	if(strcmp(renameFrom,"") == 0)
	{
		// new file
		if(dir_create)
		{
			sprintf(str,"%s", l_newdir);
		}
		else		
		{
			sprintf(str,"%s", l_new);
		}
	}
	else
	{
		// current file name
		sprintf(str,"%s", renameFrom);
	}
	
	setFont(font_arial_11);
	setColor(genericTextColor);
	
	abbreviateString(str, 230, font_arial_11);
	bg_dispString(13,20-2,str);
}

int getMaxCursorBrowser()
{
	return strlen(fileName);
}

void editFileNameAction(char c)
{
	if(c == 0) return;
	
	setPressedChar(c);
	
	if(c == RET)
	{
		renameScreenForward(0,0);
		return;		
	}
	
	if(c == CAP)
	{
		toggleCaps();
		return;
	}
	
	if(c == CLEAR_KEY)
	{
		memset(fileName, 0, FILENAME_SIZE);
		resetKBCursor();
		return;
	}
	
	char tmpBuffer[256];
	memset(tmpBuffer, 0, FILENAME_SIZE);
	strcpy(tmpBuffer, fileName);
	int oldCursor = getKBCursor();
	
	genericAction(tmpBuffer, FILENAME_SIZE - 1, c);
	
	int *pts = NULL;
	int numPts = getWrapPoints(3, 5, tmpBuffer, 3, 165 + 8 - 25, 242, 165 + 16, &pts, font_gautami_10);
	free(pts);
	
	if(numPts < 4)
		strcpy(fileName,tmpBuffer);
	else
		moveKBCursorAbsolute(oldCursor);
}

void renameEntry(char *oldFileName, char *newFileName)
{
	if(strlen(oldFileName) == 0)
		return;
	if(strlen(newFileName) == 0)
		return;
	
	if(strcmp(oldFileName,".") == 0)
		return;
	if(strcmp(newFileName,".") == 0)
		return;
	
	if(strcmp(oldFileName,"..") == 0)
		return;
	if(strcmp(newFileName,"..") == 0)
		return;
	
	char oldFile[256];
	char newFile[256];
	char oldF[256];
	char newF[256];
	char tmpExt[256];
	
	if(strlen(curDir) == 0) // it's root
	{
		strcpy(oldF, "/");
		strcpy(newF, "/");
	}
	else // curDir is a directory
	{
		strcpy(oldF, curDir);
		strcpy(newF, curDir);
		
		if(curDir[strlen(curDir)-1] != '/')
		{
			strcat(oldF, "/");
			strcat(newF, "/");
		}
	}
	
	// copy after directory
	strcat(oldF, oldFileName);
	strcat(newF, newFileName);
	
	// save a copy for testing same thing
	strcpy(oldFile, oldF);
	strcpy(newFile, newF);
	strlwr(oldFile);
	strlwr(newFile);

	if(strcmp(oldFile,newFile) == 0)
		return;
	
	// save a copy for bmf renaming
	strcpy(oldFile, oldF);
	strcpy(newFile, newF);
	separateExtension(oldFile, tmpExt);	
	
	// test to make sure they aren't the same thing!		
	if(strcmp(oldFileName,newFileName) == 0)
		return;
	
	DRAGON_rename(oldF, newF);
	
	strlwr(tmpExt);
	if(strcmp(tmpExt, ".bmf") != 0) // this isn't a bmf itself
	{
		separateExtension(newFile, tmpExt);
		
		strcat(oldFile, ".bmf");
		strcat(newFile, ".bmf");
		
		if(DRAGON_FileExists(oldFile) != FE_NONE)
			DRAGON_rename(oldFile, newFile);
	}
}

void browserRenameForward(int x, int y)
{
	renameFile();	
	createBrowser(true);
}

void browserRenameBack(int x, int y)
{
	createBrowser(true);
}

void renameScreenForward(int x, int y)
{
	if(strlen(fileName) == 0)
	{
		return;
	}
	
	browserSafeFileName();
	forwardAction(x,y);
}

void drawBrowserRenameScreen()
{
	drawEditFileName();
	drawEditFileControls();
}

void createRenameScreen(char *curFileName, void (*forward)(int,int), void (*back)(int,int))
{
	strcpy(renameFrom, curFileName);
	strcpy(fileName, curFileName);
	
	resetKBCursor();
	clearControls();
	
	registerScreenUpdate(drawBrowserRenameScreen, NULL);
	registerHelpScreen("renaming.html", true);
	
	// to stop renaming nothingness
	forwardAction = forward;
	
	addHome(l_swap, toggleKeyboard);
	addDefaultButtons(l_confirm, renameScreenForward, l_back, back);
	addKeyboard(0, 0, true, false, SCREEN_BG, editFileNameAction, NULL);
}

void renameFile()
{	
	if(strlen(fileName) == 0)
		return;
	
	if(strcmp(renameFrom,"") == 0)
	{
		if(strcmp(fileName,".") == 0)
		return;
		
		if(strcmp(fileName,"..") == 0)
			return;
		
		// new file
		DRAGON_chdir(curDir);
		
		if(dir_create)
			DRAGON_mkdir(fileName);
		else
			DRAGON_fclose(DRAGON_fopen(fileName, "w"));
	}
	else
	{	
		char oldFile[256];
		char newFile[256];
		
		strcpy(oldFile, renameFrom);
		strcpy(newFile, fileName);
		strlwr(oldFile);
		strlwr(newFile);

		if(strcmp(oldFile,newFile) == 0)
		{
			// Make sure we can change capitalization
			renameEntry(renameFrom, "_TMP_REN.DSO");
			renameEntry("_TMP_REN.DSO", fileName);
		}
		else
		{
			renameEntry(renameFrom, fileName);
		}
	}
}

void deleteFAT(char *entry, bool deleteFinal)
{
	DRAGON_chdir("/");
	
	if(DRAGON_FileExists(entry) == FE_NONE)
		return;
	
	if(DRAGON_FileExists(entry) == FE_FILE)
	{
		DRAGON_remove(entry);
		return;
	}
	
	// type is FE_DIR, make sure it's empty
	
	char toDelete[256];
	char done = false;
	int type;
	
	DRAGON_chdir(entry);
	type = DRAGON_FindFirstFile(toDelete);
	
	while(!done)
	{
		if(type == FE_DIR)
		{
			if(strcmp(toDelete, ".") != 0 && strcmp(toDelete, "..") != 0)
			{
				char str[256];
				
				strcpy(str, entry);
				
				int x = strlen(str)-1;
				
				if(str[x] == '/')
					str[x] = 0;
				
				strcat(str, "/");
				strcat(str, toDelete);
				
				deleteFAT(str, true);
				DRAGON_chdir(entry);
			}
		}
		else if(type == FE_FILE)
			DRAGON_remove(toDelete);
		else
			done = true;
		
		if(!done)
			type = DRAGON_FindNextFile(toDelete);
	}
	
	DRAGON_closeFind();
	
	if(deleteFinal)
	{
		DRAGON_remove(entry);
	}
}

void browserForward()
{
	deletePullUp();
	
	switch(browserMode)
	{
		case 0:	 // open
			if(browserEntries == 0)
				break;
			
			loadFileWrapper(getCursor());
			return;
		case 1:	 // new file
			createRenameScreen("", browserRenameForward, browserRenameBack);
			
			browserMode = 0;
			copyFlag = false;
			cutFlag = false;
			dir_create = false;
			return;
		case 2: // rename
			if(browserEntries == 0)
				break;
			
			if(strcmp(dirList[getCursor()].longName, ".") == 0)
				break;
			
			if(strcmp(dirList[getCursor()].longName, "..") == 0)
				break;
			
			createRenameScreen(dirList[getCursor()].longName, browserRenameForward, browserRenameBack);
			
			browserMode = 0;
			copyFlag = false;
			cutFlag = false;
			dir_create = false;
			
			return;
		case 3: // mkdir
			createRenameScreen("", browserRenameForward, browserRenameBack);
			
			browserMode = 0;
			copyFlag = false;
			dir_create = true;
			cutFlag = false;
			
			return;
		case LOADBIN: // load bin
			if(browserEntries == 0)
				break;
			if(dirList[getCursor()].fileType == BINFILE)
				loadFile(getCursor());				
			break;
		case CONFIRMDELETE: // confirm delete
			char str[256];
			
			strcpy(str, curDir);
			
			int x = strlen(str)-1;
			
			if(str[x] == '/')
				str[x] = 0;
			
			strcat(str, "/");
			strcat(str, dirList[getCursor()].longName);
			
			deleteFAT(str, true);
			if(getCursor() > 0)
				moveCursorRelative(CURSOR_BACKWARD);
			
			browserMode = 0;			
			browserChangeDir(curDir);
			
			break;			
	}
	
	createButtons();
}

void browserBack()
{
	deletePullUp();
	
	switch(browserMode)
	{
		case 0:	 // up
			upOneDir();
			break;
		case 1: // delete
			if(browserEntries == 0)
				break;
			
			if(strcmp(dirList[getCursor()].longName, ".") == 0)
				break;
			
			if(strcmp(dirList[getCursor()].longName, "..") == 0)
				break;
			
			browserMode = CONFIRMDELETE;
			break;
		case 2: // copy/paste
		{
			char str[256];
			
			if(copyFlag == false)
			{				
				if(browserEntries == 0)
					break;
				// check if its a dir, we dont support that
				if(dirList[getCursor()].fileType != DIRECTORY)
				{
					// ok we support, set the copyflag and the copy from
					strcpy(str, curDir);
					
					int x = strlen(str)-1;
					
					if(str[x] == '/')
						str[x] = 0;
						
					strcat(str, "/");
					strcat(str, dirList[getCursor()].longName);
					
					strcpy(copyFrom, str);
					browserMode = 0;
					
					copyFlag = true;
				}
			}
			else
			{
				// already copied, lets paste!
				
				char oldDir[256];
				char newDir[256];
				
				// ok first, we get the current directory				
				strcpy(str, curDir);
					
				int x = strlen(str)-1;
					
				if(str[x] == '/')
					str[x] = 0;
				
				strcat(str, "/");
				
				// change to current directory
				
				strcpy(newDir,str);
				
				// strip file out of copyFrom and place into str
				
				memset(str, 0, 256);
				
				x = 0;
				int y = 0;
				
				while(copyFrom[x] != 0)
				{
					if(copyFrom[x] == '/')
						y = x;
					
					x++;
				}
				
				
				memset(oldDir, 0, 256);
				strncpy(oldDir, copyFrom, y + 1);
				
				if(strcmp(oldDir,newDir) == 0)
				{
					// src and dest cant be the same!
					break;
				}
				
				// y holds the position of the last '/'
				
				x = y + 1;
				y = 0;
				while(copyFrom[x] != 0)
				{
					str[y] = copyFrom[x];
					x++;
					y++;
				}
				
				str[y] = 0;
				
				// str now holds the filename from copyfrom
				DRAGON_chdir(newDir);
				DRAGON_FILE *dest = DRAGON_fopen(str, "w");
				
				DRAGON_chdir("/");
				DRAGON_FILE *src = DRAGON_fopen(copyFrom, "r");
				
				uint8 *cpyBuffer = (uint8 *)safeMalloc(COPY_SIZE);
				uint16 cnt;
				
				setProgress(DRAGON_flength(src));
				
				while(!DRAGON_feof(src))
				{
					cnt = DRAGON_fread(cpyBuffer, 1, COPY_SIZE, src);
					DRAGON_fwrite(cpyBuffer, 1, cnt, dest);
					
					updateProgressRelative(cnt);
				}
				
				clearProgress();
				
				DRAGON_fclose(dest);
				DRAGON_fclose(src);
				
				copyFlag = false;
				browserMode = 0;
				
				free(cpyBuffer);
				
				browserChangeDir(curDir);
			}
			break;	
		}
		case 3: // cut/paste
		{
			char str[256];
			
			if(cutFlag == false)
			{				
				if(browserEntries == 0)
					break;
				
				// check if its a dir, we dont support that
				if(dirList[getCursor()].fileType != DIRECTORY)
				{
					// ok we support, set the cutflag and the copy from
					strcpy(str, curDir);
					
					int x = strlen(str)-1;
					
					if(str[x] == '/')
						str[x] = 0;
						
					strcat(str, "/");
					strcat(str, dirList[getCursor()].longName);
					
					strcpy(copyFrom, str);
					browserMode = 0;
					
					cutFlag = true;
				}
			}
			else
			{
				// already cut, lets paste!
				
				char oldDir[256];
				char newDir[256];
				
				// ok first, we get the current directory				
				strcpy(str, curDir);
					
				int x = strlen(str)-1;
					
				if(str[x] == '/')
					str[x] = 0;
				
				strcat(str, "/");
				strcpy(newDir,str);								
				
				memset(str, 0, 256);
				
				x = 0;
				int y = 0;
				
				while(copyFrom[x] != 0)
				{
					if(copyFrom[x] == '/')
						y = x;
					
					x++;
				}
				
				memset(oldDir, 0, 256);
				strncpy(oldDir, copyFrom, y + 1);
				
				if(strcmp(oldDir,newDir) == 0)
				{
					// src and dest cant be the same!
					break;
				}
				
				// y holds the position of the last '/'
				
				x = y + 1;
				y = 0;
				while(copyFrom[x] != 0)
				{
					str[y] = copyFrom[x];
					x++;
					y++;
				}
				
				str[y] = 0;
				
				// str now has the filename
				strcat(newDir, str);
				strcpy(oldDir, copyFrom);
				
				DRAGON_rename(oldDir, newDir);
				
				cutFlag = false;
				browserChangeDir(curDir);
				browserMode = 0;
			}
			break;	
		}
		case LOADBIN: // cancel load bin
			browserMode = 0;
			clearBinName();
			break;
		case CONFIRMDELETE: // cancel delete
			browserMode = 1;
			break;
	}
	
	createButtons();
}

int getBrowserEntries()
{
	return browserEntries;
}
