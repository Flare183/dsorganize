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
#include <stdlib.h>
#include <string.h>
#include <libfb/libcommon.h>
#include <libdt/libdt.h>
#include "calendar.h"
#include "dayview.h"
#include "fatwrapper.h"
#include "globals.h"
#include "colors.h"
#include "fonts.h"
#include "general.h"
#include "keyboard.h"
#include "language.h"
#include "controls.h"
#include "settings.h"
#include "help.h"

static char reminders[32];
static char dayViews[32];
static bool calLoaded = false;
static char *reminder = NULL;
static char *dayView = NULL;
static int oldYear;
static int oldDay;
static int oldMonth;

int curDay;
int curMonth;
int curYear;

// Function prototypes
void createCalendar();
void createEditCalendar();
void drawCalendar();
void drawReminders();
void drawReminder();
void drawEditReminder();
void saveReminder();
void destroyReminder();
int getDayFromTouch(int tx, int ty);
bool isReminderEnd(int c);
void editReminderAction(char c);

// Memory routines
void destroyDay()
{
	if(dayView != NULL)
		trackFree(dayView);
	
	dayView = NULL;
}

void setDate()
{
	curDay = getDay();
	curMonth = getMonth();
	curYear = getYear();
}

void destroyReminder()
{
	if(reminder != NULL)
		trackFree(reminder);
	
	reminder = NULL;
}

void createReminder()
{
	destroyReminder();
	
	reminder = (char *)trackMalloc(REM_SIZE, "reminder");
	memset(reminder,0,REM_SIZE);
}

void checkReminderLoaded()
{
	if(!(curYear == oldYear && curMonth == oldMonth && curDay == oldDay) || !calLoaded)
	{
		//we have to load new reminders first...
		char str[512];
		
		if(curMonth != oldMonth || !calLoaded)
		{
			// we gotta update the reminders thing
			for(uint16 x=1;x<=31;x++)
			{	
				sprintf(str,"%s%02d%02d%04d.REM",d_reminder,curMonth,x,curYear);
				if(DRAGON_FileExists(str) == FE_FILE)
					reminders[x] = 1;
				else
					reminders[x] = 0;
					
				sprintf(str,"%s%02d%02d%04d.DPL", d_day, curMonth, x, curYear);
				if(DRAGON_FileExists(str) == FE_FILE)
				{
					dayViews[x] = 1;
				}
				else
				{
					sprintf(str,"%s%02d.DPL", d_day, dayOfWeek(x, curMonth, curYear));
					if(DRAGON_FileExists(str) == FE_FILE)
					{
						dayViews[x] = 1;
					}
					else
					{
						sprintf(str,"%s--%02d----.DPL", d_day, x);
						if(DRAGON_FileExists(str) == FE_FILE)
						{
							dayViews[x] = 1;
						}
						else
						{
							sprintf(str,"%s%02d%02d----.DPL", d_day, x, curMonth);
							if(DRAGON_FileExists(str) == FE_FILE)
							{
								dayViews[x] = 1;
							}
							else
							{
								dayViews[x] = 0;
							}
						}
					}
				}
			}
		}
		
		calLoaded = true;
		
		oldYear = curYear;
		oldMonth = curMonth;
		oldDay = curDay;	
		
		sprintf(str,"%s%02d%02d%04d.REM",d_reminder,curMonth,curDay,curYear);	
		
		createReminder();
		
		if(DRAGON_FileExists(str) == FE_FILE)
		{		
			DRAGON_FILE *fFile = DRAGON_fopen(str, "r");
			uint16 len = DRAGON_fread(reminder, 1, REM_SIZE - 1, fFile);
			DRAGON_fclose(fFile);
			
			reminder[len] = 0;
		}
		
		destroyDay();
		loadDayPlanner(&dayView, curDay, curMonth, curYear);
	}
}

void freeCalendar()
{
	destroyReminder();
	destroyDay();
}

void initCalendar()
{
	// Ensure no leftovers
	freeCalendar();
	
	// Initialize controls
	createCalendar();
	
	calLoaded = false;
	
	oldDay = 0;
	oldYear = 0;
	oldMonth = 0;
	
	checkReminderLoaded();
}

// Control callbacks
void exitCalendar(int x, int y)
{
	freeCalendar();	
	clearControls();
	returnHome();
}

void calendarForward(int x, int y)
{
	resetKBCursor();
	createEditCalendar();
}

void calendarBack(int x, int y)
{
	freeCalendar();	
	initDayView();	
	setMode(DAYVIEW);
}

void calendarClick(int x, int y)
{
	int z = getDayFromTouch(x,y);
	
	if(z != -1)
	{
		if(curDay == z && secondClickAction()) // second click
		{
			calendarForward(0,0);
		}
		
		curDay = z;
	}
}

void editCalendarForward(int x, int y)
{
	saveReminder();
	createCalendar();
}

void editCalendarBack(int x, int y)
{
	oldDay = 0;
	initCalendar();
}

void calendarPreviousDay(int x, int y)
{
	--curMonth;
	
	if(curMonth < 1)
	{
		curMonth = 12;
		--curYear;
	}
}

