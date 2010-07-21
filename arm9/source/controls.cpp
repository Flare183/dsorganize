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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libfb/libcommon.h>
#include <libdt/libdt.h>
#include "fonts.h"
#include "controls.h"
#include "general.h"
#include "language.h"
#include "colors.h"
#include "keyboard.h"
#include "graphics.h"
#include "settings.h"
#include "globals.h"
#include "calculator.h"
#include "soundplayer.h"
#include "sidetabs.h"

extern bool milTime;

// for keyboard drawing
extern char me_upper[42];
extern char me_lower[42];
extern char me_number[42];
extern char me_upperspecial[42];
extern char me_lowerspecial[42];

// for control struct
static CONTROL_STRUCT *controls = NULL;
static u32 tcID = 0;
static int controlCount = 0;
static int qX = 0;
static int qY = 0;
static int qT = -1;
static bool hkQueued = false;
static bool ignoreUp = false;
static u32 hkD = 0;
static u32 hkH = 0;

// for keyboard
static char lChar = 0;
static char kChar = 0;
static char eraseChar = 0;

// for key up
static int lastX = -1;
static int lastY = -1;

// for left/right handed
static int handMode = HAND_RIGHT;

// for orientation
static int curOrientation = -1;
static int oldOrientations[10];
static int oldOPosition = 0;

// prototypes
int findControl(u32 controlID);
void drawScrollBarInternal(int pos, int max, bool isHeld, void (*callBack)(int,int), int x, int y, int width, int height, bool displayHandle);
int getScrollBarInternal(int py, int max, int y, int by);
int getCursorFromTouchInternal(int u, int v, int listCur, int listMax, int listLeft, int listTop, int listRight, int listBottom);
void drawListBoxInternal(int tx, int ty, int bx, int by, int curEntry, int maxEntries, char *noString, void (*callback)(int, int, int));
void drawKeyboardInternal(int x, int y, uint16 *bb);
void drawSubKeysInternal(uint16 *bb);
void drawDeleteInternal(uint16 *bb);
bool executeControls(int x, int y, u32 type);
bool executeHotKeys(u32 hkdown, u32 hkheld);

void pushOrientation(int orientation)
{
	if(oldOPosition == MAX_ORIENTATIONS)
	{
		return;
	}
	
	oldOrientations[oldOPosition] = orientation;
	oldOPosition++;
}

int popOrientation()
{
	if(oldOPosition == 0)
	{
		return ORIENTATION_0;
	}
	
	oldOPosition--;
	return oldOrientations[oldOPosition];
}

void setOrientation(int orientation)
{
	// Setting while already in a mode causes flicker
	if(curOrientation == orientation)
		return;
	
	fb_setOrientation(orientation);
	bg_setOrientation(orientation);
	
	curOrientation = orientation;
}

void setNewOrientation(int orientation)
{
	// Setting while already in a mode causes flicker
	if(curOrientation == orientation)
		return;
	
	if(curOrientation != -1)
	{
		// Push old orientation on the stack
		pushOrientation(curOrientation);
	}
	
	// Set orientation
	setOrientation(orientation);
}

void setOldOrientation()
{
	// Pop orientation off the stack and set it
	setOrientation(popOrientation());
}

int getOrientation()
{
	return curOrientation;
}

int getScreenCenter()
{
	switch(curOrientation)
	{
		case ORIENTATION_0:
			return O0_CENTER;
		case ORIENTATION_90:
		case ORIENTATION_270:
			return O90_CENTER;
	}
	
	return 0;
}

int getScreenWidth()
{
	return getScreenCenter() * 2;
}

int getScreenHeight()
{
	switch(curOrientation)
	{
		case ORIENTATION_0:
			return O0_HEIGHT;
		case ORIENTATION_90:
		case ORIENTATION_270:
			return O90_HEIGHT;
	}
	
	return 0;
}

void drawTimeDate()
{
	char str[128];
	setFont(font_arial_11);
	
	setColor(homeDateTextColor);	
	fb_drawRect(0,0, 255, 32, homeDateFillColor);
	
	if(isReverseDate())
		sprintf(str,"%s, %d %s, %d", l_days[getDayOfWeek()], getDay(), l_months[getMonth()-1], getYear());
	else
		sprintf(str,"%s, %s %d, %d", l_days[getDayOfWeek()], l_months[getMonth()-1], getDay(), getYear());
	
	fb_dispString(3,3-2,str);			
	
	if(milTime)
		sprintf(str, "%02d%c%02d%c%02d", getHour(true), ':', getMinute(), ':', getSecond());
	else
		sprintf(str, "%d%c%02d%c%02d %s", getHour(false), ':', getMinute(), ':', getSecond(), l_ampm[isPM()]);
	fb_dispString(3,18-2,str);	
	
	fb_drawRect(0,33,255,33,homeDateBorderColor);
	fb_drawRect(0,34,255,35,homeDateBorderFillColor);
	fb_drawRect(0,36,255,36,homeDateBorderColor);
}

void drawHome(char *hometext, int enabled)
{
	char str[64];
	
	setFont(font_gautami_10);
	
	if(enabled)
	{
		setColor(genericTextColor);
	}
	else
	{
		setColor(soundHoldColor);
	}
	
	sprintf(str,"%c\a\a%s", BUTTON_START, hometext);
	
	bg_setDefaultClipping();
	bg_dispString(centerOnPt(getScreenCenter(),str,font_gautami_10), 3, str);
}

bool isHome(int px, int py, char *text)
{
	if(py >= TOPAREA)
	{
		return false;
	}
	
	char str[128];
	
	sprintf(str,"%c\a\a%s", BUTTON_START, text);	
	
	int offsetPt = 0;
	int centerPt = 0;
	
	switch(curOrientation)
	{
		case ORIENTATION_0:
			centerPt = O0_CENTER;
			break;
		case ORIENTATION_90:
		case ORIENTATION_270:
			centerPt = O90_CENTER;
			break;
	}
	
	offsetPt = centerPt - centerOnPt(centerPt, str, font_gautami_10);	
	return (px >= centerPt - offsetPt) && (px <= centerPt + offsetPt);
}

bool isLButton(int px, int py, char *lButtonPtr)
{
	if(lButtonPtr == 0)
		return false;
	if(py >= TOPAREA)
		return false;
	
	char str[128];
	sprintf(str,"%c\a\a%s", BUTTON_L, lButtonPtr);	
	
	return (px <= getStringWidth(str, font_gautami_10) && px != 0);
}

bool isRButton(int px, int py, char *rButtonPtr)
{
	if(rButtonPtr == 0)
		return false;
	if(py >= TOPAREA)
		return false;
	
	char str[128];
	sprintf(str,"%s\a\a%c", rButtonPtr, BUTTON_R);
	
	switch(curOrientation)
	{
		case ORIENTATION_0:
			return (px >= (O0_RBUTTON - getStringWidth(str, font_gautami_10)));
		case ORIENTATION_90:
		case ORIENTATION_270:
			return (px >= (O90_RBUTTON - getStringWidth(str, font_gautami_10)));
	}
	
	return false;	
}

int getLLocation()
{
	switch(curOrientation)
	{
		case ORIENTATION_0:
			return (O0_FORWARDRIGHT + O0_FORWARDLEFT) / 2;
		case ORIENTATION_90:
		case ORIENTATION_270:
			return (O90_FORWARDRIGHT + O90_FORWARDLEFT) / 2;
	}
	
	return 0;
}

int getRLocation()
{
	switch(curOrientation)
	{
		case ORIENTATION_0:
			return (O0_BACKLEFT + O0_BACKRIGHT) / 2;
		case ORIENTATION_90:
		case ORIENTATION_270:
			return (O90_BACKLEFT + O90_BACKRIGHT) / 2;
	}
	
	return 0;
}

void addHome(char *title, void (*tD)(int, int))
{
    newControl(CONTROL_HOME, 0, 0, 0, 0, CT_HOME, title);
    setControlCallbacks(CONTROL_HOME, tD, NULL, NULL);
	setControlHotkey(CONTROL_HOME, KEY_START, 0);
}

