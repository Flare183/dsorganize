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
#include <libfb/libcommon.h>
#include <libdt/libdt.h>
#include "calendar.h"
#include "dayview.h"
#include "fatwrapper.h"
#include "colors.h"
#include "fonts.h"
#include "general.h"
#include "graphics.h"
#include "globals.h"
#include "keyboard.h"
#include "language.h"
#include "controls.h"
#include "settings.h"
#include "help.h"

static DV_DYNAMIC *dvStruct = NULL;
static uint16 colors[12] = { 0x221F, 0x0200, 0x7C1F, 0x001F, 0x7C00, 0x4210, 0x0318, 0x0360, 0x4010, 0x2D7F, 0x4000, 0x6318 };
static int whatType = TYPE_DAILY;

extern int curDay;
extern int curMonth;
extern int curYear;
extern bool milTime;

// function prototypes
void createDayView();
void createEditDayView();
void loadRepeatMode();
void drawCurrentEvent();
void drawEditDayView();
void drawEditCurrentEvent();
bool dayViewCursorMove();
void saveDayView();
void dvBookMarkUp();
void dvBookMarkDown();
void scrollCallBack(int x, int y);
void dayviewListCallback(int pos, int x, int y);
void loadDay(int whichDay, int whichMonth, int whichYear);
void editDayViewAction(char c);

int getCurTime()
{
	int ct = getHour(true) * 2;
	
	if(getMinute() > 29)
		ct++;
	
	return ct;
}

void freeDayView()
{
	if(dvStruct != NULL)
	{
		trackFree(dvStruct);
	}
	
	dvStruct = NULL;
}

void resetDayView()
{
	// Ensure no leftover
	freeDayView();
	
	// Create controls
	createDayView();
}

void checkDayViewLoaded()
{
	if(dvStruct == NULL)
	{
		loadDay(curDay, curMonth, curYear);
	}
}

void initDayView()
{
	// set up screen
	resetDayView();
	
	// move cursor to time
	moveCursorAbsolute(getCurTime());
	
	// ensure loaded at startup
	checkDayViewLoaded();
}

void exitDayView(int x, int y)
{	
	freeDayView();
	returnHome();
}

void dayViewForward(int x, int y)
{
	loadRepeatMode();
	createEditDayView();
}

void dayViewBack(int x, int y)
{
	freeDayView();
	initCalendar();
	setMode(CALENDAR);
}

void editDayViewForward(int x, int y)
{
	saveDayView();
	resetDayView();
}

void editDayViewBack(int x, int y)
{
	resetDayView();
}

void dayViewNextDay(int x, int y)
{
	int cd = curDay;
	int cm = curMonth-1;
	int cy = curYear;
	
	addDay(&cd, &cm, &cy);
	
	curDay = cd;
	curMonth = cm+1;
	curYear = cy;	
	
	freeDayView();
}

void dayViewPrevDay(int x, int y)
{
	int cd = curDay;
	int cm = curMonth-1;
	int cy = curYear;
	
	subtractDay(&cd, &cm, &cy);
	
	curDay = cd;
	curMonth = cm+1;
	curYear = cy;		
	
	freeDayView();
}

void dayViewMoveCursor(int x, int y)
{
	switch(getControlID())
	{
		case 0: // up button
			if(getCursor() > 0)
			{
				moveCursorRelative(-1);
			}
			break;
		case 1: // down button
			if(getCursor() < 47)
			{
				moveCursorRelative(1);
			}
			break;
		case 2: // left button
			dvBookMarkUp();
			break;
		case 3: // right button
			dvBookMarkDown();
			break;
		case 4: // edit up button
			moveKBCursorAbsolute(getPositionAbove());
			break;
		case 5: // edit down button
			moveKBCursorAbsolute(getPositionBelow());
			break;
		case 6: // edit left button
			if(getKBCursor() > 0)
			{
				moveKBCursorRelative(CURSOR_BACKWARD);
			}
			break;
		case 7: // edit right button
			if(dayViewCursorMove())
			{
				moveKBCursorRelative(CURSOR_FORWARD);	
			}
			break;
	}
}

