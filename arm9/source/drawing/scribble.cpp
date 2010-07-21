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
#include <libpicture.h>
#include "scribble.h"
#include "drawtools.h"
#include "address.h"
#include "home.h"
#include "texteditor.h"
#include "pictureviewer.h"
#include "fatwrapper.h"
#include "colors.h"
#include "filerout.h"
#include "browser.h"
#include "general.h"
#include "globals.h"
#include "graphics.h"
#include "settings.h"
#include "fonts.h"
#include "keyboard.h"
#include "language.h"
#include "controls.h"
#include "help.h"
#include "lcdfunctions.h"

extern SCRIBBLE_FILE *scribbleList;

static uint16 customColors[27] = { 	0x0000, 0x4210, 0x0010, 0x0110, 0x0210, 0x0200, 0x4A49, 0x4000, 0x4010,
									0xFFFF, 0x6318, 0x001F, 0x221F, 0x03FF, 0x03F0, 0x7FE0, 0x7C00, 0x7C1F, 
									0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF };

static uint16 *toolSprites[MAX_TOOLS] = { tool_pencil, tool_pen, tool_colorgrab, tool_erasor, tool_floodfill, tool_line, tool_thickline, tool_rect, tool_thickrect, tool_circle, tool_thickcircle, tool_spray, tool_replace, tool_text };

static PICTURE_DATA scribblePicture;
static uint16 *tmpBuffer;
static uint16 curColor = 0;
static uint16 tmpColor;
static uint16 saveColor = 0;
static int scribbleEntries;
static int colorCursor = 0;
static int toolsCursor = 0;
static int curTool = TOOL_PENCIL;
static int queuedTool = -1;
static int lastX = -1;
static int lastY = -1;
static int curX = -1;
static int curY = -1;
static int slidePosition;
static int box_x;
static int box_y;
static char textEntry[SCRIBBLE_LEN];
static bool isFlipped = false;
static bool isText = false;
static bool isHeld = false;
static bool dragging = false;
static bool dragging2 = false;

// prototypes
void createScribble();
void createEditScribble();
void createColorPicker(int x, int y);
void scribbleListCallback(int pos, int x, int y);
void initTools();
void memCpySet(void *dst, void *src, int size);
void drawScribbleScreen();
void drawColorPicker();
void drawToolsScreen();
void saveScribble();
void loadColorPicker();
void destroyColorPicker(bool toSaveColor);
void scribbleDown(int x, int y);
void scribbleMove(int x, int y);
void scribbleUp();
void colorPickerDown(int x, int y);
void colorPickerMove(int x, int y);
void colorPickerUp();
void drawScribbleText();
void editScribbleAction(char c);
void exitScribbleCancel(int x, int y);
void exitScribbleConfirm(int x, int y);

int getScribbleEntries()
{
	return scribbleEntries;
}

void loadScribble()
{
	freeScribbleList();
	scribbleEntries = populateScribbleList(d_scribble);
}

void initScribble()
{
	// Init default image
	initImage(&scribblePicture, 0, 0);
	
	// load the scribble list
	loadScribble();
	
	// create the controls
	createScribble();
	
	// Ensure stuff is filled in
	freeHome();
	loadHome();
}

void freeScribble()
{	
	freeImage(&scribblePicture);
	freeScribbleList();
}

void scribbleCreateForward(int x, int y)
{
	if(!isFlipped)
	{
		isFlipped = true;
		
		fb_setBGColor(0xFFFF);
		createImage(&scribblePicture, 256, 192, 0xFFFF);
		strcpy(scribblePicture.fileName, d_scribble);
		strcat(scribblePicture.fileName, getFileName());
		
		switch(getSaveFormat())
		{
			case SAVE_BMP:
				strcat(scribblePicture.fileName, ".bmp");
				break;
			case SAVE_PNG:
				strcat(scribblePicture.fileName, ".png");
				break;
		}
		
		fb_swapBuffers();
		setDrawBuffer(fb_backBuffer());
		fb_swapBuffers();
		lcdSwap();
		
		initTools();
		
		createEditScribble();
	}
}