void calendarNextDay(int x, int y)
{
	++curMonth;
	if(curMonth > 12)
	{
		curMonth = 1;
		++curYear;
	}
}

void calendarMoveCursor(int x, int y)
{
	switch(getControlID())
	{
		case 0: // up button
			if(curDay > 7)
			{
				curDay -= 7;
			}
			break;
		case 1: // down button
			if(curDay < daysInMonth(curMonth-1,curYear)-6)
			{
				curDay += 7;
			}
			break;
		case 2: // left button
			--curDay;
			if(curDay < 1)
			{
				curDay = 1;
			}
			break;
		case 3: // right button
			++curDay;
			if(curDay > daysInMonth(curMonth-1,curYear))
			{
				curDay = daysInMonth(curMonth-1,curYear);
			}
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
			if(!isReminderEnd(getKBCursor()))
			{
				moveKBCursorRelative(CURSOR_FORWARD);	
			}
			break;
	}
}

// Screen update routines
void calendarUpdateScreen()
{
	drawTimeDate();
	drawReminders();
	drawCalendar();
}

void editReminderUpdateScreen()
{
	drawTimeDate();
	drawReminder();
	drawEditReminder();
}

// Calendar render creation routines
void createCalendar()
{
	clearControls();
	
	registerScreenUpdate(calendarUpdateScreen, NULL);
	registerHelpScreen("calendar.html", true);
	
	addHome(l_home, exitCalendar);
	addDefaultButtons(l_editreminder, calendarForward, l_viewday, calendarBack);
	addDefaultLR(l_prev, calendarPreviousDay, l_next, calendarNextDay);
	
	newControl(0xFF, 23, 57, 233, 160, CT_HITBOX, NULL);
	setControlCallbacks(0xFF, calendarClick, NULL, NULL);
	
	// for dpad
	addHotKey(0, KEY_UP, calendarMoveCursor, 0, NULL);
	addHotKey(1, KEY_DOWN, calendarMoveCursor, 0, NULL);
	addHotKey(2, KEY_LEFT, calendarMoveCursor, 0, NULL);
	addHotKey(3, KEY_RIGHT, calendarMoveCursor, 0, NULL);
}

void createEditCalendar()
{
	clearControls();
	
	registerScreenUpdate(editReminderUpdateScreen, NULL);
	registerHelpScreen("edit.html", true);
	
	addHome(l_swap, toggleKeyboard);
	addDefaultButtons(l_save, editCalendarForward, l_back, editCalendarBack);
	addKeyboard(0, 0, true, false, SCREEN_BG, editReminderAction, NULL);
	
	// for dpad
	addHotKey(4, KEY_UP, calendarMoveCursor, KEY_UP, NULL);
	addHotKey(5, KEY_DOWN, calendarMoveCursor, KEY_DOWN, NULL);
	addHotKey(6, KEY_LEFT, calendarMoveCursor, KEY_LEFT, NULL);
	addHotKey(7, KEY_RIGHT, calendarMoveCursor, KEY_RIGHT, NULL);

	setControlRepeat(4, 15, 4);
	setControlRepeat(5, 15, 4);
	setControlRepeat(6, 15, 4);
	setControlRepeat(7, 15, 4);
}

// Internal display routines
bool isReminderEnd(int c)
{
	if(reminder == NULL)
		return true;
	
	if(reminder[c] == 0)
		return true;
	
	return false;
}

void drawCalendar()
{	
	uint16 x = 0;
	uint16 y = 0;
	uint16 z = 0;
	
	setFont(font_arial_11);
	setColor(genericTextColor);
	
	{
		char str[128];
		sprintf(str, "%s - %d",l_months[curMonth-1],curYear);
		bg_dispString(23,20-2,str);
	}
	
	setFont(font_gautami_10);
	setColor(genericTextColor);
	
	for(x=0;x<7;x++)
	{		
		bg_dispString(23+(x*31), 20+20, l_days[x+7]);
		bg_drawRect(23+(x*31), 28+20, 20+(x*31)+27, 28+20, calendarDayUnderlineColor);
	}
	
	z = 1;	
	x = dayOfWeek(1,curMonth,curYear);
	y = 0;
	
	if(curDay > daysInMonth(curMonth-1,curYear))
		curDay = daysInMonth(curMonth-1,curYear);
	
	while(z <= daysInMonth(curMonth-1,curYear))
	{	
		if(dayOfWeek(z,curMonth,curYear) == 0 || dayOfWeek(z,curMonth,curYear) == 6)
	    	setColor(calendarWeekendColor);
		else
			setColor(calendarNormalColor);		
		
		if(z == curDay)
		{
			// we gotta draw a rect behind today
			bg_drawRect(23+(x*31), 57+(15*y), 44+(x*31), 70+(15*y), genericHighlightColor);			
		}
		
		if(dayViews[z] == 1)
			bg_drawRect(26+(x*31), 68+(15*y), 41+(x*31), 68+(15*y), calendarHasDayPlannerColor);			
		
		if(reminders[z] == 1)
			setColor(calencarHasReminderColor);		
		
		if(curMonth == getMonth() && curYear == getYear() and z == getDay())
			setColor(calendarCurrentDateColor);
		
		char str[512];	
		
		sprintf(str,"%02d", z);
		bg_dispString(29+(x*31), 40 + 20 + (15*y), str);		
		
		x++;
		if(x > 6)
		{
			x = 0;
			y++;
		}
		
		z++;
	}
}