void updateLRButtons()
{
	switch(whatType)
	{
		case TYPE_DAILY:
			updateControl(CONTROL_L, "N/A");
			setControlEnabled(CONTROL_L, false);
			updateControl(CONTROL_R, l_weekly);
			break;
		case TYPE_WEEKLY:
			updateControl(CONTROL_L, l_daily);
			setControlEnabled(CONTROL_L, true);
			updateControl(CONTROL_R, l_monthly);
			break;
		case TYPE_MONTHLY:
			updateControl(CONTROL_L, l_weekly);
			updateControl(CONTROL_R, l_annually);
			setControlEnabled(CONTROL_R, true);
			break;
		case TYPE_ANNUALLY:
			updateControl(CONTROL_L, l_monthly);
			updateControl(CONTROL_R, "N/A");
			setControlEnabled(CONTROL_R, false);
			break;
	}
}

void editDayViewNextDay(int x, int y)
{
	whatType++;
	updateLRButtons();
}

void editDayViewPrevDay(int x, int y)
{
	whatType--;
	updateLRButtons();
}

void updateDayViewScreen()
{	
	drawTimeDate();
	drawCurrentEvent();
}

void updateEditDayViewScreen()
{	
	drawTimeDate();
	drawEditDayView();
	drawEditCurrentEvent();
}

void createDayView()
{
	clearControls();
	
	registerScreenUpdate(updateDayViewScreen, NULL);
	registerHelpScreen("dayplanner.html", true);
	
	addHome(l_home, exitDayView);	
	addDefaultButtons(l_editselected, dayViewForward, l_calendar, dayViewBack);
	addDefaultLR(l_prevday, dayViewPrevDay, l_nextday, dayViewNextDay);
	addListBox(LIST_LEFT, LIST_TOP, LIST_RIGHT, LIST_BOTTOM, DV_COUNT, NULL, dayviewListCallback, scrollCallBack);
	setControlCallbacks(CONTROL_LIST, dayViewForward, NULL, NULL);
	
	// for dpad
	addHotKey(0, KEY_UP, dayViewMoveCursor, KEY_UP, NULL);
	addHotKey(1, KEY_DOWN, dayViewMoveCursor, KEY_DOWN, NULL);
	addHotKey(2, KEY_LEFT, dayViewMoveCursor, 0, NULL);
	addHotKey(3, KEY_RIGHT, dayViewMoveCursor, 0, NULL);
	
	setControlRepeat(0, 15, 4);
	setControlRepeat(1, 15, 4);
}

void createEditDayView()
{
	resetKBCursor();
	clearControls();
	
	registerScreenUpdate(updateEditDayViewScreen, NULL);
	registerHelpScreen("edit.html", true);
	
	addHome(l_swap, toggleKeyboard);
	addDefaultButtons(l_save, editDayViewForward, l_back, editDayViewBack);
	addDefaultLR(NULL, editDayViewPrevDay, NULL, editDayViewNextDay);
	addKeyboard(0, 0, true, false, SCREEN_BG, editDayViewAction, NULL);
	
	updateLRButtons();
	
	// for dpad
	addHotKey(4, KEY_UP, dayViewMoveCursor, KEY_UP, NULL);
	addHotKey(5, KEY_DOWN, dayViewMoveCursor, KEY_DOWN, NULL);
	addHotKey(6, KEY_LEFT, dayViewMoveCursor, KEY_LEFT, NULL);
	addHotKey(7, KEY_RIGHT, dayViewMoveCursor, KEY_RIGHT, NULL);
	
	setControlRepeat(4, 15, 4);
	setControlRepeat(5, 15, 4);
	setControlRepeat(6, 15, 4);
	setControlRepeat(7, 15, 4);
}