void addDefaultButtons(char *ltitle, void (*tL)(int, int), char *rtitle, void (*tR)(int, int))
{
	char aButton = BUTTON_A;
	char bButton = BUTTON_B;
	
	if(isABSwapped())
	{
		aButton = BUTTON_B;
		bButton = BUTTON_A;
	}
	
	switch(curOrientation)
	{
		case ORIENTATION_0:
			newControl(CONTROL_FORWARD, O0_FORWARDLEFT, O0_TOP, O0_FORWARDRIGHT, O0_BOTTOM, CT_BUTTON, ltitle);
			newControl(CONTROL_BACKWARD, O0_BACKLEFT, O0_TOP, O0_BACKRIGHT, O0_BOTTOM, CT_BUTTON, rtitle);
			
			break;
		case ORIENTATION_90:
		case ORIENTATION_270:
			newControl(CONTROL_FORWARD, O90_FORWARDLEFT, O90_TOP, O90_FORWARDRIGHT, O90_BOTTOM, CT_BUTTON, ltitle);
			newControl(CONTROL_BACKWARD, O90_BACKLEFT, O90_TOP, O90_BACKRIGHT, O90_BOTTOM, CT_BUTTON, rtitle);
			
			break;
	}
	
	setControlCallbacks(CONTROL_FORWARD, tL, NULL, NULL);
	setControlState(CONTROL_FORWARD, aButton);
	setControlHotkey(CONTROL_FORWARD, KEY_A, 0);

	setControlCallbacks(CONTROL_BACKWARD, tR, NULL, NULL);
	setControlState(CONTROL_BACKWARD, bButton);	
	setControlHotkey(CONTROL_BACKWARD, KEY_B, 0);
}

void addDefaultLR(char *ltitle, void (*tL)(int, int), char *rtitle, void (*tR)(int, int))
{
	newControl(CONTROL_L, 0, 0, 0, 0, CT_LBUTTON, ltitle);
	newControl(CONTROL_R, 0, 0, 0, 0, CT_RBUTTON, rtitle);
	
    setControlCallbacks(CONTROL_L, tL, NULL, NULL);
	setControlCallbacks(CONTROL_R, tR, NULL, NULL);
	
	setControlHotkey(CONTROL_L, KEY_L, 0);
	setControlHotkey(CONTROL_R, KEY_R, 0);
}

void addListBox(int x, int y, int bx, int by, u32 maxItems, char *emptyText, void (*cb1)(int, int, int), void (*cb2)(int,int))
{
	newControl(CONTROL_LIST, x, y, bx, by, CT_LIST, emptyText);
	setControlAbsoluteCoords(CONTROL_LIST, true);
	setControlCallbacks(CONTROL_LIST, NULL, cb2, NULL);
	
	int i = findControl(CONTROL_LIST);
	
	controls[i].state = maxItems & 0x7FFF;
	controls[i].priv2 = (void *)cb1;
	controls[i].priv3 = (void *)cb2;
}

void addKeyboard(int x, int y, bool displayExtra, bool displayDelete, int whichScreen, void (*actionKB)(char), void (*del)())
{
	newControl(CONTROL_KEYBOARD, x, y, displayExtra ? 1 : 0, displayDelete ? 1 : 0, CT_KEYBOARD, (void *)actionKB);
	setControlAbsoluteCoords(CONTROL_KEYBOARD, true);
	setControlCallbacks(CONTROL_KEYBOARD, NULL, NULL, del);
	setControlRepeat(CONTROL_KEYBOARD, 15, 4);
	
	int i = findControl(CONTROL_KEYBOARD);
	
	controls[i].priv2 = (void *)whichScreen;
}

void addHotKey(u32 controlID, u32 hotkeyDown, void (*a1)(int,int), u32 hotkeyMove, void (*a2)(int,int))
{
	newControl(controlID, 0, 0, 0, 0, CT_HOTKEY, NULL);
	setControlCallbacks(controlID, a1, a2, NULL);
	setControlHotkey(controlID, hotkeyDown, hotkeyMove);
}

void addSideBar(void (*cb1)(int, int))
{
	newControl(CONTROL_SIDEBAR, 0, 0, 0, 0, CT_SIDEBAR, NULL);
	setControlCallbacks(CONTROL_SIDEBAR, cb1, NULL, NULL);
	
	initSideTabs();
}

void addScrollBar(int x, int y, int by, int maxValue, int *curValue, void (*cb)(int,int))
{
	newControl(CONTROL_SCROLL, x, y, maxValue, by, CT_SCROLL, curValue);
	
	int i = findControl(CONTROL_SCROLL);	
	controls[i].priv2 = (void *)cb;
}

void addCustom(u32 controlID, int x, int y, int bx, int by, void (*df)())
{
	newControl(controlID, x, y, bx, by, CT_CUSTOM, (void *)df);	
}

void registerScreenUpdate(void (*su)(), void (*fr)())
{
	newControl(CONTROL_SCREEN, 0, 0, 0, 0, CT_SCREEN, (void *)su);
    setControlCallbacks(CONTROL_SCREEN, NULL, NULL, fr);
	setControlEnabled(CONTROL_SCREEN, false);
}

void clearControls()
{
	if(!controls)
	{
		return;
	}
	
	for(int i = 0; i < controlCount; i++)
	{
		if(controls[i].type == CT_SCREEN)
		{
			if(controls[i].touchUp != NULL)
			{
				// Run free routine
				controls[i].touchUp();
			}
		}
		else if(controls[i].type == CT_SIDEBAR)
		{
			destroySideTabs();
		}
		
		controls[i].type = CT_DELETED;
	}
	
	free(controls);
	
	controls = NULL;
	controlCount = 0;
	
	// clear queue
	qT = -1;
	hkQueued = false;
	ignoreUp = true;
}

bool doesControlExist(u32 controlID)
{
	return findControl(controlID) >= 0;
}

void newControl(u32 controlID, int x, int y, int bx, int by, u32 type, const void *priv)
{
	int wControl = -1;
	
	if(!controls)
	{
		controlCount = 1;
		controls = (CONTROL_STRUCT *)safeMalloc(sizeof(CONTROL_STRUCT));
		
		// First controls
		wControl = 0;
	}
	else
	{
		// Find an empty control
		for(int i = 0; i < controlCount; i++)
		{
			if(controls[i].type == CT_DELETED)
			{
				wControl = i;
				break;
			}
		}
		
		// No control found
		if(wControl == -1)
		{
			controlCount++;
			controls = (CONTROL_STRUCT *)realloc(controls, sizeof(CONTROL_STRUCT) * controlCount);
			
			memset(&controls[controlCount - 1], 0, sizeof(CONTROL_STRUCT));
			wControl = controlCount - 1;
		}
	}
	
	controls[wControl].x = x;
	controls[wControl].y = y;
	controls[wControl].bx = bx;
	controls[wControl].by = by;
	controls[wControl].type = type;
	
	controls[wControl].controlID = controlID;
	controls[wControl].priv = (void *)priv;
	
	controls[wControl].attributes = ATTR_ENABLED | ATTR_VISIBLE;
}

int findControl(u32 controlID)
{
	if(!controls)
	{
		return -1;
	}
	
	for(int i = 0; i < controlCount; i++)
	{
		if(controls[i].controlID == controlID)
		{
			return i;
		}
	}
	
	return -1;
}

void updateControl(u32 controlID, const void *priv)
{
	if(!controls)
	{
		return;
	}
	
	int i = findControl(controlID);
	
	if(i >= 0)
	{
		controls[i].priv = (void *)priv;
	}
}

void setControlCallbacks(u32 controlID, void (*touchDown)(int x, int y), void (*touchMove)(int x, int y), void (*touchUp)())
{
	if(!controls)
	{
		return;
	}
	
	int i = findControl(controlID);
	
	if(i >= 0)
	{
		controls[i].touchDown = touchDown;
		controls[i].touchMove = touchMove;
		controls[i].touchUp = touchUp;
	}
}

void setControlAttribute(u32 controlID, u32 attrib, bool state)
{
	if(!controls)
	{
		return;
	}
	
	int i = findControl(controlID);
	
	if(i >= 0)
	{
		controls[i].attributes &= (~attrib);
		controls[i].attributes |= state ? attrib : 0;
	}
}

void deleteControl(u32 controlID)
{
	if(!controls)
	{
		return;
	}
	
	int i = findControl(controlID);
	
	if(i >= 0)
	{
		// No need to free memory as that's done every time we switch screens
		memset(&controls[i], 0, sizeof(CONTROL_STRUCT));
		controls[i].type = CT_DELETED;
	}
}

void setControlState(u32 controlID, u32 state)
{
	if(!controls)
	{
		return;
	}
	
	int i = findControl(controlID);
	
	if(i >= 0)
	{
		controls[i].state &= 0x8000;
		controls[i].state |= (state & 0x7FFF);
	}
}

void setControlHotkey(u32 controlID, u32 hotkeydown, u32 hotkeyheld)
{
	if(!controls)
	{
		return;
	}
	
	int i = findControl(controlID);
	
	if(i >= 0)
	{
		controls[i].hotkey1 = hotkeydown;
		controls[i].hotkey2 = hotkeyheld;
	}
}

void setControlRepeat(u32 controlID, u8 repeatStart, u8 repeatContinue)
{
	if(!controls)
	{
		return;
	}
	
	int i = findControl(controlID);
	
	if(i >= 0)
	{	
		setControlAttribute(controlID, ATTR_REPEAT, true);	
		
		controls[i].repeatstart = repeatStart;
		controls[i].repeatcontinue = repeatContinue;
	}
}

