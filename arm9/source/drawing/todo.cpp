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
#include "todo.h"
#include "address.h"
#include "browser.h"
#include "fatwrapper.h"
#include "colors.h"
#include "filerout.h"
#include "general.h"
#include "globals.h"
#include "graphics.h"
#include "settings.h"
#include "fonts.h"
#include "keyboard.h"
#include "language.h"
#include "controls.h"
#include "help.h"

extern TODO_FILE *todoList;
extern bool autoBullet;

static int editField = -1;
static int todoEntries;

static uint16 *sprList[6] = { onhold, normal, normalsemi, urgent, urgentsemi, finished };

// prototypes
void createTodo();
void createEditTodo();
void editTodoAction(char c);
void drawCurrentTodo();
void drawEditTodo();
void todoListCallback(int pos, int x, int y);
void deleteTodoForward(int x, int y);
void deleteTodoBack(int x, int y);
void saveTodo(TODO_FILE *vf, char *dir);
void editTodoForward(int x, int y);
void editTodoBack(int x, int y);
void addBulletToTodo();
int getMaxCursorTodo();

void loadTodo()
{
	freeTodoList();
	
	todoEntries = populateTodoList(d_todo);
}

void freeTodo()
{
	freeTodoList();
}

int getTodoEntries()
{
	return todoEntries;
}

void initTodo()
{
	iprintf("1");
	// ensure no cursors
	editField = -1;
	
	iprintf("2");
	// load the todo list
	loadTodo();
	
	iprintf("3");
	// create the controls
	createTodo();
	iprintf("4");
}

void todoBack(int x, int y)
{
	editField = 0;
	
	moveCursorAbsolute(addTodo());
	
	clearTodo(&todoList[getCursor()]);
	todoEntries++;
	
	createEditTodo();
}

void todoForward(int x, int y)
{
	editField = 0;
	
	createEditTodo();
}

void deleteTodoForward(int x, int y)
{
	DRAGON_chdir(d_todo);
	DRAGON_remove(todoList[getCursor()].fileName);
	DRAGON_chdir("/");
	
	loadTodo();
	
	deleteControl(CONTROL_LIST);
	addListBox(LIST_LEFT, LIST_TOP, LIST_RIGHT, LIST_BOTTOM, todoEntries, l_notodo, todoListCallback, NULL);
	setControlCallbacks(CONTROL_LIST, todoForward, NULL, NULL);
	
	deleteTodoBack(0,0);
	
	if(getCursor() > 0)
	{
		moveCursorRelative(CURSOR_BACKWARD);
	}
}

void deleteTodoBack(int x, int y)
{
	// Remove old buttons
	deleteControl(CONTROL_FORWARD);
	deleteControl(CONTROL_BACKWARD);

	if(todoEntries == 0)
	{
		addDefaultButtons(NULL, NULL, l_create, todoBack);
	}
	else
	{
		addDefaultButtons(l_editselected, todoForward, l_create, todoBack);
	}
	
	setControlEnabled(0, true);
	setControlEnabled(1, true);
}

void deleteEditTodoForward(int x, int y)
{
	DRAGON_chdir(d_todo);
	DRAGON_remove(todoList[getCursor()].fileName);
	DRAGON_chdir("/");
	
	initTodo();
}

void deleteEditTodoBack(int x, int y)
{
	// Remove old buttons
	deleteControl(CONTROL_FORWARD);
	deleteControl(CONTROL_BACKWARD);
	
	addDefaultButtons(l_save, editTodoForward, l_back, editTodoBack);
}

void incrementUrgency()
{
	if(todoList[getCursor()].urgency < FINISHED)
	{
		todoList[getCursor()].urgency++;
	}
}

void decrementUrgency()
{
	if(todoList[getCursor()].urgency > ONHOLD)
	{
		todoList[getCursor()].urgency--;
	}
}

void exitTodo(int x, int y)
{
	freeTodo();
	returnHome();
}

void editTodoForward(int x, int y)
{	
	saveTodo(&todoList[getCursor()], d_todo);	
	sortTodoList(todoEntries);
	
	editField = -1;
	
	createTodo();
}