void loadRepeatMode()
{
	whatType = TYPE_DAILY;
	
	if(strlen(dvStruct->dayView[getCursor()]) > 0)
	{
		whatType = dvStruct->dayType[getCursor()];
	}
}

bool dayViewCursorMove()
{
	return dvStruct->dayView[getCursor()][getKBCursor()] != 0;
}

void loadFile(char *tFile, DV_DYNAMIC *dvFile, int wType)
{
	if(DRAGON_FileExists(tFile) == FE_FILE)
	{
		// we can load, it exists		
		DRAGON_FILE *fFile = DRAGON_fopen(tFile, "r");
		
		int timeSlot = 0;
		uint16 textSize = 0;
		
		while(!DRAGON_feof(fFile))
		{
			// get the time it should appear
			timeSlot = DRAGON_fgetc(fFile);
			
			// get the size of the file
			textSize = DRAGON_fgetc(fFile);
			textSize = textSize | DRAGON_fgetc(fFile) << 8;
			
			if(strlen(dvFile->dayPath[timeSlot]) == 0)
			{
				// get the entry itself
				DRAGON_fread(dvFile->dayView[timeSlot], 1, textSize, fFile);
				dvFile->dayType[timeSlot] = wType;
				
				// pop the current filename into the path
				strncpy(dvFile->dayPath[timeSlot], tFile, 255);
			}
			else
			{
				// already have an event there
				DRAGON_fseek(fFile, textSize, SEEK_CUR);
			}
		}	
		
		DRAGON_fclose(fFile);
	}
}

void loadDay(int whichDay, int whichMonth, int whichYear)
{
	// clear out a prior run
	freeDayView();
	
	dvStruct = (DV_DYNAMIC *)trackMalloc(sizeof(DV_DYNAMIC), "tmp dayview");
	memset(dvStruct, 0, sizeof(DV_DYNAMIC));
	
	char tmpFile[256];
	
	// grab the latest day
	sprintf(tmpFile,"%s%02d%02d%04d.DPL", d_day, whichMonth, whichDay, whichYear);
	loadFile(tmpFile, dvStruct, TYPE_DAILY);
	
	// grab anything that is weekly	
	sprintf(tmpFile,"%s%02d.DPL", d_day, dayOfWeek(whichDay, whichMonth, whichYear));
	loadFile(tmpFile, dvStruct, TYPE_WEEKLY);
	
	// grab anything that is monthly
	sprintf(tmpFile,"%s--%02d----.DPL", d_day, whichDay);
	loadFile(tmpFile, dvStruct, TYPE_MONTHLY);
	
	// grab anything that is annual
	sprintf(tmpFile,"%s%02d%02d----.DPL", d_day, whichDay, whichMonth);
	loadFile(tmpFile, dvStruct, TYPE_ANNUALLY);
}

void saveFile(char *fName, DV_DYNAMIC *dvFile) 
{
	bool dataExists = false;
	
	// check to see if this file even has data
	for(int tTime = 0;tTime < DV_COUNT; tTime++)
	{
		if(dvStruct->dayView[tTime][0] != 0)
		{
			dataExists = true;
			break;
		}
	}
	
	if(!dataExists)
	{
		// delete the file because data is not present
		
		if(DRAGON_FileExists(fName) == FE_FILE)
		{
			//delete file, nothing in it!
			DRAGON_remove(fName);			
		}
		
		return;
	}
	
	DRAGON_FILE *fFile = DRAGON_fopen(fName, "w");
	
	for(int tTime = 0; tTime < DV_COUNT; tTime++)
	{
		int textSize = strlen(dvFile->dayView[tTime]);
		
		if(textSize > 0)
		{
			DRAGON_fputc((char)tTime, fFile);
			DRAGON_fputc((char)(textSize & 0xFF), fFile);
			DRAGON_fputc((char)((textSize >> 8) & 0xFF), fFile);			
			DRAGON_fwrite(dvFile->dayView[tTime], 1, textSize, fFile); // write the entry itself
		}	
	}
	
	DRAGON_fclose(fFile);
}