void scribbleCreateBack(int x, int y)
{
	initScribble();
}

void scribbleForward(int x, int y)
{
	createEditScribble();
}

void scribbleBack(int x, int y)
{	
	createRenameScreen("", scribbleCreateForward, scribbleCreateBack);
}

void deleteScribbleForward(int x, int y)
{
	DRAGON_remove(scribbleList[getCursor()].fileName);
	
	initScribble();
}

void deleteScribbleBack(int x, int y)
{
	// Remove old buttons
	deleteControl(CONTROL_FORWARD);
	deleteControl(CONTROL_BACKWARD);
	
	if(scribbleEntries == 0)
	{
		addDefaultButtons(NULL, NULL, l_create, scribbleBack);	
	}
	else
	{
		addDefaultButtons(l_editselected, scribbleForward, l_create, scribbleBack);
	}
}

void editScribbleForward(int x, int y)
{
	saveScribble();
	saveSettings(); // to save the custom color data
	
	freeScribble();
	initScribble();
}

void editScribbleBack(int x, int y)
{
	fb_setBGColor(genericFillColor);
	saveSettings(); // to save the custom color data
	
	freeScribble();
	initScribble();
}

void colorPickerForward(int x, int y)
{
	destroyColorPicker(true);
	createEditScribble();
}

void colorPickerBack(int x, int y)
{
	destroyColorPicker(false);
	createEditScribble();
}

void exitScribble(int x, int y)
{
	freeScribble();
	returnHome();
}

void deleteScribbleEntry(int x, int y)
{
	// Remove old buttons
	deleteControl(CONTROL_FORWARD);
	deleteControl(CONTROL_BACKWARD);
	
	addDefaultButtons(l_delete, deleteScribbleForward, l_back, deleteScribbleBack);
}

void scribbleMoveCursor(int x, int y)
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
			if(getCursor() < (scribbleEntries - 1))
			{
				moveCursorRelative(1);
			}
			break;
		case 2: // left button
			moveCursorRelative(-8);
			break;
		case 3: // right button
			moveCursorRelative(8);
			
			if(getCursor() >= scribbleEntries)
			{
				moveCursorAbsolute(scribbleEntries - 1);
			}
			break;
	}
}

void editScribbleMoveCursor(int x, int y)
{
	switch(getControlID())
	{		
		case CURSOR_UP:
		{
			int tCursor = toolsCursor-9;
			
			if(tCursor >= 0)
				toolsCursor = tCursor;
			return;
		}
		case CURSOR_DOWN:
		{
			int tCursor = toolsCursor+9;
			
			if(tCursor < MAX_TOOLS)
				toolsCursor = tCursor;
			return;
		}
		case CURSOR_LEFT:
			if(toolsCursor > 0)
				toolsCursor--;
			return;
		case CURSOR_RIGHT:
			if(toolsCursor < MAX_TOOLS-1)
				toolsCursor++;
			return;
	}
	
	switch(getControlID() - 4)
	{	
		case CURSOR_UP:
			if(colorCursor >= 9)
				colorCursor-=9;
			break;
		case CURSOR_DOWN:
			if(colorCursor < 27-9)
				colorCursor+=9;
			break;
		case CURSOR_LEFT:
			if(colorCursor > 0)
				colorCursor--;
			break;
		case CURSOR_RIGHT:
			if(colorCursor < 27-1)
				colorCursor++;
			break;
	}
	
	curColor = customColors[colorCursor] | BIT(15);
}

void drawEditScribbleScreen()
{
	drawScribbleScreen();
	drawToolsScreen();
}

void drawColorPickerScreen()
{
	drawColorPicker();
	drawToolsScreen();
}