void setControlVisible(u32 controlID, bool visible)
{
	setControlAttribute(controlID, ATTR_VISIBLE, visible);
}

void setControlEnabled(u32 controlID, bool enabled)
{
	setControlAttribute(controlID, ATTR_ENABLED, enabled);
}

void setControlGrayed(u32 controlID, bool grayed)
{
	setControlAttribute(controlID, ATTR_GRAYED, grayed);
}

void setControlAbsoluteCoords(u32 controlID, bool abscoord)
{
	setControlAttribute(controlID, ATTR_ABSCOORD, abscoord);
}

void setControlAlwaysDraw(u32 controlID, bool alwaysdraw)
{
	setControlAttribute(controlID, ATTR_ALWAYSDRAW, alwaysdraw);
}

void drawControls(bool specialOnly)
{
	if(!controls)
	{
		return;
	}
	
	for(int i = 0; i < controlCount; i++)
	{
		// only draw the control if we are in all controls mode, or it is marked as special
		if((controls[i].attributes & ATTR_VISIBLE) && (!specialOnly || (specialOnly && (controls[i].attributes & ATTR_ALWAYSDRAW))))
		{
			switch(controls[i].type)
			{
				case CT_SCREEN:
				{
					void (*su)() = (void (*)())controls[i].priv;					
					su();
					
					break;
				}
				case CT_HOME:
					drawHome((char *)controls[i].priv, (controls[i].attributes & ATTR_ENABLED) && !(controls[i].attributes & ATTR_GRAYED));
					break;
				case CT_LBUTTON:
				{
					if(controls[i].priv != NULL)
					{
						char str[128];
						
						setFont(font_gautami_10);
						
						if(controls[i].attributes & ATTR_ENABLED)
						{
							setColor(genericTextColor);
						}
						else
						{
							setColor(soundHoldColor);
						}
							
						sprintf(str,"%c\a\a%s", BUTTON_L, (char *)controls[i].priv);
						bg_setDefaultClipping();
						bg_dispString(3, 3, str);
					}
					
					break;
				}
				case CT_RBUTTON:
				{
					if(controls[i].priv != NULL)
					{
						char str[128];
						
						setFont(font_gautami_10);
						
						if(controls[i].attributes & ATTR_ENABLED)
						{
							setColor(genericTextColor);
						}
						else
						{
							setColor(soundHoldColor);
						}
						
						sprintf(str,"%s\a\a%c", (char *)controls[i].priv, BUTTON_R);
						bg_setDefaultClipping();
						
						switch(curOrientation)
						{
							case ORIENTATION_0:
								bg_dispString(O0_RBUTTON-getStringWidth(str, font_gautami_10),3, str);
								break;
							case ORIENTATION_90:
							case ORIENTATION_270:
								bg_dispString(O90_RBUTTON-getStringWidth(str, font_gautami_10),3, str);
								break;
						}
					}
					
					break;
				}
				case CT_BUTTON:
					if(controls[i].priv != NULL)
					{
						bg_drawFilledRect(controls[i].x, controls[i].y, controls[i].bx, controls[i].by, widgetBorderColor, widgetFillColor);
						
						bg_setClipping(controls[i].x, controls[i].y, controls[i].bx, controls[i].by);
						
						setFont(font_arial_9);
						setColor(widgetTextColor);
						
						if(controls[i].priv != NULL)
						{
							char str[128];
							
							if(controls[i].state > 0)
							{
								sprintf(str, "%c\a%s", controls[i].state, (char *)controls[i].priv);
							}
							else
							{
								strcpy(str, (char *)controls[i].priv);
							}
							
							bg_dispString(centerOnPt((controls[i].bx - controls[i].x) / 2, str, font_arial_9),3,str);
						}
						
						bg_setDefaultClipping();
					}
					break;
				case CT_LIST:
					uint16 numEntries = (((controls[i].by - controls[i].y) - 4) / LIST_STEP) + 1;
					
					if(getCursor() < 0 || getCursor() >= (int)(controls[i].state & 0x7FFF))
					{
						// Protection against cursor above or below a certain amount
						resetCursor();
					}
					
					switch(handMode)
					{	
						case HAND_LEFT:
							drawScrollBarInternal(getCursor(), controls[i].state & 0x7FFF, controls[i].state & 0x8000, (void (*)(int,int))controls[i].priv3, controls[i].x, controls[i].y, DEFAULT_SCROLL_WIDTH, controls[i].by - controls[i].y, ((controls[i].state & 0x7FFF) >= numEntries));
							drawListBoxInternal(controls[i].x + DEFAULT_SCROLL_WIDTH + 2, controls[i].y, controls[i].bx, controls[i].by, getCursor(), controls[i].state & 0x7FFF, (char *)controls[i].priv, (void (*)(int,int,int))controls[i].priv2);
							break;
						case HAND_RIGHT:
							drawScrollBarInternal(getCursor(), controls[i].state & 0x7FFF, controls[i].state & 0x8000, (void (*)(int,int))controls[i].priv3, controls[i].bx - DEFAULT_SCROLL_WIDTH, controls[i].y, DEFAULT_SCROLL_WIDTH, controls[i].by - controls[i].y, ((controls[i].state & 0x7FFF) >= numEntries));
							drawListBoxInternal(controls[i].x, controls[i].y, controls[i].bx - (DEFAULT_SCROLL_WIDTH + 2), controls[i].by, getCursor(), controls[i].state & 0x7FFF, (char *)controls[i].priv, (void (*)(int,int,int))controls[i].priv2);
							break;
					}
					
					break;
				case CT_SCROLL:
					if(*((int *)controls[i].priv) < 0 || *((int *)controls[i].priv) >= controls[i].bx)
					{
						// Protection against cursor above or below a certain amount
						*((int *)controls[i].priv) = 0;
					}
					
					drawScrollBarInternal(*((int *)controls[i].priv), controls[i].bx, controls[i].state & 0x8000, (void (*)(int,int))controls[i].priv2, controls[i].x, controls[i].y, DEFAULT_SCROLL_WIDTH, controls[i].by - controls[i].y, controls[i].bx > 0);
					
					break;
				case CT_KEYBOARD:					
					--eraseChar;		
					if(!eraseChar && pressedChar())
					{
						clearPressedChar();
					}
					else if(pressedChar())
					{
						eraseChar = 1;
					}
					
					drawKeyboardInternal(controls[i].x, controls[i].y, ((int)controls[i].priv2 == SCREEN_FB) ? fb_backBuffer() : bg_backBuffer());
					
					if(controls[i].bx != 0)
					{
						drawSubKeysInternal(((int)controls[i].priv2 == SCREEN_FB) ? fb_backBuffer() : bg_backBuffer());
					}
					
					if(controls[i].by != 0)
					{
						drawDeleteInternal(((int)controls[i].priv2 == SCREEN_FB) ? fb_backBuffer() : bg_backBuffer());
					}
					
					break;
				case CT_SIDEBAR:
					displaySideTabs();
					break;
				case CT_CUSTOM:
					void (*df)();
					
					df = (void (*)())controls[i].priv;
					df();
					
					break;
			}
		}
	}
}

void grabCorrectFunction(int controlPos, int x, int y, u32 type)
{
	if(x >= 0 && y >= 0 && !(controls[controlPos].attributes & ATTR_ABSCOORD))
	{
		// Position relative
		x -= controls[controlPos].x;
		y -= controls[controlPos].y;
	}

	switch(type)
	{
		case CE_DOWN:
			if(controls[controlPos].touchDown)
			{
				controls[controlPos].touchDown(x, y);
			}
			return;
		case CE_MOVE:
			if(controls[controlPos].touchMove)
			{
				controls[controlPos].touchMove(x, y);
			}
			return;
		case CE_UP:
			if(controls[controlPos].touchUp)
			{
				controls[controlPos].touchUp();
			}
			return;
	}
}

void queueControls(int x, int y, int type)
{
	if(!controls)
	{
		return;
	}
	
	if(qT != -1 || kChar != 0)
	{
		if(kChar == 0 && qT == CE_MOVE && (type == CE_UP || type == CE_MOVE))
		{
			// Up takes precidence over a queued move, move always replaces itself
			ignoreUp = false;
			
			qX = x;
			qY = y;
			qT = type;
		}
		
		// already queued, exit
		return;
	}
	
	for(int i = 0; i < controlCount; i++)
	{
		switch(controls[i].type)
		{
			case CT_KEYBOARD:
				if(type == CE_DOWN)
				{	
					int kX = x;
					
					if(activeKeyboard() != KB_MESSAGEASE && (y - controls[i].y) < DEFAULT_TOP)
					{
						kX -= controls[i].x;
					}
					
					kChar = executeClick(kX, y - controls[i].y);
					
					if(kChar == 0 && controls[i].by > 0) // only check for delete if we didn't hit a key and we enabled delete showing
					{
						// test for delete
						if(x >= DELETE_LEFT && y >= DELETE_TOP && x <= DELETE_RIGHT && y <= DELETE_BOTTOM)
						{
							qX = x;
							qY = y;
							qT = CE_DELETE;
							return;
						}
					}
				}
				if(type == CE_UP)
				{
					char kC = executeUp();
					
					if(kC != 0)
					{
						kChar = kC;
					}
				}
				
				if(kChar != 0)
				{
					controls[i].repeatcount = -controls[i].repeatstart;
					return;
				}
		}
	}
	
	if(ignoreUp && type == CE_DOWN)
	{
		ignoreUp = false;
	}
	else if(ignoreUp && type == CE_UP)
	{
		ignoreUp = false;
		return;
	}
	
	qX = x;
	qY = y;
	qT = type;
}