void editTodoBack(int x, int y)
{
	initTodo();
}

void deleteTodoEntry(int x, int y)
{
	// Remove old buttons
	deleteControl(CONTROL_FORWARD);
	deleteControl(CONTROL_BACKWARD);
	
	addDefaultButtons(l_delete, deleteTodoForward, l_back, deleteTodoBack);
	
	setControlEnabled(0, false);
	setControlEnabled(1, false);
}

void deleteEditTodoEntry()
{
	// Remove old buttons
	deleteControl(CONTROL_FORWARD);
	deleteControl(CONTROL_BACKWARD);
	
	addDefaultButtons(l_delete, deleteEditTodoForward, l_back, deleteEditTodoBack);
}

void todoNextField(int x, int y)
{
	++editField;
	
	clearSelect();
	resetKBCursor();
	
	addBulletToTodo();
	
	setControlEnabled(CONTROL_L, true);
	setControlEnabled(CONTROL_R, false);
}

void todoPrevField(int x, int y)
{
	--editField;
	
	clearSelect();
	resetKBCursor();
	
	setControlEnabled(CONTROL_R, true);
	setControlEnabled(CONTROL_L, false);
}

void todoMoveCursor(int x, int y)
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
			if(getCursor() < (todoEntries - 1))
			{
				moveCursorRelative(1);
			}
			break;
		case 2: // left button
			moveCursorRelative(-8);
			break;
		case 3: // right button
			moveCursorRelative(8);
			
			if(getCursor() >= todoEntries)
			{
				moveCursorAbsolute(todoEntries - 1);
			}
			break;
		case 4: // edit up button
			if(editField == 0)
			{
				incrementUrgency();
			}
			else
			{
				moveKBCursorAbsolute(getPositionAbove());
			}
			
			break;
		case 5: // edit down button
			if(editField == 0)
			{
				decrementUrgency();
			}
			else
			{
				moveKBCursorAbsolute(getPositionBelow());
			}
			
			break;
		case 6: // edit left button
			if(getKBCursor() > 0)
			{
				moveKBCursorRelative(CURSOR_BACKWARD);
			}
			break;
		case 7: // edit right button
			if(getKBCursor() < getMaxCursorTodo())
			{
				moveKBCursorRelative(CURSOR_FORWARD);	
			}
			break;
	}
}

void drawEditTodoScreen()
{
	drawCurrentTodo();
	drawEditTodo();
}

void createTodo()
{
	clearControls();
	
	registerScreenUpdate(drawCurrentTodo, NULL);
	registerHelpScreen("todo.html", true);
	
	addHome(l_home, exitTodo);
	
	if(todoEntries == 0)
	{
		addDefaultButtons(NULL, NULL, l_create, todoBack);
	}
	else
	{
		addDefaultButtons(l_editselected, todoForward, l_create, todoBack);
	}
	
	addHotKey(8, KEY_X, deleteTodoEntry, 0, NULL);
	addListBox(LIST_LEFT, LIST_TOP, LIST_RIGHT, LIST_BOTTOM, todoEntries, l_notodo, todoListCallback, NULL);
	setControlCallbacks(CONTROL_LIST, todoForward, NULL, NULL);
	
	// for dpad
	addHotKey(0, KEY_UP, todoMoveCursor, KEY_UP, NULL);
	addHotKey(1, KEY_DOWN, todoMoveCursor, KEY_DOWN, NULL);
	addHotKey(2, KEY_LEFT, todoMoveCursor, KEY_LEFT, NULL);
	addHotKey(3, KEY_RIGHT, todoMoveCursor, KEY_RIGHT, NULL);
	
	setControlRepeat(0, 15, 4);
	setControlRepeat(1, 15, 4);
	setControlRepeat(2, 15, 4);
	setControlRepeat(3, 15, 4);
}