void createScribble()
{
	if(isFlipped)
	{
		lcdSwap();
		isFlipped = false;
	}
	
	clearControls();
	
	registerScreenUpdate(drawSplash, NULL);
	registerHelpScreen("scribble.html", true);
	
	addHome(l_home, exitScribble);
	
	if(scribbleEntries == 0)
	{
		addDefaultButtons(NULL, NULL, l_create, scribbleBack);	
	}
	else
	{
		addDefaultButtons(l_editselected, scribbleForward, l_create, scribbleBack);
	}
	
	addHotKey(8, KEY_X, deleteScribbleEntry, 0, NULL);
	addListBox(LIST_LEFT, LIST_TOP, LIST_RIGHT, LIST_BOTTOM, scribbleEntries, l_noscribble, scribbleListCallback, NULL);
	setControlCallbacks(CONTROL_LIST, scribbleForward, NULL, NULL);
	
	// for dpad
	addHotKey(0, KEY_UP, scribbleMoveCursor, KEY_UP, NULL);
	addHotKey(1, KEY_DOWN, scribbleMoveCursor, KEY_DOWN, NULL);
	addHotKey(2, KEY_LEFT, scribbleMoveCursor, KEY_LEFT, NULL);
	addHotKey(3, KEY_RIGHT, scribbleMoveCursor, KEY_RIGHT, NULL);
	
	setControlRepeat(0, 15, 4);
	setControlRepeat(1, 15, 4);
	setControlRepeat(2, 15, 4);
	setControlRepeat(3, 15, 4);
}

void createEditScribble()
{
	if(!isFlipped)
	{
		isFlipped = true;
		
		setBGColor(0xFFFF, &scribblePicture);
		setLargeDimensions(256,192); // normal thumbnail
		loadImage(scribbleList[getCursor()].fileName, &scribblePicture, 0, 0);
		memCpySet(fb_backBuffer(),scribblePicture.picData+2, 256*192*2);
		setDrawBuffer(fb_backBuffer());
		fb_swapBuffers();
		
		memset(fb_backBuffer(), 0xFF, 256*192*2);
		
		lcdSwap();
		initTools();
	}
	
	clearControls();
	
	registerScreenUpdate(drawEditScribbleScreen, enableFBSwap);
	registerHelpScreen("editscribble.html", false);
	
	// Set up custom left and right buttons
	newControl(CONTROL_FORWARD, O0_FORWARDLEFT, O0_TOP, O0_FORWARDRIGHT, O0_BOTTOM, CT_BUTTON, l_save);
	newControl(CONTROL_BACKWARD, O0_BACKLEFT, O0_TOP, O0_BACKRIGHT, O0_BOTTOM, CT_BUTTON, l_back);
	
	setControlCallbacks(CONTROL_FORWARD, NULL, NULL, NULL);
	setControlCallbacks(CONTROL_BACKWARD, NULL, NULL, NULL);
	
	setControlState(CONTROL_FORWARD, BUTTON_L);
	setControlState(CONTROL_BACKWARD, BUTTON_R);
	
	setControlEnabled(CONTROL_FORWARD, false);
	setControlEnabled(CONTROL_BACKWARD, false);
	
	addHotKey(0xF0, KEY_L, editScribbleForward, 0, NULL);
	addHotKey(0xF1, KEY_R, editScribbleBack, 0, NULL);
	
	// for moving controls
	addHotKey(CURSOR_UP, KEY_UP, editScribbleMoveCursor, 0, NULL);
	addHotKey(CURSOR_DOWN, KEY_DOWN, editScribbleMoveCursor, 0, NULL);
	addHotKey(CURSOR_LEFT, KEY_LEFT, editScribbleMoveCursor, 0, NULL);
	addHotKey(CURSOR_RIGHT, KEY_RIGHT, editScribbleMoveCursor, 0, NULL);
	
	addHotKey(CURSOR_UP + 4, KEY_X, editScribbleMoveCursor, 0, NULL);
	addHotKey(CURSOR_DOWN + 4, KEY_B, editScribbleMoveCursor, 0, NULL);
	addHotKey(CURSOR_LEFT + 4, KEY_Y, editScribbleMoveCursor, 0, NULL);
	addHotKey(CURSOR_RIGHT + 4, KEY_A, editScribbleMoveCursor, 0, NULL);
	
	// for color picker
	addHotKey(0xF2, KEY_START, createColorPicker, 0, NULL);
	
	newControl(0xFF, 0, 0, 255, 191, CT_HITBOX, NULL);
	setControlCallbacks(0xFF, scribbleDown, scribbleMove, scribbleUp);
	
	disableFBSwap();
	
	isText = false;
}