void executeQueuedControls()
{
	if(kChar == 0)
	{
		if(qT != -1)
		{
			executeControls(qX, qY, qT);
		}
		
		qX = 0;
		qY = 0;
		qT = -1;
		
		if(hkQueued)
		{
			executeHotKeys(hkD, hkH);
		}
		
		hkQueued = false;
		
		return;
	}
	
	int i = findControl(CONTROL_KEYBOARD);
	
	if(controls[i].priv != NULL)
	{
		clickSound();
		
		void (*kAction)(char) = (void (*)(char))controls[i].priv;
		kAction(kChar);
		
		if(kChar == CLEAR_KEY || kChar == CAP || kChar == SHF || kChar == SPL)
		{
			lChar = 0;
		}
		else
		{
			lChar = kChar;
		}
	}
	
	kChar = 0;
	qT = -1;
	hkQueued = false;
}

u32 getControlID()
{
	return tcID;
}

bool executeControls(int x, int y, u32 type)
{
	if(!controls)
	{
		return false;
	}
	
	if(type == CE_DOWN || type == CE_MOVE)
	{			
		lastX = x;
		lastY = y;
	}
	else if(type == CE_UP)
	{
		// Clear hold status
		for(int i = (controlCount - 1); i >= 0; i--) // to ensure proper z-order
		{	
			switch(controls[i].type)
			{
				case CT_LIST:
				case CT_SCROLL:
					// Disable held
					controls[i].state &= (~0x8000);
					
					break;
			}
		}
		
		x = lastX;
		y = lastY;
	}
	
	// orientation stuff
	switch(getOrientation())
	{
		case ORIENTATION_90:
		{
			// reverse coordinates
			int tInt = 191 - y;
			
			y = x;
			x = tInt;
			break;
		}
		case ORIENTATION_270:
		{
			// reverse coordinates
			int tInt = y;
			
			y = 255 - x;
			x = tInt;
			break;
		}
	}
	
	for(int i = (controlCount - 1); i >= 0; i--) // to ensure proper z-order
	{	
		if(controls[i].type == CT_DELETED)
		{
			// Skip this control since it's been deleted
			continue;
		}
		
		// To query later
		tcID = controls[i].controlID;
		
		switch(controls[i].type)
		{
			case CT_HOME:
				if(isHome(x, y, (char *)controls[i].priv))
				{
					if(controls[i].attributes & ATTR_ENABLED)
					{
						grabCorrectFunction(i, x, y, type);
					}
					
					return true;
				}
				
				break;
			case CT_LBUTTON:
				if(isLButton(x, y, (char *)controls[i].priv))
				{
					if(controls[i].attributes & ATTR_ENABLED)
					{
						if(controls[i].priv != NULL)
						{
							grabCorrectFunction(i, x, y, type);
						}
					}
					
					return true;
				}
				
				break;
			case CT_RBUTTON:
				if(isRButton(x, y, (char *)controls[i].priv))
				{
					if(controls[i].attributes & ATTR_ENABLED)
					{
						if(controls[i].priv != NULL)
						{
							grabCorrectFunction(i, x, y, type);
						}
					}
					
					return true;
				}
				
				break;
			case CT_SIDEBAR:
				if(isSideTab(x, y))
				{
					if(controls[i].attributes & ATTR_ENABLED && type == CE_DOWN)
					{
						if(clickSideTab(x, y))
						{
							// Only call functions on successful change event
							grabCorrectFunction(i, x, y, type);
						}
					}
					
					return true;
				}
				
				break;
			case CT_BUTTON:
				if(x >= controls[i].x && x <= controls[i].bx && y >= controls[i].y && y <= controls[i].by)
				{
					if(controls[i].attributes & ATTR_ENABLED)
					{
						if(controls[i].priv != NULL)
						{
							grabCorrectFunction(i, x, y, type);
						}
					}
					
					return true;
				}
				
				break;
			case CT_CUSTOM:
				if(x >= controls[i].x && x <= controls[i].bx && y >= controls[i].y && y <= controls[i].by)
				{
					if(controls[i].attributes & ATTR_ENABLED)
					{
						grabCorrectFunction(i, x, y, type);
					}
					
					return true;
				}
				
				break;
			case CT_HITBOX:
				if(x >= controls[i].x && x <= controls[i].bx && y >= controls[i].y && y <= controls[i].by)
				{
					if(controls[i].attributes & ATTR_ENABLED)
					{
						grabCorrectFunction(i, x, y, type);
						return true;
					}
				}
				
				break;
			case CT_LIST:
				if(type == CE_UP)
				{
					return true;
				}
				
				if(y >= controls[i].y && y <= controls[i].by && ((x >= controls[i].x && x <= controls[i].bx) || ((controls[i].state & 0x8000) && x >= (controls[i].x - 20) && x <= (controls[i].bx + 20))))
				{
					u32 numEntries = (((controls[i].by - controls[i].y) - 4) / LIST_STEP) + 1;
					
					switch(handMode)
					{	
						case HAND_LEFT:
							if(x <= controls[i].x + DEFAULT_SCROLL_WIDTH)
							{
								if((controls[i].state & 0x7FFF) < numEntries)
								{
									// exit if not displayed
									break;
								}
								
								// scrollbar
								if(y <= controls[i].y + SCROLL_ARROW_HEIGHT)
								{
									if(type != CE_DOWN)
									{
										break;
									}
									
									if(getCursor() > 0)
									{
										moveCursorRelative(-1);
									}
								}
								else if(y >= controls[i].by - SCROLL_ARROW_HEIGHT)
								{
									if(type != CE_DOWN)
									{
										break;
									}
									
									if(getCursor() < (int)(controls[i].state & 0x7FFF) - 1)
									{
										moveCursorRelative(1);
									}
								}
								else
								{
									controls[i].state |= 0x8000;
									moveCursorAbsolute(getScrollBarInternal(y, (controls[i].state & 0x7FFF), controls[i].y + SCROLL_ARROW_HEIGHT + 2, controls[i].by - (SCROLL_ARROW_HEIGHT + 2)));
								}
								
								return true;
							}
							else
							{
								// listbox
								if(type != CE_DOWN)
								{
									break;
								}
								
								int tCursor = getCursorFromTouchInternal(x, y, getCursor(), (controls[i].state & 0x7FFF), controls[i].x + DEFAULT_SCROLL_WIDTH + 2, controls[i].y, controls[i].bx, controls[i].by);
								
								if(tCursor > -1)
								{
									if(tCursor == getCursor() && secondClickAction())
									{
										// double click
										grabCorrectFunction(i, -1, -1, CE_DOWN);
									}
									else
									{
										moveCursorAbsolute(tCursor);
									}
								}
								
								return true;
							}
							break;
						case HAND_RIGHT:
							if(x >= controls[i].bx - DEFAULT_SCROLL_WIDTH)
							{
								if((controls[i].state & 0x7FFF) < numEntries)
								{
									// exit if not displayed
									break;
								}
								
								// scrollbar
								if(y <= controls[i].y + SCROLL_ARROW_HEIGHT)
								{
									if(type != CE_DOWN)
									{
										break;
									}
									
									if(getCursor() > 0)
									{
										moveCursorRelative(-1);
									}
								}
								else if(y >= controls[i].by - SCROLL_ARROW_HEIGHT)
								{
									if(type != CE_DOWN)
									{
										break;
									}
									
									if(getCursor() < (int)(controls[i].state & 0x7FFF) - 1)
									{
										moveCursorRelative(1);
									}
								}
								else
								{
									controls[i].state |= 0x8000;
									moveCursorAbsolute(getScrollBarInternal(y, (controls[i].state & 0x7FFF), controls[i].y + SCROLL_ARROW_HEIGHT + 2, controls[i].by - (SCROLL_ARROW_HEIGHT + 2)));
								}
								
								return true;
							}
							else
							{
								// listbox
								if(type != CE_DOWN)
								{
									break;
								}
								
								int tCursor = getCursorFromTouchInternal(x, y, getCursor(), (controls[i].state & 0x7FFF), controls[i].x, controls[i].y, controls[i].bx - (DEFAULT_SCROLL_WIDTH + 2), controls[i].by);
								
								if(tCursor > -1)
								{
									if(tCursor == getCursor() && secondClickAction())
									{
										// double click
										grabCorrectFunction(i, -1, -1, CE_DOWN);
									}
									else
									{
										moveCursorAbsolute(tCursor);
									}
								}
								
								return true;
							}
							break;
					}
				}
				break;
			case CT_SCROLL:
				if(type == CE_UP)
				{
					return true;
				}
				
				if(y >= controls[i].y && y <= controls[i].by && ((x >= controls[i].x && x <= (controls[i].x + DEFAULT_SCROLL_WIDTH)) || ((controls[i].state & 0x8000) && x >= (controls[i].x - 20) && x <= ((controls[i].x + DEFAULT_SCROLL_WIDTH) + 20))))
				{
					int numEntries = controls[i].bx;
					
					if(numEntries <= 0)
					{
						// exit if not displayed
						break;
					}
					
					// scrollbar
					if(y <= controls[i].y + SCROLL_ARROW_HEIGHT)
					{
						if(type != CE_DOWN)
						{
							break;
						}
						
						if(*((int *)controls[i].priv) > 0)
						{
							(*((int *)controls[i].priv))--;
						}
					}
					else if(y >= controls[i].by - SCROLL_ARROW_HEIGHT)
					{
						if(type != CE_DOWN)
						{
							break;
						}
						
						if(*((int *)controls[i].priv) < numEntries - 1)
						{
							(*((int *)controls[i].priv))++;
						}
					}
					else
					{
						controls[i].state |= 0x8000;
						*((int *)controls[i].priv) = getScrollBarInternal(y, numEntries, controls[i].y + SCROLL_ARROW_HEIGHT + 2, controls[i].by - (SCROLL_ARROW_HEIGHT + 2));
					}
				}
				
				break;
			case CT_KEYBOARD:
				if(type == CE_DELETE)
				{
					clickSound();
					grabCorrectFunction(i, -1, -1, CE_UP);
					return true;
				}
				
				if(controls[i].repeatcount > controls[i].repeatcontinue)
				{
					// reset count
					controls[i].repeatcount = 0;
					
					if(lChar == 0)
					{
						break;
					}
					
					kChar = executeClick(x - ((activeKeyboard() == KB_MESSAGEASE) ? 0 : controls[i].x), y - controls[i].y);
					if(kChar != lChar)
					{
						kChar = 0;
						lChar = 0;
					}
					
					return true;
				}
				
				break;
		}
	}
	
	return false;
}