void saveData(char *fName)
{
	// create some space for the temporary file
	DV_DYNAMIC *dvSave = (DV_DYNAMIC *)trackMalloc(sizeof(DV_DYNAMIC), "tmp save dayview");	
	memset(dvSave, 0, sizeof(DV_DYNAMIC));
	
	// grab the current contents of the file to be updated
	loadFile(fName, dvSave, -1);
	
	// loop through the current struct, updating as necessary	
	for(int tTime = 0;tTime < DV_COUNT; tTime++)
	{
		if(strcmp(dvStruct->dayPath[tTime], fName) == 0)
		{
			// an entry needs to be updated			
			memcpy(dvSave->dayView[tTime], dvStruct->dayView[tTime], DV_SIZE);
		}
	}
	
	saveFile(fName, dvSave);
	trackFree(dvSave);
}

void saveDayView()
{
	// set up filenames for new entries
	char tFile[256];
	
	switch(whatType)
	{
		case TYPE_DAILY:
			sprintf(tFile,"%s%02d%02d%04d.DPL", d_day, curMonth, curDay, curYear);
			break;
		case TYPE_WEEKLY:
			sprintf(tFile,"%s%02d.DPL", d_day, dayOfWeek(curDay, curMonth, curYear));
			break;
		case TYPE_MONTHLY:
			sprintf(tFile,"%s--%02d----.DPL", d_day, curDay);
			break;
		case TYPE_ANNUALLY:
			sprintf(tFile,"%s%02d%02d----.DPL", d_day, curDay, curMonth);
			break;
	}
	
	// check to see if the data changed repeating types (and existed before)
	if(dvStruct->dayPath[getCursor()][0] != 0 && whatType != dvStruct->dayType[getCursor()])
	{
		// it hopped types, so we must clear it out of the old data		
		char tData[DV_SIZE];
		
		memcpy(tData, dvStruct->dayView[getCursor()], DV_SIZE);
		memset(dvStruct->dayView[getCursor()], 0, DV_SIZE);
		
		// save the temporary erased data
		saveData(dvStruct->dayPath[getCursor()]);
		
		// restore data
		memcpy(dvStruct->dayView[getCursor()], tData, DV_SIZE);
	}
	
	// now it's safe to update
	dvStruct->dayType[getCursor()] = whatType;
	strncpy(dvStruct->dayPath[getCursor()], tFile, 255);
	saveData(dvStruct->dayPath[getCursor()]);
}

void loadDayPlanner(char **rStr, int tDay, int tMonth, int tYear)
{
	loadDay(tDay, tMonth, tYear);
	
	*rStr = (char *)trackMalloc(1024, "tmp dayview string");
	memset(*rStr, 0, 1024);
	
	int a = getCurTime();
	int z = a + 2;
	
	while(z > DV_COUNT - 1)
	{
		a--;
		z--;
	}
	
	for(uint16 x=a;x<=z;x++)
	{
		char str[1024];		
		memset(str, 0, 1024);
		
		if(milTime)
		{
			if(x & 1 == 1)
				sprintf(str,"%02d:30\t%s\n", x >> 1, dvStruct->dayView[x]);
			else
				sprintf(str,"%02d:00\t%s\n", x >> 1, dvStruct->dayView[x]);	
		}
		else
		{
			if(x & 1 == 1)
				sprintf(str,"%d:30\t%s\n", format12(x >> 1), dvStruct->dayView[x]);
			else
				sprintf(str,"%d:00\t%s\n", format12(x >> 1), dvStruct->dayView[x]);
		}			
		
		abbreviateString(str, 240, font_gautami_10);
		strcat(*rStr, str);
	}
	
	freeDayView();
}

void loadCurrentDayPlanner(char **rStr)
{
	loadDayPlanner(rStr, getDay(), getMonth(), getYear());
}