void createEditTodo()
{
	resetKBCursor();
	clearControls();
	
	registerScreenUpdate(drawEditTodoScreen, NULL);
	registerHelpScreen("edittodo.html", true);
	
	addHome(l_swap, toggleKeyboard);
	addDefaultButtons(l_save, editTodoForward, l_back, editTodoBack);
	addKeyboard(0, 0, true, strlen(todoList[getCursor()].fileName) > 0, SCREEN_BG, editTodoAction, deleteEditTodoEntry);
	addDefaultLR(l_prevfield, todoPrevField, l_nextfield, todoNextField);
	setControlEnabled(CONTROL_L, false);
	
	// for dpad
	addHotKey(4, KEY_UP, todoMoveCursor, KEY_UP, NULL);
	addHotKey(5, KEY_DOWN, todoMoveCursor, KEY_DOWN, NULL);
	addHotKey(6, KEY_LEFT, todoMoveCursor, KEY_LEFT, NULL);
	addHotKey(7, KEY_RIGHT, todoMoveCursor, KEY_RIGHT, NULL);

	setControlRepeat(4, 15, 4);
	setControlRepeat(5, 15, 4);
	setControlRepeat(6, 15, 4);
	setControlRepeat(7, 15, 4);
}

void addBulletToTodo()
{	
	if(autoBullet && strlen(todoList[getCursor()].message) == 0)
	{
		editTodoAction('-');
		editTodoAction(' ');
		clearPressedChar();
	}
}

void saveTodo(TODO_FILE *vf, char *dir)
{
	DRAGON_chdir("/");
	
	char filename[255];	
	
	DRAGON_chdir(dir);	
	
	if(strlen(vf->fileName) == 0)
	{	// new todo
		
		sprintf(filename, "%s.todo", vf->title);
		if(DRAGON_FileExists(filename) != FE_NONE)
			sprintf(filename, "%s%d%d%d%d%d.todo", vf->title, getDay(), getMonth(), getYear(), getHour(true), getMinute());
		
		safeFileName(filename);
		
		strcpy(vf->fileName,filename);
	}
	else
		strcpy(filename, vf->fileName);
	
	DRAGON_FILE *fFile = DRAGON_fopen(filename, "w");

	uint16 sz;
	
	DRAGON_fputc(vf->urgency, fFile);
	
	sz = strlen(vf->title) + 1; // catch zero based strings
	DRAGON_fputc((char)(sz & 0xFF), fFile);
	DRAGON_fputc((char)((sz >> 8) & 0xFF), fFile);			
	DRAGON_fwrite(vf->title, 1, sz, fFile); // write the entry itself
	
	sz = strlen(vf->message) + 1; // catch zero based strings
	DRAGON_fputc((char)(sz & 0xFF), fFile);
	DRAGON_fputc((char)((sz >> 8) & 0xFF), fFile);			
	DRAGON_fwrite(vf->message, 1, sz, fFile); // write the entry itself
	
	DRAGON_fclose(fFile);
}

void fixGautami()
{
	for(int i=0;i<6;i++)
	{
		font_gautami_10[i+20] = sprList[i];
		((int16 *)font_gautami_10[259])[i+20] = -4;
	}
}

void loadCurrentTodos(char **rStr)
{
	int tmpEntries = populateTodoList(d_todo);
	
	if(tmpEntries == 0)
	{
		freeTodo();
		*rStr = (char *)trackMalloc(1, "tmp todo");
		*rStr[0] = 0;
	}
	else
	{
		*rStr = (char *)trackMalloc(1024, "tmp todo");
		memset(*rStr, 0, 1024);
		
		for(int i=0;i<=2;i++)
		{
			if(i == tmpEntries)
				break;
			
			char str[256];
			sprintf(str, "%c %s\n\v", todoList[i].urgency+20, todoList[i].title);
			strcat(*rStr, str);
		}
		
		freeTodo();
	}
	
	freeTodoList();
}

void dispCursorTodo(int x)
{
	if(x != editField)
	{
		hideCursor();
		clearHighlight();
		return;
	}
	
	setFakeHighlight();
	
	if(blinkOn())
	{
		if(isInsert())
			setCursorProperties(cursorNormalColor, -2, -3, -1);
		else
			setCursorProperties(cursorOverwriteColor, -2, -3, -1);
			
		showCursor();
		setCursorPos(getKBCursor());		
	}
}