void updateControlRepeats()
{
	if(!controls)
	{
		return;
	}
	
	for(int i = 0; i < controlCount; i++)
	{
		// check to see if we should handle repeats
		if(controls[i].attributes & ATTR_REPEAT)
		{
			// increase repeat counter
			controls[i].repeatcount++;
		}
	}
}

void queueHotKeys(u32 hkdown, u32 hkheld)
{
	// Zero out bits we don't want to queue
	hkdown = hkdown & 0xFFF;
	hkheld = hkheld & 0xFFF;
	
	if(hkdown == 0 && hkheld == 0)
	{
		return;
	}
	
	if(!hkQueued)
	{
		hkQueued = true;
		hkD = hkdown;
		hkH = hkheld;
	}
}

bool executeHotKeys(u32 hkdown, u32 hkheld)
{
	if(!controls)
	{
		return false;
	}
	
	for(int i = (controlCount - 1); i >= 0; i--)
	{
		if(controls[i].type == CT_DELETED)
		{
			// Skip this control since it's been deleted
			continue;
		}
		
		// To query later
		tcID = controls[i].controlID;
		
		if(hkdown & controls[i].hotkey1)
		{
			if(controls[i].attributes & ATTR_ENABLED)
			{
				grabCorrectFunction(i, -1, -1, CE_DOWN);
			}
			
			// check to see if we should handle repeats
			if(controls[i].attributes & ATTR_REPEAT)
			{
				// start off
				controls[i].repeatcount = -controls[i].repeatstart;
			}
			
			return true;
		}
		
		if(hkheld & controls[i].hotkey2)
		{
			if(controls[i].attributes & ATTR_ENABLED)
			{			
				grabCorrectFunction(i, -1, -1, CE_MOVE);
			}
			
			// check to see if we should handle repeats
			if(controls[i].attributes & ATTR_REPEAT)
			{
				// increase repeat counter
				controls[i].repeatcount++;
				
				// if we've ticked over
				if(controls[i].repeatcount > controls[i].repeatcontinue)
				{
					// reset count
					controls[i].repeatcount = 0;
					
					// run down button if enabled
					if(controls[i].attributes & ATTR_ENABLED)
					{
						grabCorrectFunction(i, -1, -1, CE_DOWN);
					}
				}
			}
			
			return true;
		}
	}
	
	return false;
}

void drawFilledRect(int x, int y, int bx, int by, uint16 borderColor, uint16 fillColor, uint16 *buffer, int size)
{
	// Draw sides
	drawRect(x, y, bx, y, borderColor, buffer, size);
	drawRect(x, by, bx, by, borderColor, buffer, size);
	drawRect(x, y, x, by, borderColor, buffer, size);
	drawRect(bx, y, bx, by, borderColor, buffer, size);
	
	// Draw center
	drawRect(x + 1, y + 1, bx - 1, by - 1, fillColor, buffer, size);
}