void createColorPicker(int x, int y)
{
	if(colorCursor < 18) // don't edit pallate
	{
		return;
	}
	
	loadColorPicker();
	
	clearControls();
	
	registerScreenUpdate(drawColorPickerScreen, NULL);
	registerHelpScreen("color.html", false);
	
	addDefaultButtons(l_confirm, colorPickerForward, l_back, colorPickerBack);	
	
	newControl(0xFF, 0, 0, 255, 191, CT_HITBOX, NULL);
	setControlCallbacks(0xFF, colorPickerDown, colorPickerMove, colorPickerUp);
}

void createScribbleText()
{
	resetKBCursor();
	clearControls();
	
	registerScreenUpdate(drawScribbleText, NULL);
	registerHelpScreen("edit.html", true);
	
	addHome(l_swap, toggleKeyboard);
	addDefaultButtons(l_confirm, exitScribbleConfirm, l_back, exitScribbleCancel);
	addKeyboard(0, 0, true, false, SCREEN_BG, editScribbleAction, NULL);
	
	isText = true;
}

uint16 *getCustomColors()
{
	return customColors;
}

void scribbleListCallback(int pos, int x, int y)
{
	char str[256];
	
	strcpy(str, scribbleList[pos].shortName);
	abbreviateString(str, LIST_WIDTH - 15, font_arial_9);		
	
	bg_dispString(15, 0, str);
	bg_drawRect(x + 3, y + 3, x + 8, y + 8, listTextColor);
}

void initTools()
{
	colorCursor = 0;
	curColor = 0 | BIT(15);
	toolsCursor = 0;
	curTool = TOOL_PENCIL;
	lastX = -1;
	lastY = -1;
	setPenWidth(1);
}

void memCpySet(void *dst, void *src, int size)
{
	uint16 *tDst = (uint16 *)dst;
	uint16 *tSrc = (uint16 *)src;
	
	for(int i = 0; i < (size >> 1); i++)
	{
		tDst[i] = tSrc[i] | BIT(15);
	}
}

