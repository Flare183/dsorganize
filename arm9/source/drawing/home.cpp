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
#include "home.h"
#include "calendar.h"
#include "dayview.h"
#include "todo.h"
#include "about.h"
#include "colors.h"
#include "shortcuts.h"
#include "settings.h"
#include "general.h"
#include "globals.h"
#include "fatwrapper.h"
#include "fonts.h"
#include "graphics.h"
#include "language.h"
#include "keyboard.h"
#include "controls.h"
#include "help.h"

extern bool milTime;
extern char lLanguage[LANG_SIZE];
extern int locations[13];

static char *strings[MAX_MODES];
static uint16 *sprites[MAX_MODES];
static SHORTCUT homeSC[6];
static bool shortcutLoaded = false;
static bool shortcutLaunched = false;
static char *curReminder = NULL;
static char *curDayPlanner = NULL;
static char *curTodo = NULL;
static char naString[] = "N/A";
static int scCount = 0;
static int homePage = 0;
static int homeCursor = 0;
static int lastButton = 0;

// prototypes
void createHome();
void drawHomeScreen();
void drawHomeScreenMore();
void drawHomeScreenShortcuts();
void runHomeAction(int x, int y);
int cursorHomeScreen(int z, int y);

bool wasShortcutLaunched()
{
	return shortcutLaunched;
}

void resetShortcutLaunchedFlag()
{
	shortcutLaunched = false;
}

void setStrings()
{
	for(int x=0;x<MAX_MODES;x++)
	{	
		switch(locations[x])
		{
			case 0:		
				strings[x] = l_calendar;
				sprites[x] = spr_Cal;
				break;
			case 1:
				strings[x] = l_dayview;
				sprites[x] = spr_Day;
				break;
			case 2:
				strings[x] = l_addressbook;
				sprites[x] = spr_Add;
				break;
			case 3:
				strings[x] = l_todo;
				sprites[x] = spr_Todo;
				break;
			case 4:
				strings[x] = l_scribble;
				sprites[x] = spr_Scribble;
				break;
			case 5:
				strings[x] = l_browser;
				sprites[x] = spr_browser;
				break;
			case 6:
				strings[x] = l_calculator;
				sprites[x] = spr_calc;
				break;
			case 7:
				strings[x] = l_irc;
				sprites[x] = spr_irc;
				break;
			case 8:
				strings[x] = l_webbrowser;
				sprites[x] = spr_webbrowser;
				break;
			case 9:
				strings[x] = l_database;
				sprites[x] = spr_hbdb;
				break;
			case 10:
			case 11:
				strings[x] = naString;
				sprites[x] = spr_unavailable;
				break;
		}
	}
}

void destroyShortcuts()
{
	if(!scCount)
		return;
	
	for(int x = 0;x < scCount;x++)
		freeShortcutIcon(&homeSC[x]);
	
	memset(&homeSC[0], 0, sizeof(SHORTCUT) * MAX_SHORTCUTS);
	scCount = 0;
	
	shortcutLoaded = false;
}

void homeMoveCursor(int x, int y)
{
	switch(getControlID())
	{		
		case 0:
			if(homeCursor >= 3)
			{
				homeCursor -= 3;
			}
			break;
		case 1:
			homeCursor += 3;
			
			if(homeCursor > 6)
			{
				homeCursor = 6;
			}
			break;
		case 2:
			if(homeCursor > 0)
			{
				homeCursor--;
			}
			break;
		case 3:
			if(homeCursor < 6)
			{
				homeCursor++;
			}
			break;
	}
	
	lastButton = getControlID();
}

void homeCallAbout(int x, int y)
{	
	setMode(ABOUT);
	initAbout();
}

void homeLButton(int x, int y)
{
	if(homePage > 0)
	{
		homePage--;
	}
}

void homeRButton(int x, int y)
{
	if(homePage < 2) // FIXME
	{
		homePage++;
	}
}

void homeClick(int x, int y)
{
	int tmpCur = cursorHomeScreen(x, y);
	
	if(tmpCur != -1)
	{
		if(homePage >= 2 && tmpCur >= scCount)
		{
			if(tmpCur != 6)
			{
				return;
			}
		}
		
		homeCursor = tmpCur;
		runHomeAction(0,0);
	}
}