void drawKeyboardInternal(int x, int y, uint16 *bb)
{
	setFont(font_gautami_10);
	setColor(keyboardTextColor);
	
	switch(activeKeyboard())
	{
		case KB_QWERTY:
		{
			drawFilledRect(x+KEYBOARD_LEFT+200, KEYBOARD_TOP+19, x+KEYBOARD_LEFT+228, KEYBOARD_TOP+38, keyboardBorderColor, pressedChar() == BSP ? keyboardHighlightColor : keyboardSpecialColor, bb, 192);
			drawFilledRect(x+KEYBOARD_LEFT+190, KEYBOARD_TOP+38, x+KEYBOARD_LEFT+228, KEYBOARD_TOP+57, keyboardBorderColor, pressedChar() == RET ? keyboardHighlightColor : keyboardSpecialColor, bb, 192);
			drawFilledRect(x+KEYBOARD_LEFT+76, KEYBOARD_TOP+76, x+KEYBOARD_LEFT+171, KEYBOARD_TOP+95, keyboardBorderColor, pressedChar() == SPC ? keyboardHighlightColor : keyboardSpecialColor, bb, 192);
			drawFilledRect(x+KEYBOARD_LEFT+19, KEYBOARD_TOP+76, x+KEYBOARD_LEFT+38, KEYBOARD_TOP+95, keyboardBorderColor, pressedChar() == DEL ? keyboardHighlightColor : keyboardSpecialColor, bb, 192);
			
			drawFilledRect(x+KEYBOARD_LEFT, KEYBOARD_TOP+38, x+KEYBOARD_LEFT+19, KEYBOARD_TOP+57, keyboardBorderColor, isCaps() ? keyboardSpecialHighlightColor : keyboardSpecialColor, bb, 192);
			drawFilledRect(x+KEYBOARD_LEFT, KEYBOARD_TOP+57, x+KEYBOARD_LEFT+29, KEYBOARD_TOP+76, keyboardBorderColor, isShift() ? keyboardSpecialHighlightColor : keyboardSpecialColor, bb, 192);
			drawFilledRect(x+KEYBOARD_LEFT, KEYBOARD_TOP+76, x+KEYBOARD_LEFT+19, KEYBOARD_TOP+95, keyboardBorderColor, isSpecial() ? keyboardSpecialHighlightColor : keyboardSpecialColor, bb, 192);
			
			dispCustomSprite(x+13 + 02, 37 + 24 + 21, k_caps, 31775, 0xFFFF, bb, 255, 191);
			dispCustomSprite(x+13 + 04, 37 + 60 + 23, k_spl, 31775, 0xFFFF, bb, 255, 191);
			dispCustomSprite(x+13 + 05, 37 + 42 + 22, k_shift, 31775, 0xFFFF, bb, 255, 191);
			dispCustomSprite(x+13 + 23, 37 + 60 + 23, k_del, 31775, 0xFFFF, bb, 255, 191);
			dispCustomSprite(x+13 + 114, 37 + 60 + 23, k_space, 31775, 0xFFFF, bb, 255, 191);
			dispCustomSprite(x+13 + 208, 37 + 5 + 20, k_bsp, 31775, 0xFFFF, bb, 255, 191);
			dispCustomSprite(x+13 + 194, 37 + 22 + 21, k_ret, 31775, 0xFFFF, bb, 255, 191);
			
			char *str;
			bool doIt = false;
			
			if((!isCaps() && !isShift()) || (isCaps() && isShift()))
			{
				
				if(!isSpecial())
				{
					// lowercase
					str = l_lowercase;		
					doIt = true;
				}
				else
				{
					// special 0
					str = l_special0;		
					doIt = true;
				}
			}
			else
			{
				if(!isSpecial())
				{
					// uppercase
					str = l_uppercase;		
					doIt = true;
				}
				else
				{
					// special 1
					str = l_special1;
					doIt = true;
				}
			}
			
			if(doIt)
			{
				int j = 0;
				for(uint16 i=0;i<=9;i++)
				{
					uint16 tColor = keyboardFillColor;
					if(pressedChar() != ' ' && ((!isSpecial() && (pressedChar() == l_lowercase[i+j] || pressedChar() == l_uppercase[i+j])) || (isSpecial() && (pressedChar() == l_special0[i+j] || pressedChar() == l_special1[i+j]))))
					{
						tColor = keyboardHighlightColor;
					}
					
					drawFilledRect(x+KEYBOARD_LEFT+10+(i*19), KEYBOARD_TOP+19, x+KEYBOARD_LEFT+10+(i*19)+19, KEYBOARD_TOP+20+18, keyboardBorderColor, tColor, bb, 192);						
					dispChar(x+13+11+(i*19)+9 - (getCharWidth(str[i+j])/2), 37+18+8, str[i+j], bb, 255, 191);
				}
				
				j = 10;
				for(uint16 i=0;i<=8;i++)
				{
					uint16 tColor = keyboardFillColor;
					if(pressedChar() != ' ' && ((!isSpecial() && (pressedChar() == l_lowercase[i+j] || pressedChar() == l_uppercase[i+j])) || (isSpecial() && (pressedChar() == l_special0[i+j] || pressedChar() == l_special1[i+j]))))
					{
						tColor = keyboardHighlightColor;					
					}
					
					drawFilledRect(x+KEYBOARD_LEFT+19+(i*19), KEYBOARD_TOP+38, x+KEYBOARD_LEFT+19+(i*19)+19, KEYBOARD_TOP+39+18, keyboardBorderColor, tColor, bb, 192);
					dispChar(x+13+21+(i*19)+8 - (getCharWidth(str[i+j])/2), 37+36+9, str[i+j], bb, 255, 191);
				}
				
				j = 19;
				for(uint16 i=0;i<=9;i++)
				{
					uint16 tColor = keyboardFillColor;
					if(pressedChar() != ' ' && ((!isSpecial() && (pressedChar() == l_lowercase[i+j] || pressedChar() == l_uppercase[i+j])) || (isSpecial() && (pressedChar() == l_special0[i+j] || pressedChar() == l_special1[i+j]))))
					{
						tColor = keyboardHighlightColor;					
					}
					
					drawFilledRect(x+KEYBOARD_LEFT+29+(i*19), KEYBOARD_TOP+57, x+KEYBOARD_LEFT+29+(i*19)+19, KEYBOARD_TOP+58+18, keyboardBorderColor, tColor, bb, 192);
					dispChar(x+13+30+(i*19)+9 - (getCharWidth(str[i+j])/2), 37+54+10, str[i+j], bb, 255, 191);
				}
				
				j = 29;
				for(uint16 i=0;i<=1;i++)
				{
					uint16 tColor = keyboardFillColor;
					if(pressedChar() != ' ' && ((!isSpecial() && (pressedChar() == l_lowercase[i+j] || pressedChar() == l_uppercase[i+j])) || (isSpecial() && (pressedChar() == l_special0[i+j] || pressedChar() == l_special1[i+j]))))
					{
						tColor = keyboardHighlightColor;					
					}
					
					drawFilledRect(x+KEYBOARD_LEFT+38+(i*19), KEYBOARD_TOP+76, x+KEYBOARD_LEFT+38+(i*19)+19, KEYBOARD_TOP+77+18, keyboardBorderColor, tColor, bb, 192);
					dispChar(x+13+39+(i*19)+9 - (getCharWidth(str[i+j])/2), 37+72+11, str[i+j], bb, 255, 191);
				}
				
				j = 31;
				for(uint16 i=0;i<=2;i++)
				{
					uint16 tColor = keyboardFillColor;
					if(pressedChar() != ' ' && ((!isSpecial() && (pressedChar() == l_lowercase[i+j] || pressedChar() == l_uppercase[i+j])) || (isSpecial() && (pressedChar() == l_special0[i+j] || pressedChar() == l_special1[i+j]))))
					{
						tColor = keyboardHighlightColor;					
					}
					
					drawFilledRect(x+KEYBOARD_LEFT+171+(i*19), KEYBOARD_TOP+76, x+KEYBOARD_LEFT+171+(i*19)+19, KEYBOARD_TOP+77+18, keyboardBorderColor, tColor, bb, 192);
					dispChar(x+13+171+(i*19)+9 - (getCharWidth(str[i+j])/2), 37+72+11, str[i+j], bb, 255, 191);
				}		
			}
			
			if(!isShift())
			{
				// numbers
				for(uint16 i=0;i<=11;i++)
				{
					uint16 tColor = keyboardFillColor;
					if(pressedChar() != ' ' && (pressedChar() == l_numbers[i] || pressedChar() == l_symbols[i]))
					{
						tColor = keyboardHighlightColor;					
					}
					
					drawFilledRect(x+KEYBOARD_LEFT+(i*19), KEYBOARD_TOP, x+KEYBOARD_LEFT+(i*19)+19, KEYBOARD_TOP+19, keyboardBorderColor, tColor, bb, 192);
					dispChar(x+13+(i*19)+9 - (getCharWidth(l_numbers[i])/2), 37+7, l_numbers[i], bb, 255, 191);
				}
			}
			else
			{
				// symbols	
				for(uint16 i=0;i<=11;i++)
				{
					uint16 tColor = keyboardFillColor;
					if(pressedChar() != ' ' && (pressedChar() == l_numbers[i] || pressedChar() == l_symbols[i]))
					{
						tColor = keyboardHighlightColor;					
					}
					
					drawFilledRect(x+KEYBOARD_LEFT+(i*19), KEYBOARD_TOP, x+KEYBOARD_LEFT+(i*19)+19, KEYBOARD_TOP+19, keyboardBorderColor, tColor, bb, 192);
					dispChar(x+13+(i*19)+9 - (getCharWidth(l_symbols[i])/2), 37+7, l_symbols[i], bb, 255, 191);
				}	
			}
			
			break;
		}
		case KB_MESSAGEASE:
		{
			uint16 tColor[17] = { 	keyboardSpecialColor, keyboardFillColor, keyboardFillColor, keyboardFillColor, keyboardSpecialColor, keyboardSpecialColor, 
									keyboardSpecialColor, keyboardFillColor, keyboardFillColor, keyboardFillColor, keyboardSpecialColor, keyboardSpecialColor, 
									keyboardSpecialColor, keyboardFillColor, keyboardFillColor, keyboardFillColor, keyboardABCSwapColor };
			
			switch(pressedChar())
			{
				case BSP:
					tColor[5] = keyboardHighlightColor;
					break;
				case RET:
					tColor[11] = keyboardHighlightColor;
					break;
				case SPC:
					tColor[10] = keyboardHighlightColor;
					break;
				case DEL:
					tColor[4] = keyboardHighlightColor;
					break;
			}
			
			if(isCaps())
				tColor[0] = keyboardSpecialHighlightColor;
			if(isShift())
				tColor[6] = keyboardSpecialHighlightColor;
			if(isSpecial())
				tColor[12] = keyboardSpecialHighlightColor;
			
			for(int wB = 0;wB < 17; wB++)
			{
				int wbX = wB % 6;
				int wbY = wB / 6;
				
				drawFilledRect(32 + (wbX * 32), 37 + (wbY * 32), 32 + ((wbX + 1) * 32), 37 + ((wbY + 1) * 32), keyboardBorderColor, tColor[wB], bb, 192);
			}
			
			dispCustomSprite(32 + 8, 37 + 13, k_caps, 31775, 0xFFFF, bb, 255, 191);
			dispCustomSprite(32 + 7, 37 + 45, k_shift, 31775, 0xFFFF, bb, 255, 191);
			dispCustomSprite(32 + 10, 37 + 77, k_spl, 31775, 0xFFFF, bb, 255, 191);
			dispCustomSprite(32 + 170, 37 + 12, k_bsp, 31775, 0xFFFF, bb, 255, 191);
			dispCustomSprite(32 + 161, 37 + 43, k_ret, 31775, 0xFFFF, bb, 255, 191);	
			dispCustomSprite(32 + 138, 37 + 13, k_del, 31775, 0xFFFF, bb, 255, 191);
			dispCustomSprite(32 + 134, 37 + 45, k_space, 31775, 0xFFFF, bb, 255, 191);
			
			if(activeView() == ACTIVE_NUMBERS)
				dispString(32 + 133, 37 + 76, "ABC", bb, 1, 0, 0, 255, 191);
			else
				dispString(32 + 135, 37 + 76, "123", bb, 1, 0, 0, 255, 191);
			
			char *str = NULL;
			int adjVal = 0;
			
			switch(activeView())
			{
				case ACTIVE_LOWERCASE:
					str = me_lower;
					adjVal = 1;
					break;
				case ACTIVE_UPPERCASE:
					str = me_upper;
					break;
				case ACTIVE_NUMBERS:
					str = me_number;
					break;
				case ACTIVE_LOWERCASE_SPECIAL:
					str = me_lowerspecial;
					break;
				case ACTIVE_UPPERCASE_SPECIAL:
					str = me_upperspecial;
					break;
			}
			
			int j = 0;
			
			for(int i=0;i<9;i++)
			{
				int tX = i % 3;
				int tY = i / 3;
				
				dispChar(32+49+(tX * 32) - (getCharWidth(str[i+j])/2), 37+12+(tY * 32), str[i+j], bb, 255, 191);
			}
			
			setColor(keyboardMEDSTextColor);
			
			j = 9;
			dispChar(32+63 - (getCharWidth(str[j])), 37+23, str[j], bb, 255, 191);
			j = 10;
			dispChar(32+80 - (getCharWidth(str[j])/2), 37+23, str[j], bb, 255, 191);
			j = 11;
			dispChar(32+99, 37+23, str[j], bb, 255, 191);
			j = 12;
			dispChar(32+63 - (getCharWidth(str[j])), 37+44, str[j], bb, 255, 191);
			
			j = 13;
			for(int i=0;i<9;i++)
			{
				int x = i % 3;
				int y = i / 3;
				int z = 0;
				
				if(y == 1 && adjVal == 1)
					z = 1;
				
				dispChar(32+66+(x * 11) + adjVal, 37+34+(y * 10) - adjVal + z, str[i+j], bb, 255, 191);
			}
			
			j = 22;
			dispChar(32+99, 37+44, str[j], bb, 255, 191);
			j = 23;
			dispChar(32+63 - (getCharWidth(str[j])), 37+65, str[j], bb, 255, 191);
			j = 24;
			dispChar(32+80 - (getCharWidth(str[j])/2), 37+65, str[j], bb, 255, 191);
			j = 25;
			dispChar(32+95 - (getCharWidth(str[j])), 37+76, str[j], bb, 255, 191);
			j = 26;
			dispChar(32+99, 37+65, str[j], bb, 255, 191);
			
			// extra shit that we are gonig to fill in anywhere
			// yea, yea, i know it's messy, so what?
			j = 27;
			dispChar(32+63 - (getCharWidth(str[j])), 37+12, str[j], bb, 255, 191);
			j = 28;
			dispChar(32+49 - (getCharWidth(str[j])/2), 37+23, str[j], bb, 255, 191);
			j = 29;
			dispChar(32+66, 37+12, str[j], bb, 255, 191);
			j = 30;
			dispChar(32+96 - (getCharWidth(str[j])), 37+12, str[j], bb, 255, 191);			
			j = 31;
			dispChar(32+99, 37+12, str[j], bb, 255, 191);
			j = 32;
			dispChar(32+113 - (getCharWidth(str[j])/2), 37+23, str[j], bb, 255, 191);
			j = 33;
			dispChar(32+49 - (getCharWidth(str[j])/2), 37+34, str[j], bb, 255, 191);
			j = 34;
			dispChar(32+113 - (getCharWidth(str[j])/2), 37+34, str[j], bb, 255, 191);			
			j = 35;
			dispChar(32+49 - (getCharWidth(str[j])/2), 37+54, str[j], bb, 255, 191);
			j = 36;
			dispChar(32+113 - (getCharWidth(str[j])/2), 37+54, str[j], bb, 255, 191);			
			j = 37;
			dispChar(32+49 - (getCharWidth(str[j])/2), 37+65, str[j], bb, 255, 191);
			j = 38;
			dispChar(32+113 - (getCharWidth(str[j])/2), 37+65, str[j], bb, 255, 191);			
			j = 39;
			dispChar(32+63 - (getCharWidth(str[j])), 37+76, str[j], bb, 255, 191);		
			j = 40;
			dispChar(32+66, 37+76, str[j], bb, 255, 191);
			j = 41;
			dispChar(32+99, 37+76, str[j], bb, 255, 191);
			j = 42;
			dispChar(32+120, 37+76, str[j], bb, 255, 191);
			
			break;
		}
	}
}