int getDayFromTouch(int tx, int ty)
{
	uint16 z = 1;	
	uint16 x = dayOfWeek(1,curMonth,curYear);
	uint16 y = 0;
	
	while(z <= daysInMonth(curMonth-1,curYear))
	{	
		if(tx > (x*31) && tx < 21+(x*31))
		{
			if(ty > (15*y) && ty < 13+(15*y))
				return z;
		}		
		
		x++;
		if(x > 6)
		{
			x = 0;
			y++;
		}
		
		z++;
	}
	
	return -1;
}

void drawCurDay()
{
	fb_setClipping(3, 152, 252, 192);
	
	setFont(font_gautami_10);
	setColor(genericTextColor);
	
	fb_dispString(0,5,dayView);
	
	fb_drawRect(0,191-39,255,191-39,homeDateBorderColor);
	fb_drawRect(0,191-41,255,191-40,homeDateBorderFillColor);
	fb_drawRect(0,191-42,255,191-42,homeDateBorderColor);
}

void drawReminders()
{
	checkReminderLoaded();
	
	fb_setClipping(3,41,252,150);
	
	setFont(font_arial_9);
	setColor(genericTextColor);
	
	fb_dispString(0,-3,reminder);
	
	drawCurDay();
}

void loadCurrentReminder(char **rStr)
{
	char str[256];
	sprintf(str,"%s%02d%02d%04d.REM",d_reminder,getMonth(),getDay(),getYear());	
	
	if(DRAGON_FileExists(str) == FE_FILE)
	{		
		DRAGON_FILE *fFile = DRAGON_fopen(str, "r");
		u32 tLen = DRAGON_flength(fFile);
		
		*rStr = (char *)trackMalloc(tLen+5, "tmp reminder");
		memset(*rStr, 0, tLen+5);
		
		DRAGON_fread(*rStr, 1, tLen, fFile);
		DRAGON_fclose(fFile);
	}
	else
	{
		*rStr = (char *)trackMalloc(1, "tmp reminder");
		*rStr[0] = 0;
	}
}

void drawReminder()
{	
	fb_setClipping(3,41,252, 150);
	
	fb_drawRect(0,37,255,148,textAreaFillColor);
	setFont(font_arial_9);
	setColor(textAreaTextColor);	
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
	
	fb_dispString(0,-3,reminder);
	
	setWrapNormal();
	hideCursor();
	clearHighlight();
	drawCurDay();
}

void drawEditReminder()
{	
	char str[128];
	
	setFont(font_arial_11);
	
	if(isReverseDate())
		sprintf(str,"%s, %d %s, %d", l_days[dayOfWeek(curDay, curMonth, curYear)], curDay, l_months[curMonth-1], curYear);
	else
		sprintf(str,"%s, %s %d, %d", l_days[dayOfWeek(curDay, curMonth, curYear)], l_months[curMonth-1], curDay, curYear);
	
	setColor(genericTextColor);
	bg_dispString(13,20-2,str);
}

void saveReminder()
{
	char str[512];
	
	// here is where we write back the reminder if we've gone and edited it
	
	sprintf(str,"%s%02d%02d%04d.REM",d_reminder,curMonth,curDay,curYear);		
	
	if(reminder == NULL || reminder[0] == 0)
	{
		if(DRAGON_FileExists(str) == FE_FILE)
		{
			//delete file, nothing in it!
			DRAGON_remove(str);			
		}
		
		reminders[curDay] = 0;
		return; // gtfo!
	}
	
	DRAGON_FILE *fFile = DRAGON_fopen(str, "w");
	DRAGON_fwrite (reminder, 1, strlen(reminder), fFile);
	DRAGON_fclose(fFile);
	reminders[curDay] = 1;
}

void editReminderAction(char c)
{
	if(c == 0) return;
	
	setPressedChar(c);
	
	if(c == CLEAR_KEY)
	{
		memset(reminder, 0, REM_SIZE);
		resetKBCursor();
		return;
	}
	if(c == CAP)
	{
		toggleCaps();
		return;
	}
	
	char *tmpBuffer = (char *)trackMalloc(REM_SIZE,"reminder kbd");
	memcpy(tmpBuffer,reminder,REM_SIZE);
	int oldCursor = getKBCursor();
	
	genericAction(tmpBuffer, REM_SIZE - 1, c);
	
	int *pts = NULL;
	int numPts = getWrapPoints(0, -3, tmpBuffer, 3, 41, 252, 190, &pts, font_arial_9);
	free(pts);
	
	if(numPts <= MAX_LINES_REMINDER)
		memcpy(reminder,tmpBuffer,REM_SIZE);
	else
		moveKBCursorAbsolute(oldCursor);
	
	trackFree(tmpBuffer);
}