void loadHome()
{
	// load reminder
	loadCurrentReminder(&curReminder);
	abbreviateString(curReminder, 240, font_gautami_10);
	
	char *tmpStr = strchr(curReminder, RET);
	if(tmpStr)
	{
		tmpStr[0] = '.';
		tmpStr[1] = '.';
		tmpStr[2] = '.';
		tmpStr[3] = 0;
	}
	
	// load day planner
	loadCurrentDayPlanner(&curDayPlanner);
	
	// load todos
	loadCurrentTodos(&curTodo);
}

void freeHome()
{
	shortcutLoaded = false;
	
	destroyShortcuts();
	
	if(curReminder)
		trackFree(curReminder);
	curReminder = NULL;
	
	if(curDayPlanner)
		trackFree(curDayPlanner);
	curDayPlanner = NULL;
	
	if(curTodo)
		trackFree(curTodo);
	curTodo = NULL;
}

void initHome()
{
	// Ensure no leftovers
	freeHome();
	
	// Load current homepage
	loadHome();
	
	// Set up controls
	createHome();
}

void drawHomePages()
{
	drawSplash();
	
	switch(homePage)
	{
		case 0:
			drawHomeScreen();
			break;
		case 1:
			drawHomeScreenMore();
			break;
		default:
			drawHomeScreenShortcuts();		
			break;
	}
}

void createHome()
{
	clearControls();
	
	registerScreenUpdate(drawHomePages, freeHome);
	registerHelpScreen("home.html", true);
	
	newControl(0xFF, 0, 0, 255, 191, CT_HITBOX, NULL);
	setControlCallbacks(0xFF, homeClick, NULL, NULL);
	
	// for dpad
	addHotKey(0, KEY_UP, homeMoveCursor, 0, NULL);
	addHotKey(1, KEY_DOWN, homeMoveCursor, 0, NULL);
	addHotKey(2, KEY_LEFT, homeMoveCursor, 0, NULL);
	addHotKey(3, KEY_RIGHT, homeMoveCursor, 0, NULL);
	
	// for key input
	addHotKey(10, KEY_L, homeLButton, 0, NULL);
	addHotKey(11, KEY_R, homeRButton, 0, NULL);	
	addHotKey(12, KEY_START, homeCallAbout, 0, NULL);
	addHotKey(13, KEY_A, runHomeAction, 0, NULL);
}

void drawSplash()
{
	fb_setDefaultClipping();
	drawTimeDate();
	
	// display reminder
	setColor(genericTextColor);
	setFont(font_gautami_10);
	fb_dispString(3, 41, l_reminder);	
	fb_drawFilledRect(3, 41 + 8, 252, 41 + 23, widgetBorderColor, textEntryFillColor);
	fb_setClipping(3, 41 + 8, 252, 41 + 26);
	setColor(textEntryTextColor);
	fb_dispString(3, 5, curReminder);
	
	// display day planner
	fb_setDefaultClipping();
	setColor(genericTextColor);
	fb_dispString(3, 68, l_dayview);	
	fb_drawFilledRect(3, 68 + 8, 252, 68 + 48, widgetBorderColor, textEntryFillColor);
	fb_setClipping(3, 68 + 8, 252, 68 + 51);
	setColor(textEntryTextColor);
	fb_dispString(3, 4, curDayPlanner);
	
	// display todo	
	fb_setDefaultClipping();
	setColor(genericTextColor);
	fb_dispString(3, 120, l_todo);	
	fb_drawFilledRect(3, 120 + 8, 252, 120 + 47, widgetBorderColor, textEntryFillColor);
	fb_setClipping(3, 120 + 8, 252, 120 + 50);
	setColor(textEntryTextColor);
	fb_dispString(3, 5, curTodo);
	
	// draw bottom welcome
	fb_setDefaultClipping();
	setFont(font_arial_11);
	setColor(homeDateTextColor);
	
	fb_setClipping(0, 0, 255, 191);
	fb_drawRect(0,191-16, 255, 191, homeDateFillColor);
	
	PERSONAL_DATA p = *PersonalData;
	char str[256];
	char name[20];
	
	memset(name, 0, 20);
	
	for(uint16 x=0;x<p.nameLen;x++)
		name[x] = (char)(p.name[x] & 0xFF);
	
	sprintf(str, "%s, %s!", l_welcome, name);
	fb_dispString(3,191-17,str);			
	
	fb_drawRect(0,191-17,255,191-17,homeDateBorderColor);
	fb_drawRect(0,191-19,255,191-18,homeDateBorderFillColor);
	fb_drawRect(0,191-20,255,191-20,homeDateBorderColor);
}