void drawSubKeysInternal(uint16 *bb)
{
	setFont(font_gautami_10);
	setColor(keyboardTextColor);
	
	char *insStr = NULL;
	
	if(isInsert())
	{
		insStr = "Ins";
	}
	else
	{
		insStr = "Ovr";
	}
	
	// draw insert
	drawRect(INS_LEFT, INS_TOP, INS_RIGHT, INS_BOTTOM, keyboardBorderColor, bb, 255);
	drawRect(INS_LEFT + 1, INS_TOP + 1, INS_RIGHT - 1, INS_BOTTOM - 1, keyboardFillColor, bb, 255);
	dispString(centerOnPt(INS_CENTER, insStr, font_gautami_10), INS_TOP + 5, insStr, bb, 1, 0, 0, 255, 191);
	
	// draw clear
	if(getMode() != TEXTEDITOR)
	{
		drawRect(CLEAR_LEFT, CLEAR_TOP, CLEAR_RIGHT, CLEAR_BOTTOM, keyboardBorderColor, bb, 255);
		drawRect(CLEAR_LEFT + 1, CLEAR_TOP + 1, CLEAR_RIGHT - 1, CLEAR_BOTTOM - 1, keyboardFillColor, bb, 255);
		dispString(centerOnPt(CLEAR_CENTER, "Clear", font_gautami_10), CLEAR_TOP + 5, "Clear", bb, 1, 0, 0, 255, 191);
	}
	
	// draw control
	drawRect(CTRL_LEFT, CTRL_TOP, CTRL_RIGHT, CTRL_BOTTOM, keyboardBorderColor, bb, 255);
	
	uint16 tColor = keyboardFillColor;	
	if(isControl())
		tColor = keyboardSpecialHighlightColor;

	drawRect(CTRL_LEFT + 1, CTRL_TOP + 1, CTRL_RIGHT - 1, CTRL_BOTTOM - 1, tColor, bb, 255);
	dispString(centerOnPt(CTRL_CENTER, "Ctrl", font_gautami_10), CTRL_TOP + 5, "Ctrl", bb, 1, 0, 0, 255, 191);
	
	if(isSelect())
	{
		insStr = "Select";
	}
	else
	{
		insStr = "Move";
	}
	
	// draw select
	drawRect(SELECT_LEFT, SELECT_TOP, SELECT_RIGHT, SELECT_BOTTOM, keyboardBorderColor, bb, 255);
	drawRect(SELECT_LEFT + 1, SELECT_TOP + 1, SELECT_RIGHT - 1, SELECT_BOTTOM - 1, keyboardFillColor, bb, 255);
	dispString(centerOnPt(SELECT_CENTER, insStr, font_gautami_10), SELECT_TOP + 5, insStr, bb, 1, 0, 0, 255, 191);
}

void drawDeleteInternal(uint16 *bb)
{
	setFont(font_gautami_10);
	setColor(keyboardTextColor);
	
	drawRect(DELETE_LEFT, DELETE_TOP, DELETE_RIGHT, DELETE_BOTTOM, keyboardBorderColor, bb, 255);
	drawRect(DELETE_LEFT + 1, DELETE_TOP + 1, DELETE_RIGHT - 1, DELETE_BOTTOM - 1, keyboardFillColor, bb, 255);
	dispString(centerOnPt(DELETE_CENTER, "Delete", font_gautami_10), DELETE_TOP + 5, "Delete", bb, 1, 0, 0, 255, 191);
}