void drawCurDVTime()
{
	char str[512];
	setFont(font_arial_11);
	setColor(homeDateTextColor);
	
	fb_setClipping(0, 0, 255, 191);
	fb_drawRect(0,191-16, 255, 191, homeDateFillColor);
	
	char date[20];
	if(isReverseDate())
		sprintf(date, "%02d/%02d/%04d", curDay, curMonth, curYear);
	else
		sprintf(date, "%d/%02d/%04d", curMonth, curDay, curYear);
	
	if(milTime)
		sprintf(str,"%s, %d:%02d", date, getCursor() >> 1, 30 *(getCursor() & 1));
	else
		sprintf(str,"%s, %d:%02d %s", date, format12(getCursor() >> 1), 30 *(getCursor() & 1), l_ampm[getCursor() / 24]);
	
	fb_dispString(3,191-16,str);			
	
	fb_drawRect(0,191-17,255,191-17,homeDateBorderColor);
	fb_drawRect(0,191-19,255,191-18,homeDateBorderFillColor);
	fb_drawRect(0,191-20,255,191-20,homeDateBorderColor);
}

void drawCurrentEvent()
{
	checkDayViewLoaded();
	
	setFont(font_arial_9);
	setColor(genericTextColor);
	fb_setClipping(3,41,250,190-20);
	fb_dispString(0,-3,dvStruct->dayView[getCursor()]);
	
	drawCurDVTime();	
}

void drawEditDayView()
{	
	setFont(font_arial_9);
	setColor(textAreaTextColor);
	fb_drawRect(0,37,255,191-18,textAreaFillColor);
	fb_setClipping(3,41,250,190-20);
	setWrapToBorder();
	
	if(blinkOn())
	{
		if(isInsert())
			setCursorProperties(cursorNormalColor, 1, -1, 0);
		else
			setCursorProperties(cursorOverwriteColor, 1, -1, 0);
			
		showCursor();
		setCursorPos(getKBCursor());		
	}
	
	setFakeHighlight();
	
	fb_dispString(0,-3,dvStruct->dayView[getCursor()]);
	
	setWrapNormal();
	hideCursor();
	clearHighlight();
	
	drawCurDVTime();
}

void drawEditCurrentEvent()
{
	char str[512];
	
	setFont(font_arial_11);
	setColor(genericTextColor);
	
	char date[20];
	if(isReverseDate())
		sprintf(date, "%02d/%02d/%04d", curDay, curMonth, curYear);
	else
		sprintf(date, "%d/%02d/%04d", curMonth, curDay, curYear);
	
	char *whatMode = NULL;
	
	switch(whatType)
	{
		case TYPE_DAILY:			
			whatMode = l_daily;
			break;
		case TYPE_WEEKLY:
			whatMode = l_weekly;
			break;
		case TYPE_MONTHLY:
			whatMode = l_monthly;
			break;
		case TYPE_ANNUALLY:
			whatMode = l_annually;
			break;
	}
	
	if(milTime)
	{
		if(getCursor() & 1 == 1)
			sprintf(str,"%s @ %02d:30 - %s", date, getCursor() >> 1, whatMode);
		else
			sprintf(str,"%s @ %02d:00 - %s", date, getCursor() >> 1, whatMode);	
	}
	else
	{
		if(getCursor() & 1 == 1)
			sprintf(str,"%s @ %d:30 - %s", date, format12(getCursor() >> 1), whatMode);
		else
			sprintf(str,"%s @ %d:00 - %s", date, format12(getCursor() >> 1), whatMode);
	}
	
	bg_dispString(13,20-2,str);
}