void drawScribbleScreen()
{
	if(queuedTool != -1) // for tools that need preview
	{
		memCpySet(getDrawBuffer(), fb_backBuffer(), 256*192*2);
		
		switch(queuedTool)
		{
			case TOOL_LINE:
				setPenWidth(1);
				s_drawLine(lastX, lastY, curX, curY, curColor);
				break;				
			case TOOL_THICKLINE:
				setPenWidth(3);
				s_drawLine(lastX, lastY, curX, curY, curColor);
				break;
			case TOOL_RECT:
				setPenWidth(1);
				s_drawLine(lastX, lastY, lastX, curY, curColor);
				s_drawLine(lastX, lastY, curX, lastY, curColor);
				s_drawLine(curX, lastY, curX, curY, curColor);
				s_drawLine(lastX, curY, curX, curY, curColor);
				break;
			case TOOL_THICKRECT:
				setPenWidth(3);
				s_drawLine(lastX, lastY, lastX, curY, curColor);
				s_drawLine(lastX, lastY, curX, lastY, curColor);
				s_drawLine(curX, lastY, curX, curY, curColor);
				s_drawLine(lastX, curY, curX, curY, curColor);
				break;						
			case TOOL_CIRCLE:
			{
				setPenWidth(1);
				
				int cX = (curX + lastX) / 2;
				int cY = (curY + lastY) / 2;
				
				int radiusX = 0;
				int radiusY = 0;
				
				if(curX > lastX)
					radiusX = (curX - lastX) / 2;
				else
					radiusX = (lastX - curX) / 2;
				
				if(curY > lastY)
					radiusY = (curY - lastY) / 2;
				else
					radiusY = (lastY - curY) / 2;
					
				if(radiusX == 0 || radiusY == 0)
					s_drawLine(lastX, lastY, curX, curY, curColor);
				else
					s_drawEllipse(cX, cY, radiusX, radiusY, curColor);
				break;					
			}
			case TOOL_THICKCIRCLE:
			{
				setPenWidth(3);
				
				int cX = (curX + lastX) / 2;
				int cY = (curY + lastY) / 2;
				
				int radiusX = 0;
				int radiusY = 0;
				
				if(curX > lastX)
					radiusX = (curX - lastX) / 2;
				else
					radiusX = (lastX - curX) / 2;
				
				if(curY > lastY)
					radiusY = (curY - lastY) / 2;
				else
					radiusY = (lastY - curY) / 2;
					
				if(radiusX == 0 || radiusY == 0)
					s_drawLine(lastX, lastY, curX, curY, curColor);
				else
					s_drawEllipse(cX, cY, radiusX, radiusY, curColor);
				break;
			}
			case TOOL_TEXT:
				setPenWidth(1);
				setDrawCheckered(true);
				
				s_drawLine(lastX, lastY, lastX, curY, 0);
				s_drawLine(lastX, lastY, curX, lastY, 0);
				s_drawLine(curX, lastY, curX, curY, 0);
				s_drawLine(lastX, curY, curX, curY, 0);
				
				setDrawCheckered(false);
				break;
		}
	}
}

void drawToolsScreen()
{	
	setFont(font_arial_9);
	bg_setDefaultClipping();
	setColor(widgetTextColor);
	
	// colors
	
	for(int x=0;x<27;x++)
	{
		int xp = x % 9;
		int yp = x / 9;
		
		if(x == colorCursor)
			bg_drawFilledRect(13 + 26*xp, 90 + 23*yp, 17 + 26*xp + 15, 94 + 23*yp + 15, widgetHighlightColor, widgetFillColor);
		else
			bg_drawFilledRect(13 + 26*xp, 90 + 23*yp, 17 + 26*xp + 15, 94 + 23*yp + 15, widgetBorderColor, widgetFillColor);
		
		bg_drawRect(15 + 26*xp, 92 + 23*yp, 15 + 26*xp + 15, 92 + 23*yp + 15, customColors[x]);
	}
	
	// tools	
	
	for(int x=0;x<MAX_TOOLS;x++)
	{
		int xp = x % 9;
		int yp = x / 9;
		
		if(x == toolsCursor)
			bg_drawFilledRect(13 + 26*xp, 13 + 23*yp, 17 + 26*xp + 15, 17 + 23*yp + 15, widgetHighlightColor, widgetFillColor);
		else
			bg_drawFilledRect(13 + 26*xp, 13 + 23*yp, 17 + 26*xp + 15, 17 + 23*yp + 15, widgetBorderColor, widgetFillColor);
		
		bg_dispSprite(15 + 26*xp, 15 + 23*yp, toolSprites[x], 31775);
	}
}

void drawScribbleText()
{
	int a,b,c,d;
	
	if(lastX > curX)
	{
		a = curX;
		c = lastX;
	}
	else
	{
		c = curX;
		a = lastX;
	}
	
	if(lastY > curY)
	{
		b = curY;
		d = lastY;
	}
	else
	{
		d = curY;
		b = lastY;
	}
	
	a+=3;
	b+=3;
	c-=3;
	d-=3;
	
	setColor(curColor);
	setFont(font_arial_9);
	fb_setClipping(a,b,c,d);
	
	setWrapToBorder();
	
	if(blinkOn())
	{
		if(isInsert())
			setCursorProperties(cursorNormalColor, 2, 0, 0);
		else
			setCursorProperties(cursorOverwriteColor, 2, 0, 0);
			
		showCursor();
		setCursorPos(getKBCursor());		
	}
	
	setFakeHighlight();
	fb_dispString(0,-3,textEntry);
	
	clearHighlight();
	setWrapNormal();
	hideCursor();	
}