void drawScrollBarInternal(int pos, int max, bool isHeld, void (*callBack)(int,int), int x, int y, int width, int height, bool displayHandle)
{
	int bottom = y + height;
	int right = x + width;
	double z;
	
	if(max <= 0)
	{
		pos = 0;
		max = 1;
	}
	
	bg_drawFilledRect(x, y, right, y + SCROLL_ARROW_HEIGHT, widgetBorderColor, widgetFillColor);
	bg_drawFilledRect(x, bottom - SCROLL_ARROW_HEIGHT, right, bottom, widgetBorderColor, widgetFillColor);
	bg_drawFilledRect(x, y + SCROLL_ARROW_HEIGHT + 2, right, bottom - SCROLL_ARROW_HEIGHT - 2, widgetBorderColor, scrollFillColor);

	bg_dispCustomSprite(x + 2, y + 4, up_arrow, 31775, scrollNormalColor);
	bg_dispCustomSprite(x + 2, bottom - SCROLL_ARROW_HEIGHT + 5, down_arrow, 31775, scrollNormalColor);
	
	// mark where we have stuff already

	if(callBack)
	{
		callBack(x,y);
	}
	
	if(!displayHandle)
		return;
	
	int verticalPlay = height - (2 * (SCROLL_ARROW_HEIGHT + 4));
	verticalPlay -= SCROLL_HANDLE_HEIGHT;
	
	z = (double)verticalPlay * (double)pos;
	z = z / (double)(max-1);
	
	int yLoc = y + SCROLL_ARROW_HEIGHT + 4 + (int)z;
	
	if(yLoc + SCROLL_HANDLE_HEIGHT > bottom - SCROLL_ARROW_HEIGHT - 4)
		yLoc = bottom - SCROLL_ARROW_HEIGHT - 4 - SCROLL_HANDLE_HEIGHT;
	
	if(!isHeld)
		bg_drawRect(x + 2, yLoc, right - 2, yLoc + SCROLL_HANDLE_HEIGHT, scrollNormalColor);
	else
		bg_drawRect(x + 2, yLoc, right - 2, yLoc + SCROLL_HANDLE_HEIGHT, scrollHighlightColor);
}

int getScrollBarInternal(int py, int max, int y, int by)
{
	// handle being too far off
	if(py < y)
		return 0;
	if(py > by)
		return max - 1;
	
	// relative position in relation to the top of the scroll area
	int relPos = py - (y + (SCROLL_HANDLE_HEIGHT / 2));
	int verticalPlay = (by - y) - (SCROLL_HANDLE_HEIGHT + 4);
	
	double z = (double)(relPos) * (double)(max-1);
	z = z / (double)(verticalPlay);
	
	if(z < 0)
		return 0;
	if(z > (max - 1))
		return max - 1;
	
	return (int)z;
}

int getCursorFromTouchInternal(int u, int v, int listCur, int listMax, int listLeft, int listTop, int listRight, int listBottom)
{
	int z = 0;
	int y = 0;
	int tc = -1;
	
	if(!(u >= listLeft && u <= listRight))
		return -1;
	
	// metrics
	uint16 numEntries = (((listBottom - listTop) - 4) / LIST_STEP) + 1;
	uint16 lowThreshold = numEntries / 2;
	uint16 highThreshold = lowThreshold + 1;
	
	if(listMax <= numEntries)
	{
		y = listMax;
		z = 0;
	}
	else
	{
		if(listCur < lowThreshold)
			z = 0;
		else if(listCur > listMax - highThreshold)
			z = listMax - numEntries;
		else
			z = listCur - lowThreshold;
		
		y = z + numEntries;
	}
	
	// in case it ever flows over
	if(y > listMax)
		y = listMax;
	
	for(uint16 x=z;x<y;x++)
	{
		if(v >= (listTop + ((x - z) * LIST_STEP)) && v <= ((listTop + LIST_STEP) + ((x - z) * LIST_STEP)))
			tc = x;
	}
	
	if(tc > listMax)
		tc = -1;
	
	return tc;
}

void drawListBoxInternal(int tx, int ty, int bx, int by, int curEntry, int maxEntries, char *noString, void (*callback)(int, int, int))
{
	// make sure we are inside the bounds and set up for defaults
	setFont(font_arial_9);
	setColor(genericTextColor);
	bg_setClipping(tx + 2, ty + 3, bx - 2, by);
	
	if(maxEntries == 0)
	{
		setColor(genericTextColor);
		bg_dispString(0, 0, noString);
		return;
	}	
	
	bg_drawFilledRect(tx, ty, bx, by, listBorderColor, listFillColor);
	
	// temporary locations
	uint16 z = 0;
	uint16 y = 0;
	
	// metrics
	uint16 numEntries = (((by - ty) - 4) / LIST_STEP) + 1;
	uint16 lowThreshold = numEntries / 2;
	uint16 highThreshold = lowThreshold + 1;
	
	if(maxEntries <= numEntries)
	{
		y = maxEntries;
		z = 0;
	}
	else
	{
		if(curEntry < lowThreshold)
			z = 0;
		else if(curEntry > maxEntries - highThreshold)
			z = maxEntries - numEntries;
		else
			z = curEntry - lowThreshold;
		
		y = z + numEntries;
	}
	
	// in case it ever flows over
	if(y > maxEntries)
		y = maxEntries;
	
	for(uint16 x=z;x<y;x++)
	{	
		if(x == curEntry)
			bg_drawRect(tx + 2, (ty + 2) + ((x - z) * LIST_STEP), bx - 2, (ty + 14) + ((x - z) * LIST_STEP), genericHighlightColor);
		
		// set clipping so a lazy callback can just display at 0,0
		bg_setClipping(tx + 2, (ty + 1) + ((x - z) * LIST_STEP), bx - 2, (ty + 15) + ((x - z) * LIST_STEP));
		
		// set up stuff for a generic callback
		setColor(listTextColor);
		callback(x, tx + 2, (ty + 2) + ((x - z) * LIST_STEP));
	}
	
	bg_setDefaultClipping();
}

void setHand(int m)
{
	handMode = m;
}

int getHand()
{
	return handMode;
}

void swapHands()
{
	if(handMode == HAND_LEFT)
	{
		setHand(HAND_RIGHT);
	}
	else
	{
		setHand(HAND_LEFT);
	}
}

void drawRoundedButton(int x, int y, int bx, int by, char *text, bool select, bool highlight)
{
	// draw the sides
	uint16 borderColor = widgetBorderColor;	
	if(select)
	{
		borderColor = textEntryHighlightColor;
	}
	
	bg_drawRect(x + 2, y, bx - 2, y, borderColor);
	bg_drawRect(x + 2, by, bx - 2, by, borderColor);
	bg_drawRect(x, y + 2, x, by - 2, borderColor);
	bg_drawRect(bx, y + 2, bx, by - 2, borderColor);
	
	bg_setPixel(x + 1, y + 1, borderColor);
	bg_setPixel(bx - 1, y + 1, borderColor);
	bg_setPixel(x + 1, by - 1, borderColor);
	bg_setPixel(bx - 1, by - 1, borderColor);
	
	// fill the damn thing
	uint16 fillColor = widgetFillColor;
	if(highlight)
	{
		fillColor = genericHighlightColor;
	}
	
	bg_drawRect(x + 2, y + 1, bx - 2, by - 1, fillColor);
	bg_drawRect(x + 1, y + 2, x + 1, by - 2, fillColor);
	bg_drawRect(bx - 1, y + 2, bx - 1, by - 2, fillColor);
	
	// figure out how many lines the text is
	int *pts = NULL;
	int numPts = 0;
	numPts = getWrapPoints(0, 0, text, x + 2, y + 2, bx - 2, by - 2, &pts, font_gautami_10);
	
	setFont(font_gautami_10);
	setColor(widgetTextColor);
	bg_setClipping(x, y + 2, bx, by - 2);
	
	int firstReturn = -1;
	if(strchr(text, '\n') != NULL)
	{
		firstReturn = strchr(text, '\n') - text;
	}
	
	// put text inside it
	for(int c=0;c<numPts;c++)
	{
		int nextPt = 0;
		if(c == (numPts - 1))
		{
			// last point
			nextPt = strlen(text);
		}
		else
		{
			// grab next
			nextPt = pts[c + 1];
		}
		
		char tmpText[256];
		memset(tmpText, 0, 256);
		memcpy(tmpText, text + pts[c], nextPt - pts[c]);
		
		if(firstReturn != 0 && pts[c] <= firstReturn)
		{
			setBold(true);
		}
		else
		{
			setBold(false);
		}
		
		bg_dispString(centerOnPt(((bx + x) / 2), tmpText, font_gautami_10) - x, ((by + y) / 2) - y - ((numPts * 10) / 2) + (c * 10), tmpText);
	}
	
	free(pts);
	
	bg_setDefaultClipping();
}

void drawTextBox(int x, int y, int bx, int by, const char *text, bool centered, bool selected, bool italics)
{
	// set up graphics options
	setFont(font_gautami_10);
	setColor(textEntryTextColor);
	
	// draw the box and set up the clipping area
	bg_drawFilledRect(x, y, bx, by, selected ? textEntryHighlightColor : widgetBorderColor, textEntryFillColor);	
	bg_setClipping(x, y, bx, by + 3);
	
	// allocate temporary storage for string to be shortened
	char *tText = (char *)safeMalloc(strlen(text) + 4); // enough room for trailing zero and the "..."
	strcpy(tText, text);
	
	// shorten the string if needed to ensure it fits
	abbreviateString(tText, bx - x, font_gautami_10);
	
	setItalics(italics);
	
	// display string with centered setting
	if(centered)
	{
		bg_dispString(centerOnPt((bx - x) >> 1, tText, font_gautami_10), 4, tText);
	}
	else
	{
		bg_dispString(3, 4, tText);
	}
	
	bg_setDefaultClipping();
	setItalics(false);
}