int getMaxCursorTodo()
{
	switch(editField)
	{
		case 0:
			return strlen(todoList[getCursor()].title);
			break;
		case 1:
			return strlen(todoList[getCursor()].message);
			break;
	}
	
	return 0;
}

void todoListCallback(int pos, int x, int y)
{
	char str[512];
	
	strcpy(str, todoList[pos].title);
	abbreviateString(str, LIST_WIDTH - 17, font_arial_9);		
	
	bg_dispString(15, 0, str);
	bg_dispSprite(x, y - 1, sprList[(int)todoList[pos].urgency],0);	
}

void drawCurrentTodo()
{	
	uint16 colors[2] = { widgetBorderColor, widgetBorderColor };
	int wTodo = getCursor();
	
	if(editField >= 0)
	{
		colors[editField] = textEntryHighlightColor; 
		wTodo = getCursor();
	}

	fb_setClipping(0,0,255,191);
	setFont(font_gautami_10);
	
	fb_dispSprite(3,12,sprList[(int)todoList[wTodo].urgency],0);
	
	setColor(genericTextColor);
	fb_dispString(18, 3, l_title);	
	fb_drawFilledRect(18, 3 + 8, 252, 3 + 23, colors[0], textEntryFillColor);

	fb_drawFilledRect(3, 30, 252, 188, colors[1], textEntryFillColor);
	
	if(todoEntries == 0)
	{
		return;
	}
	
	// draw data here
	
	setColor(textEntryTextColor);
	
	fb_setClipping(18, 3 + 8, 250, 3 + 26);	
	dispCursorTodo(0);
	fb_dispString(3, 5, todoList[wTodo].title);
	
	fb_setClipping(3, 30, 250, 188);
	dispCursorTodo(1);
	fb_dispString(3, 4, todoList[wTodo].message);
	
	hideCursor();
	clearHighlight();
}

void drawEditTodo()
{	
	setFont(font_arial_11);
	setColor(genericTextColor);
	
	char str[128];
	
	strcpy(str,todoList[getCursor()].title);		
	abbreviateString(str, 230, font_arial_11);	
	bg_dispString(13,20-2,str);
}

void editTodoAction(char c)
{
	if(c == 0)
	{ 
		return;
	}
	
	setPressedChar(c);
	
	if(c == RET)
	{ 
		if(editField == 0)
		{
			todoNextField(0,0);
			
			addBulletToTodo();
			setPressedChar(RET);
			
			return;
		}
	}
	if(c == CAP)
	{
		toggleCaps();
		return;
	}
	
	// figure out which buffer to go to, how big....
	char *buf = 0; // get rid of stupid warning message
	int size = 0;
	
	switch(editField)
	{
		case 0:
			buf = todoList[getCursor()].title;
			size = 40;
			break;
		case 1:
			buf = todoList[getCursor()].message;
			size = 1024;
			break;
	}
	
	size++;
	
	if(c == CLEAR_KEY)
	{
		memset(buf, 0, size);
		resetKBCursor();
		return;
	}		
	
	char *tmpBuffer = (char *)trackMalloc(size + 1,"todo kbd");
	memcpy(tmpBuffer,buf,size + 1);
	int oldCursor = getKBCursor();
	
	genericAction(tmpBuffer, size, c);
	
	int *pts = NULL;
	
	switch(editField)
	{
		case 0:
		{
			int numPts = getWrapPoints(3, 4, tmpBuffer, 18, 3 + 8, 250, 3 + 26, &pts, font_gautami_10);
			free(pts);
			
			if(numPts == 1)
				memcpy(buf,tmpBuffer,size);
			else
				moveKBCursorAbsolute(oldCursor);
			
			break;
		}
		case 1:
		{
			int numPts = getWrapPoints(3, 4, tmpBuffer, 3, 30, 250, 188, &pts, font_gautami_10);
			free(pts);
			
			if(numPts <= 12)
				memcpy(buf,tmpBuffer,size);
			else
				moveKBCursorAbsolute(oldCursor);
			
			break;
		}
	}
	
	trackFree(tmpBuffer);
	
	if(c == RET && autoBullet && getKBCursor() != oldCursor)
	{
		editTodoAction('-');
		editTodoAction(' ');
		setPressedChar(RET);
	}
}