void scrollCallBack(int sx, int sy)
{
	if(dvStruct == NULL)
	{
		return;
	}
	
	// draw scroll bar contents
	
	for(uint16 x=0;x<=DV_COUNT - 1;x++)
	{
		if(dvStruct->dayView[x][0] != 0)
		{
			double z = (double)100 * (double)x;
			z = z / (double)DV_COUNT - 1;
			bg_drawRect(sx + 2, sy + SCROLL_HANDLE_HEIGHT + 12 + (int)z, sx + DEFAULT_SCROLL_WIDTH - 2, sy + SCROLL_HANDLE_HEIGHT + 14 + (int)z, colors[x%12]);
		}
	}
}

void dayviewListCallback(int pos, int x, int y)
{
	if(dvStruct == NULL)
	{
		return;
	}
	
	char str[512];
	if(milTime)
	{
		if(pos & 1 == 1)
			sprintf(str,"%02d:30", (pos >> 1));
		else
			sprintf(str,"%02d:00", (pos >> 1));		
	}
	else
	{
		if(pos & 1 == 1)
			sprintf(str,"%d:30", format12(pos >> 1));
		else
			sprintf(str,"%d:00", format12(pos >> 1));
	}
	
	if(pos == getCurTime() && curMonth == getMonth() && curYear == getYear() and curDay == getDay())
		setColor(calendarCurrentDateColor);
	else
	{
		if(strlen(dvStruct->dayView[pos]) > 0)
			setColor(colors[pos % 12]);
		else
			setColor(listTextColor);			
	}
	
	bg_dispString(13, 0, str);
	
	if(strlen(dvStruct->dayView[pos]) > 0)
	{
		int tType = dvStruct->dayType[pos];
		
		uint16 *tSprite[] = { dv_daily, dv_weekly, dv_monthly, dv_annually };
		tSprite[tType][4] = listTextColor;
		tSprite[tType][5] = listFillColor;
		
		bg_dispSprite(x + 2, y + 2, tSprite[tType], 0xFFFF);
	}
	
	setColor(listTextColor);
	memset(str, 0, 512);
	
	for(uint16 bb=0;bb<strlen(dvStruct->dayView[pos]);bb++)
	{
		if(dvStruct->dayView[pos][bb] != 10 && dvStruct->dayView[pos][bb] != 13)
			str[bb] = dvStruct->dayView[pos][bb];			
		else
			str[bb] = ' ';
	}
	abbreviateString(str, LIST_WIDTH - 46, font_arial_9);
	bg_dispString(46, 0, str);
}

void dvBookMarkUp()
{
	if(getCursor() == 0)
		return;
	
	for(int i=getCursor()-1;i>=0;--i)
	{
		if(dvStruct->dayView[i][0] != 0)
		{
			moveCursorAbsolute(i);
			return;
		}
	}
}

void dvBookMarkDown()
{
	if(getCursor() == DV_COUNT - 1)
		return;
	
	for(int i=getCursor()+1;i<DV_COUNT;++i)
	{
		if(dvStruct->dayView[i][0] != 0)
		{
			moveCursorAbsolute(i);
			return;
		}
	}
}

void editDayViewAction(char c)
{
	if(c == 0) return;
	
	setPressedChar(c);
	
	if(c == CLEAR_KEY)
	{
		memset(dvStruct->dayView[getCursor()],0,DV_SIZE);
		resetKBCursor();
		return;
	}
	if(c == CAP)
	{
		toggleCaps();
		return;
	}
		
	char *tmpBuffer = (char *)trackMalloc(DV_SIZE,"dayviewer kbd");
	memcpy(tmpBuffer,dvStruct->dayView[getCursor()],DV_SIZE);
	int oldCursor = getKBCursor();
	
	genericAction(tmpBuffer, DV_SIZE - 1, c);
	
	int *pts = NULL;
	int numPts = getWrapPoints(0, -3, tmpBuffer, 3, 41, 250, 190-20, &pts, font_arial_9);
	free(pts);
	
	if(numPts <= 8)
		memcpy(dvStruct->dayView[getCursor()],tmpBuffer,DV_SIZE);
	else
		moveKBCursorAbsolute(oldCursor);
	
	trackFree(tmpBuffer);
}