void drawHomeScreen()
{
	char str[256];
	
	setStrings();
	strcpy(str, lLanguage);
	strlwr(str);
	
	setColor(genericTextColor);
	setFont(font_arial_9);
	
	if(strcmp(str, "english") != 0)
	{	
		if(homeCursor <= 5)
			bg_dispString((255 - getStringWidth(strings[homeCursor],font_arial_9))/2, 5, strings[homeCursor]);
	}

	for(int x=0;x<6;x++)
	{
		if(homeCursor == x)
			bg_drawBox(25 + ((x % 3) * 75), 25 + ((x / 3) * 75), 25 + ((x % 3) * 75) + 53, 25 + ((x / 3) * 75) + 53, homeHighlightColor);		
	}
	if(homeCursor == 6)
		bg_drawBox(55, 165, 198, 185, homeHighlightColor);
		
	bg_dispSprite(27, 27, sprites[0], 31775);
	bg_dispSprite(102, 27, sprites[1], 31775);
	bg_dispSprite(177, 27, sprites[2], 31775);
	bg_dispSprite(27, 102, sprites[3], 31775);
	bg_dispSprite(102, 102, sprites[4], 31775);
	bg_dispSprite(177, 102, sprites[5], 31775);
	
	bg_dispString((255 - getStringWidth(l_configuration,font_arial_9))/2, 168, l_configuration);
	
	// next arrow
	bg_dispSprite(211, 165, spr_next, 31775);
}

int cursorHomeScreen(int z, int y)
{
	if(z >= 55 && z <= 199 && y >= 165 && y <= 185)
		return 6;
	
	for(int x=0;x<6;x++)
	{
		if(z >= (25 + ((x % 3) * 75)) && y >= (25 + ((x / 3) * 75)) && z <= (25 + ((x % 3) * 75) + 54) && y <= (25 + ((x / 3) * 75) + 54))
			return x;
	}
	
	if(y >= 165)
	{
		if(z <= 50)
		{
			// back
			homeLButton(0,0);
		}
		
		if(z >= 163)
		{
			// forward
			homeRButton(0,0);
		}
	}
	
	return -1;
}

void drawHomeScreenMore()
{
	char str[256];
	
	setStrings();
	strcpy(str, lLanguage);
	strlwr(str);
	
	setColor(genericTextColor);
	setFont(font_arial_9);
		
	if(homeCursor > 6)
		homeCursor = 6;
	
	if(strcmp(str, "english") != 0)
	{
		if(homeCursor <= 5)
			bg_dispString((255 - getStringWidth(strings[homeCursor + 6],font_arial_9))/2, 5, strings[homeCursor + 6]);
	}
	
	for(int x=0;x<6;x++)
	{
		if(homeCursor == x)
			bg_drawBox(25 + ((x % 3) * 75), 25 + ((x / 3) * 75), 25 + ((x % 3) * 75) + 53, 25 + ((x / 3) * 75) + 53, homeHighlightColor);		
	}
	
	if(homeCursor == 6)
		bg_drawBox(55, 165, 198, 185, homeHighlightColor);
	
	bg_dispSprite(27, 27, sprites[6], 31775);
	bg_dispSprite(102, 27, sprites[7], 31775);
	bg_dispSprite(177, 27, sprites[8], 31775);
	bg_dispSprite(27, 102, sprites[9], 31775);
	bg_dispSprite(102, 102, sprites[10], 31775);
	bg_dispSprite(177, 102, sprites[11], 31775);
	
	bg_dispString((255 - getStringWidth(l_configuration,font_arial_9))/2, 168, l_configuration);
	
	// next/prev arrow
	bg_dispSprite(211, 165, spr_next, 31775);
	bg_dispSprite(5, 165, spr_back, 31775);
}

int compareShortcut(const void * a, const void * b)
{
	SHORTCUT *v1 = (SHORTCUT*)a;
	SHORTCUT *v2 = (SHORTCUT*)b;
	
	char str1[256];
	char str2[256];
	
	strcpy(str1, v1->location);
	strcpy(str2, v2->location);
	
	strlwr(str1);
	strlwr(str2);
	
	return strcmp(str1, str2);	
}