void exitScribbleConfirm(int x, int y)
{
	fb_eraseBG();
	fb_swapBuffers();
	setDrawBuffer(fb_backBuffer());
	trackFree(tmpBuffer);
	
	int a,b,c,d;
	
	if(lastX > curX)
	{
		a = curX;
		c = lastX;
	}
	else
	{
		c = curX;
		a = lastX;
	}
	
	if(lastY > curY)
	{
		b = curY;
		d = lastY;
	}
	else
	{
		d = curY;
		b = lastY;
	}
	
	a+=3;
	b+=3;
	c-=3;
	d-=3;	
	
	fb_swapBuffers();	
	
	memCpySet(getDrawBuffer(), tmpBuffer, 256*192*2);
	setColor(curColor);
	setFont(font_arial_9);	
	dispString(0, -3, textEntry, getDrawBuffer(), 1, a, b, c, d);
	
	lcdSwap();
	createEditScribble();
}

void exitScribbleCancel(int x, int y)
{
	fb_eraseBG();
	fb_swapBuffers();
	setDrawBuffer(fb_backBuffer());
	fb_swapBuffers();
	
	memCpySet(getDrawBuffer(), tmpBuffer, 256*192*2);
	trackFree(tmpBuffer);
	
	lcdSwap();
	createEditScribble();
}

void editScribbleAction(char c)
{
	if(c == 0)
	{
		return;
	}
	
	setPressedChar(c);
	
	if(c == CAP)
	{
		toggleCaps();
		return;
	}	
	
	char *tmpBuf = (char *)trackMalloc(1024,"scribble kbd");
	memcpy(tmpBuf,textEntry,1024);
	int oldCursor = getKBCursor();
	
	genericAction(tmpBuf, 1024 - 1, c);
	
	int a,b,e,d;
	
	if(lastX > curX)
	{
		a = curX;
		e = lastX;
	}
	else
	{
		e = curX;
		a = lastX;
	}
	
	if(lastY > curY)
	{
		b = curY;
		d = lastY;
	}
	else
	{
		d = curY;
		b = lastY;
	}
	
	a+=3;
	b+=3;
	e-=3;
	d-=3;
	
	int *pts = NULL;
	int numPts = getWrapPoints(0, -3, tmpBuf, a, b, e, d, &pts, font_arial_9);
	free(pts);
	
	if(numPts <= ((d-b) / 15))
		memcpy(textEntry,tmpBuf,SCRIBBLE_LEN);
	else
		moveKBCursorAbsolute(oldCursor);
	
	trackFree(tmpBuf);
}

void scribbleDown(int x, int y)
{
	if(isHeld)
		return;
	if(x <= 0 || x > 255 || y <= 0 || y > 191)
		return;
	
	curTool = toolsCursor;
	setPenWidth(1);
	queuedTool = -1;
	
	switch(curTool)
	{
		case TOOL_PEN:
			setPenWidth(3);
		case TOOL_PENCIL:
			s_setPixel(x, y, curColor);
			lastX = x;
			lastY = y;
			break;
		case TOOL_COLORGRAB:			
			if(colorCursor >= 18)
				customColors[colorCursor] = getDrawBuffer()[x + (y * 256)] | BIT(15);
			break;
		case TOOL_ERASOR:
			setPenWidth(10);			
			s_setPixel(x, y, 0xFFFF);
			lastX = x;
			lastY = y;
			break;
		case TOOL_FLOODFILL:
			s_floodFill(x, y, curColor, getDrawBuffer()[x + (y * 256)]);
			break;
		case TOOL_TEXT:
			tmpBuffer = (uint16 *)trackMalloc(256*192*2,"draw tmp buffer");	
			memcpy(tmpBuffer, getDrawBuffer(), 256*192*2);	
		case TOOL_LINE:
		case TOOL_THICKLINE:
		case TOOL_RECT:
		case TOOL_THICKRECT:
		case TOOL_CIRCLE:
		case TOOL_THICKCIRCLE:	
			memCpySet(fb_backBuffer(), getDrawBuffer(), 256*192*2);
			queuedTool = curTool;
			curX = x;
			curY = y;
			lastX = x;
			lastY = y;
			break;
		case TOOL_SPRAY:
			s_spray(x, y, 10, curColor);
			break;
		case TOOL_REPLACE:
			uint16 tC = getDrawBuffer()[x + (y * 256)] | BIT(15);
			
			for(int l=0;l<256*192;l++)
			{
				if(getDrawBuffer()[l] == tC)
					getDrawBuffer()[l] = curColor | BIT(15);
			}
			break;
	}
	
	isHeld = true;
}