void drawHomeScreenShortcuts()
{	
	setColor(0);
	setFont(font_arial_9);
	
	if(!shortcutLoaded)
	{
		// load shortcuts
		
		char tmpFile[256];
		char tStr[6][256];
		int fType;
		
		DRAGON_chdir(d_home);
		fType = DRAGON_FindFirstFile(tmpFile);
		
		while(fType != FE_NONE && scCount < MAX_SHORTCUTS)
		{
			if(getFileType(tmpFile) == SHORTCUTFILE)
			{
				sprintf(tStr[scCount], "%s%s", d_home, tmpFile);
				scCount++;
			}
			
			fType = DRAGON_FindNextFile(tmpFile);		
		}
		
		DRAGON_closeFind();		
		DRAGON_chdir("/");
		
		if(scCount > 0)
		{
			for(int x=0;x<scCount;x++)
			{				
				loadShortcut(tStr[x], &homeSC[x]);
				loadShortcutIcon(&homeSC[x]);
			}
		}
		
		if(scCount > 1) // lets sort this fucker
			qsort(homeSC, scCount, sizeof(SHORTCUT), compareShortcut);
		
		shortcutLoaded = true;
	}
	
	if(homeCursor > 6)
		homeCursor = 6;
	if(homeCursor >= scCount && homeCursor != 6)
	{
		switch(lastButton)
		{
			case 0:
				homeCursor -= 3;
				
				if(homeCursor >= scCount)
				{
					// If we are still over, move to first
					homeCursor = scCount - 1;
				}
				
				break;
			case 1:
				// Head to config
				homeCursor = 6;
				break;
			case 2:
				// Head to last one
				homeCursor = scCount - 1;
				break;
			case 3:
				// Head to config
				homeCursor = 6;
				break;
		}
	}
	
	if(homeCursor != 6)
		bg_dispString((255 - getStringWidth(homeSC[homeCursor].name,font_arial_9))/2, 5, homeSC[homeCursor].name);
	
	for(int x=0;x<scCount;x++)
	{
		if(homeSC[x].loadedIcon)
		{
			int tX = 0;
			
			if(homeSC[x].loadedIcon[1] == 31)
				tX = 10;
				
			bg_dispSprite(27 + (75 * (x % 3)) + tX, 27 + (75 * (x / 3)) + tX, homeSC[x].loadedIcon, 31775);
		}
		
		if(homeCursor == x)
			bg_drawBox(25 + ((x % 3) * 75), 25 + ((x / 3) * 75), 25 + ((x % 3) * 75) + 53, 25 + ((x / 3) * 75) + 53, homeHighlightColor);		
	}
	
	if(homeCursor == 6)
		bg_drawBox(55, 165, 198, 185, homeHighlightColor);
	
	bg_dispString((255 - getStringWidth(l_configuration,font_arial_9))/2, 168, l_configuration);
	
	// prev arrow
	bg_dispSprite(5, 165, spr_back, 31775);
}

void runShortcut(int cu)
{
	SHORTCUT tS;
	
	memcpy(&tS, &homeSC[cu], sizeof(SHORTCUT));
	
	shortcutLaunched = true;
	
	launchShortcut(&tS);
	
	switch(getFileType(tS.path))
	{
		case SOUNDFILE:
			fb_setBGColor(genericFillColor);
			break;
	}
}

void runHomeAction(int x, int y)
{
	switch(homePage)
	{
		case 0:
			if(homeCursor >= 0 && homeCursor < MAX_ITEMS)
				gotoApplication(homeCursor,true);
			else if(homeCursor == MAX_ITEMS)
				gotoApplication(GO_CONFIG,true);
			break;
		case 1:			
			if(homeCursor >= 0 && homeCursor < MAX_ITEMS)
				gotoApplication(homeCursor + MAX_ITEMS,true);
			else if(homeCursor == MAX_ITEMS)
				gotoApplication(GO_CONFIG,true);
			break;
		default:
			if(homeCursor >= 0 && homeCursor < MAX_ITEMS)
				runShortcut(homeCursor);
			else if(homeCursor == MAX_ITEMS)
				gotoApplication(GO_CONFIG,true);
			break;
	}
}