void scribbleMove(int x, int y)
{
	if(!isHeld)
		return;
	if(x <= 0 || x > 255 || y <= 0 || y > 191)
		return;
	
	switch(curTool)
	{
		case TOOL_PEN:
		case TOOL_PENCIL:
			if(lastX <= 0 || lastY <= 0 || lastX > 255 || lastY > 191)
				s_setPixel(x, y, curColor); // errant lines due to race conditions
			else				
				s_drawLine(lastX, lastY, x, y, curColor);
			
			lastX = x;
			lastY = y;
			break;
		case TOOL_COLORGRAB:			
			if(colorCursor >= 18)
				customColors[colorCursor] = getDrawBuffer()[x + (y * 256)] | BIT(15);
			break;
		case TOOL_ERASOR:
			if(lastX <= 0 || lastY <= 0 || lastX > 255 || lastY > 191)
				s_setPixel(x, y, 0xFFFF); // errant lines due to race conditions
			else				
				s_drawLine(lastX, lastY, x, y, 0xFFFF);
			
			lastX = x;
			lastY = y;
			break;
		case TOOL_TEXT:		
			if(lastX == -1 || lastY == -1)
			{
				tmpBuffer = (uint16 *)trackMalloc(256*192*2,"text tmp buffer");	
				memcpy(tmpBuffer, getDrawBuffer(), 256*192*2);		
			}
		case TOOL_LINE:
		case TOOL_THICKLINE:
		case TOOL_RECT:
		case TOOL_THICKRECT:
		case TOOL_CIRCLE:
		case TOOL_THICKCIRCLE:
			if(lastX == -1 || lastY == -1)
			{
				memCpySet(fb_backBuffer(), getDrawBuffer(), 256*192*2);
				queuedTool = curTool;
				lastX = x;
				lastY = y;
			}
			curX = x;
			curY = y;
			break;
		case TOOL_SPRAY:
			s_spray(x, y, 10, curColor);
			break;
	}
}

void scribbleUp()
{
	if(!isHeld)
		return;
	
	switch(curTool)
	{
		case TOOL_PEN:
		case TOOL_PENCIL:
		case TOOL_ERASOR:
			lastX = -1;
			lastY = -1;
			break;
		case TOOL_LINE:
		case TOOL_THICKLINE:
		case TOOL_RECT:
		case TOOL_THICKRECT:
		case TOOL_CIRCLE:
		case TOOL_THICKCIRCLE:
			memset(fb_backBuffer(), 0xFF, 256*192*2);
			lastX = -1;
			lastY = -1;
			curX = -1;
			curY = -1;
			queuedTool = -1;
			break;			
		case TOOL_TEXT:
			if(abs(curX - lastX) < 20 || abs(curY - lastY) < 20) // no shenanigans
				return;	
			
			createScribbleText();
			bg_swapBuffers();
			queuedTool = -1;
			
			lcdSwap();
			
			memset(textEntry,0,SCRIBBLE_LEN);
			
			fb_setBG(getDrawBuffer());
			memset(fb_backBuffer(), 0xFF, 256*192*2);			
			break;
	}
	
	isHeld = false;
}

void saveScribble()
{
	memCpySet(scribblePicture.picData + 2, getDrawBuffer(), 256*192*2);
	
	if(isFlipped)
	{
		lcdSwap();
		isFlipped = false;
	}
	
	DRAGON_chdir("/");
	
	enableLoading(l_saving);
	saveImageFromHandle(&scribblePicture);
	disableLoading();
	
	fb_setBGColor(genericFillColor);
	fb_swapBuffers();
}

void moveTool(int action)
{	
	switch(action)
	{
		case CURSOR_UP:
			moveKBCursorAbsolute(getPositionAbove());
			break;
		case CURSOR_DOWN:
			moveKBCursorAbsolute(getPositionBelow());
			break;
		case CURSOR_LEFT:
			if(getKBCursor() > 0)
				moveKBCursorRelative(CURSOR_BACKWARD);
			break;
		case CURSOR_RIGHT:
			if(getKBCursor() < (int)strlen(textEntry))
				moveKBCursorRelative(CURSOR_FORWARD);
			break;
	}
}

void loadColorPicker()
{
	memCpySet(scribblePicture.picData + 2, getDrawBuffer(), 256*192*2); // save current scribble
	saveColor = customColors[colorCursor] | BIT(15);
	slidePosition = 31*3;
	dragging = false;
	dragging2 = false;
	
	box_x = 3;
	box_y = 3;
	
	drawChooser(3, 3, fb_backBuffer());
	
	uint16 *bg = fb_backBuffer();
	tmpColor = bg[3 + (3 * 256)] | BIT(15);
	
	fb_setBG(fb_backBuffer());
}

void drawColorPicker()
{
	customColors[colorCursor] = drawSlider(200, 3, tmpColor, slidePosition, fb_backBuffer()) | BIT(15);	
	
	fb_drawRect(195, slidePosition + 3, 198, slidePosition + 3, 0x0);
	fb_drawRect(212, slidePosition + 3, 215, slidePosition + 3, 0x0);
}

void colorPickerDown(int x, int y)
{
	if((x >= 3 && x <= (3 + 31 * 6)) && (y >= 3 && y <= (3 + 31 * 6)))
	{
		uint16 *bg = fb_backBuffer();
		
		dragging = true;
		tmpColor = bg[x + (y * 256)] | BIT(15);
		
		box_x = x;
		box_y = y;
	}
	
	if((x >= 200 && x <= 210) && (y >= 3 && y <= (3 + 31 * 6)))
	{
		dragging2 = true;
		slidePosition = y - 3;
	}
}

void colorPickerMove(int x, int y)
{
	if(dragging)
	{	
		if(x < 3)
			x = 3;
		if(x >= (3 + 31 * 6))
			x = (3 + 31 * 6) - 1;
			
		if(y < 3)
			y = 3;
		if(y >= (3 + 31 * 6))
			y = (3 + 31 * 6) - 1;
		
		uint16 *bg = fb_backBuffer();
		
		tmpColor = bg[x + (y * 256)] | BIT(15);
		
		box_x = x;
		box_y = y;
	}
	
	if(dragging2)
	{
		dragging2 = true;
		slidePosition = y - 3;
		
		if(slidePosition < 0)
			slidePosition = 0;
		if(slidePosition >= (31 * 6))
			slidePosition = (31 * 6) - 1;				
	}
}

void colorPickerUp()
{
	dragging = false;
	dragging2 = false;
}

void destroyColorPicker(bool toSaveColor)
{
	if(!toSaveColor)
		customColors[colorCursor] = saveColor | BIT(15);
	curColor = customColors[colorCursor] | BIT(15);
	
	fb_eraseBG();
	
	fb_swapBuffers();
	fb_dispSprite(0,0,scribblePicture.picData, genericFillColor);
	setDrawBuffer(fb_backBuffer());
	fb_swapBuffers();
}

bool screensFlipped()
{
	if(isText)
		return !isFlipped;
	
	return isFlipped;
}
