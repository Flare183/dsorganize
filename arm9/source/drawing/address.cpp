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
#include <vcard.h>
#include "address.h"
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

extern VCARD_FILE *vCardList;

static int editField = -1;
static int addressEntries;

// prototypes
void createAddress();
void createEditAddress();
void drawCurrentAddress();
void editAddressAction(char c);
void addressListCallback(int pos, int x, int y);
void deleteAddressBack(int x, int y);
bool isNumbers(char *str);
void formatCell(int eF);
void drawEditAddress();
int getMaxCursor();

void dispCursor(int x)
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

void loadAddressList()
{
	freeVCard();
	
	separateMultiples(d_vcard);
	addressEntries = populateVCard(d_vcard);
}

void initAddress()
{
	// ensure no cursors
	editField = -1;
	
	// load the address list
	loadAddressList();
	
	// create the controls
	createAddress();
}

void addressForward(int x, int y)
{	
	editField = 0;
	
	resetKBCursor();
	createEditAddress();
}

void addressBack(int x, int y)
{	
	editField = 0;
	moveCursorAbsolute(addressEntries);
	
	moveCursorAbsolute(addVCard());
	resetKBCursor();
	
	clearVCard(&vCardList[getCursor()]);
	addressEntries++;
	
	createEditAddress();
}

void editAddressForward(int x, int y)
{
	saveVCard(&vCardList[getCursor()], d_vcard);
	
	sortVCard(addressEntries);
	
	editField = -1;	
	createAddress();
}

void editAddressBack(int x, int y)
{
	initAddress();
}

void deleteEditAddressForward(int x, int y)
{
	DRAGON_chdir(d_vcard);
	DRAGON_remove(vCardList[getCursor()].fileName);
	DRAGON_chdir("/");
	
	initAddress();
}

void deleteEditAddressBack(int x, int y)
{
	// Remove old buttons
	deleteControl(CONTROL_FORWARD);
	deleteControl(CONTROL_BACKWARD);
	
	addDefaultButtons(l_save, editAddressForward, l_back, editAddressBack);
}

void deleteAddressForward(int x, int y)
{
	DRAGON_chdir(d_vcard);
	DRAGON_remove(vCardList[getCursor()].fileName);
	DRAGON_chdir("/");
	
	loadAddressList();
	
	deleteControl(CONTROL_LIST);
	addListBox(LIST_LEFT, LIST_TOP, LIST_RIGHT, LIST_BOTTOM, addressEntries, l_novcard, addressListCallback, NULL);
	setControlCallbacks(CONTROL_LIST, addressForward, NULL, NULL);
	
	deleteAddressBack(0,0);
	
	if(getCursor() > 0)
	{
		moveCursorRelative(CURSOR_BACKWARD);
	}
}

void deleteAddressBack(int x, int y)
{
	// Remove old buttons
	deleteControl(CONTROL_FORWARD);
	deleteControl(CONTROL_BACKWARD);
	
	if(addressEntries == 0)
	{
		addDefaultButtons(NULL, NULL, l_create, addressBack);
	}
	else
	{
		addDefaultButtons(l_editselected, addressForward, l_create, addressBack);
	}
	
	setControlEnabled(0, true);
	setControlEnabled(1, true);
}

void addressPrevField(int x, int y)
{
	formatCell(editField);
	
	--editField;
	
	resetKBCursor();
	clearSelect();
	
	if(editField == 0)
	{
		setControlEnabled(CONTROL_L, false);
	}
	else
	{
		setControlEnabled(CONTROL_L, true);
	}
	
	setControlEnabled(CONTROL_R, true);
}

void addressNextField(int x, int y)
{
	formatCell(editField);
	
	++editField;
	
	clearSelect();
	resetKBCursor();
	
	if(editField == 10)
	{
		setControlEnabled(CONTROL_R, false);
	}
	else
	{
		setControlEnabled(CONTROL_R, true);
	}
	
	setControlEnabled(CONTROL_L, true);
}

void exitAddress(int x, int y)
{
	freeVCard();
	returnHome();
}

void drawEditAddressScreen()
{
	drawCurrentAddress();
	drawEditAddress();
}

void deleteEditAddressEntry()
{
	// Remove old buttons
	deleteControl(CONTROL_FORWARD);
	deleteControl(CONTROL_BACKWARD);
	
	addDefaultButtons(l_delete, deleteEditAddressForward, l_back, deleteEditAddressBack);
}

void deleteAddressEntry(int x, int y)
{
	// Remove old buttons
	deleteControl(CONTROL_FORWARD);
	deleteControl(CONTROL_BACKWARD);
	
	addDefaultButtons(l_delete, deleteAddressForward, l_back, deleteAddressBack);
	setControlEnabled(0, false);
	setControlEnabled(1, false);
}

void addressMoveCursor(int x, int y)
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
			if(getCursor() < (addressEntries - 1))
			{
				moveCursorRelative(1);
			}
			break;
		case 2: // left button
			moveCursorRelative(-8);
			break;
		case 3: // right button
			moveCursorRelative(8);
			
			if(getCursor() >= addressEntries)
			{
				moveCursorAbsolute(addressEntries - 1);
			}
			break;
		case 6: // edit left button
			if(getKBCursor() > 0)
			{
				moveKBCursorRelative(CURSOR_BACKWARD);
			}
			break;
		case 7: // edit right button
			if(getKBCursor() < getMaxCursor())
			{
				moveKBCursorRelative(CURSOR_FORWARD);	
			}
			break;
	}
}

void createAddress()
{
	clearControls();
	
	registerScreenUpdate(drawCurrentAddress, NULL);
	registerHelpScreen("address.html", true);
	
	addHome(l_home, exitAddress);
	
	if(addressEntries == 0)
	{
		addDefaultButtons(NULL, NULL, l_create, addressBack);
	}
	else
	{
		addDefaultButtons(l_editselected, addressForward, l_create, addressBack);
	}
	
	addHotKey(8, KEY_X, deleteAddressEntry, 0, NULL);
	addListBox(LIST_LEFT, LIST_TOP, LIST_RIGHT, LIST_BOTTOM, addressEntries, l_novcard, addressListCallback, NULL);
	setControlCallbacks(CONTROL_LIST, addressForward, NULL, NULL);
	
	// for dpad
	addHotKey(0, KEY_UP, addressMoveCursor, KEY_UP, NULL);
	addHotKey(1, KEY_DOWN, addressMoveCursor, KEY_DOWN, NULL);
	addHotKey(2, KEY_LEFT, addressMoveCursor, KEY_LEFT, NULL);
	addHotKey(3, KEY_RIGHT, addressMoveCursor, KEY_RIGHT, NULL);
	
	setControlRepeat(0, 15, 4);
	setControlRepeat(1, 15, 4);
	setControlRepeat(2, 15, 4);
	setControlRepeat(3, 15, 4);
}

void createEditAddress()
{
	clearControls();
	
	registerScreenUpdate(drawEditAddressScreen, NULL);
	registerHelpScreen("editaddress.html", true);
	
	addHome(l_swap, toggleKeyboard);
	addDefaultButtons(l_save, editAddressForward, l_back, editAddressBack);
	addKeyboard(0, 0, true, strlen(vCardList[getCursor()].fileName) > 0, SCREEN_BG, editAddressAction, deleteEditAddressEntry);
	addDefaultLR(l_prevfield, addressPrevField, l_nextfield, addressNextField);
	setControlEnabled(CONTROL_L, false);
	
	// for dpad
	addHotKey(6, KEY_LEFT, addressMoveCursor, KEY_LEFT, NULL);
	addHotKey(7, KEY_RIGHT, addressMoveCursor, KEY_RIGHT, NULL);

	setControlRepeat(6, 15, 4);
	setControlRepeat(7, 15, 4);
}

int getMaxCursor()
{
	int whichAddress = 0;
	
	if(vCardList[getCursor()].workAddrPresent == true)
		whichAddress = 1;	
	if(vCardList[getCursor()].homeAddrPresent == true) // default to home address no matter what
		whichAddress = 0;
	
	int whichPhone = 0;
	
	if(vCardList[getCursor()].workPhone[0] != 0)
		whichPhone = 1;
	if(vCardList[getCursor()].homePhone[0] != 0) // default to home phone no matter what
		whichPhone = 0;
	
	switch(editField)
	{
		case 0:
			return strlen(vCardList[getCursor()].firstName);
			break;
		case 1:
			return strlen(vCardList[getCursor()].lastName);
			break;
		case 2:
			return strlen(vCardList[getCursor()].nickName);
			break;
		case 3:
			if(whichPhone == 0)
				return strlen(vCardList[getCursor()].homePhone);
			else
				return strlen(vCardList[getCursor()].workPhone);
			
			break;
		case 4:
			return strlen(vCardList[getCursor()].cellPhone);
			break;
		case 5:
			if(whichAddress == 0)
				return strlen(vCardList[getCursor()].homeAddr.addrLine);
			else
				return strlen(vCardList[getCursor()].workAddr.addrLine);
			
			break;
		case 6:
			if(whichAddress == 0)
				return strlen(vCardList[getCursor()].homeAddr.city);
			else
				return strlen(vCardList[getCursor()].workAddr.city);
			
			break;
		case 7:
			if(whichAddress == 0)
				return strlen(vCardList[getCursor()].homeAddr.state);
			else
				return strlen(vCardList[getCursor()].workAddr.state);
			
			break;
		case 8:
			if(whichAddress == 0)
				return strlen(vCardList[getCursor()].homeAddr.zip);
			else
				return strlen(vCardList[getCursor()].workAddr.zip);
			
			break;
		case 9:
			return strlen(vCardList[getCursor()].email);
			break;
		case 10:
			return strlen(vCardList[getCursor()].comment);
			break;
	}
	return 0;
}

int getAddressEntries()
{
	return addressEntries;
}

void addressListCallback(int pos, int x, int y)
{
	if(!vCardList)
	{
		return;
	}
	
	char str[70];
	
	calculateListName(&vCardList[pos], str);
	abbreviateString(str, LIST_WIDTH - 15, font_arial_9);		
	
	bg_dispString(15, 0, str);
	bg_drawRect(x + 3, y + 3, x + 8, y + 8, listTextColor);
}

void drawCurrentAddress()
{	
	int whichAddress = 0;
	int wAddr = getCursor();
	
	uint16 colors[11] = { widgetBorderColor, widgetBorderColor, widgetBorderColor, widgetBorderColor, widgetBorderColor, widgetBorderColor, widgetBorderColor, widgetBorderColor, widgetBorderColor, widgetBorderColor, widgetBorderColor };
	
	if(editField >= 0)
	{
		colors[editField] = textEntryHighlightColor; 
		wAddr = getCursor();
	}
	
	if(vCardList[wAddr].workAddrPresent == true)
		whichAddress = 1;	
	if(vCardList[wAddr].homeAddrPresent == true) // default to home address no matter what
		whichAddress = 0;
	
	int whichPhone = 0;
	
	if(vCardList[wAddr].workPhone[0] != 0)
		whichPhone = 1;
	if(vCardList[wAddr].homePhone[0] != 0) // default to home phone no matter what
		whichPhone = 0;

	fb_setClipping(0,0,255,191);
	setFont(font_gautami_10);
	setColor(genericTextColor);
	
	fb_dispString(3, 3, l_firstname);
	fb_dispString(131, 3, l_lastname);	
	fb_drawFilledRect(3, 3 + 8, 125, 3 + 23, colors[0], textEntryFillColor);
	fb_drawFilledRect(131, 3 + 8, 252, 3 + 23, colors[1], textEntryFillColor);

	fb_dispString(3, 30, l_nickname);	
	fb_drawFilledRect(3, 30 + 8, 252, 30 + 23, colors[2], textEntryFillColor);
	
	if(whichPhone == 0)
		fb_dispString(3, 57, l_homephone);
	else
		fb_dispString(3, 57, l_workphone);
	
	fb_dispString(131, 57, l_cellphone);
	fb_drawFilledRect(3, 57 + 8, 125, 57 + 23, colors[3], textEntryFillColor);
	fb_drawFilledRect(131, 57+ 8, 252, 57 + 23, colors[4], textEntryFillColor);
	
	fb_dispString(3, 84, l_address);	
	fb_drawFilledRect(3, 84 + 8, 252, 84 + 23, colors[5], textEntryFillColor);
	
	fb_dispString(3, 111, l_city);
	fb_dispString(131, 111, l_state);
	fb_dispString(177, 111, l_zip);
	fb_drawFilledRect(3, 111 + 8, 125, 111 + 23, colors[6], textEntryFillColor);
	fb_drawFilledRect(131, 111 + 8, 171, 111 + 23, colors[7], textEntryFillColor);
	fb_drawFilledRect(177, 111 + 8, 252, 111 + 23, colors[8], textEntryFillColor);
	
	fb_dispString(3, 138, l_email);	
	fb_drawFilledRect(3, 138 + 8, 252, 138 + 23, colors[9], textEntryFillColor);
	
	fb_dispString(3, 165, l_comments);	
	fb_drawFilledRect(3, 165 + 8, 252, 165 + 23, colors[10], textEntryFillColor);	
	
	if(addressEntries == 0)
	{
		return;
	}
	
	// draw data here
	
	setColor(textEntryTextColor);
	
	fb_setClipping(3, 3 + 8, 125, 3 + 26);	
	dispCursor(0);
	fb_dispString(3, 5, vCardList[wAddr].firstName);
	fb_setClipping(131, 3 + 8, 252, 3 + 26);
	dispCursor(1);
	fb_dispString(3, 5, vCardList[wAddr].lastName);
	
	fb_setClipping(3, 30 + 8, 252, 30 + 26);
	dispCursor(2);
	fb_dispString(3, 5, vCardList[wAddr].nickName);
	
	fb_setClipping(3, 57 + 8, 125, 57 + 26);
	dispCursor(3);
	
	if(whichPhone == 0)
		fb_dispString(3, 5, vCardList[wAddr].homePhone);
	else	
		fb_dispString(3, 5, vCardList[wAddr].workPhone);
	
	fb_setClipping(131, 57+ 8, 252, 57 + 26);
	dispCursor(4);
	fb_dispString(3, 5, vCardList[wAddr].cellPhone);
		
	fb_setClipping(3, 84 + 8, 252, 84 + 26);
	dispCursor(5);
	
	if(whichAddress == 0)
		fb_dispString(3, 5, vCardList[wAddr].homeAddr.addrLine);
	else
		fb_dispString(3, 5, vCardList[wAddr].workAddr.addrLine);
	
	fb_setClipping(3, 111 + 8, 125, 111 + 26);
	dispCursor(6);
	
	if(whichAddress == 0)
		fb_dispString(3, 5, vCardList[wAddr].homeAddr.city);
	else
		fb_dispString(3, 5, vCardList[wAddr].workAddr.city);
	
	fb_setClipping(131, 111 + 8, 171, 111 + 26);
	dispCursor(7);
	
	if(whichAddress == 0)
		fb_dispString(3, 5, vCardList[wAddr].homeAddr.state);
	else
		fb_dispString(3, 5, vCardList[wAddr].workAddr.state);
	
	fb_setClipping(177, 111 + 8, 252, 111 + 26);
	dispCursor(8);
	
	if(whichAddress == 0)
		fb_dispString(3, 5, vCardList[wAddr].homeAddr.zip);
	else
		fb_dispString(3, 5, vCardList[wAddr].workAddr.zip);
	
	fb_setClipping(3, 138 + 8, 252, 138 + 26);
	dispCursor(9);
	fb_dispString(3, 5, vCardList[wAddr].email);
	
	fb_setClipping(3, 165 + 8, 252, 165 + 26);
	dispCursor(10);
	fb_dispString(3, 5, vCardList[wAddr].comment);
	hideCursor();
	clearHighlight();
}

void drawEditAddress()
{	
	char str[70];
	
	setFont(font_arial_11);
	setColor(genericTextColor);
	
	calculateListName(&vCardList[getCursor()], str);
	abbreviateString(str, 230, font_arial_11);	
	bg_dispString(13,20-2,str);
}

bool isNumbers(char *str)
{
	while(*str != 0)
	{
		if(*str < '0' || *str > '9')
			return false;
		
		str++;
	}

	return true;
}

void formatCell(int eF)
{
	if(eF == 3)
	{
		char *phone;
		int whichPhone = 0;
		
		if(vCardList[getCursor()].workPhone[0] != 0)
			whichPhone = 1;
		if(vCardList[getCursor()].homePhone[0] != 0) // default to home phone no matter what
			whichPhone = 0;
		
		if(whichPhone == 0)
			phone = vCardList[getCursor()].homePhone;
		else
			phone = vCardList[getCursor()].workPhone;
		
		// applicable, phone numbers!
		if(isNumbers(phone) && strlen(phone) == 7)
		{
			phone[7] = phone[6];
			phone[6] = phone[5];
			phone[5] = phone[4];
			phone[4] = phone[3];
			phone[3] = '-';
			phone[8] = 0;
			
			return;
		}
		
		if(isNumbers(phone) && strlen(phone) == 10)
		{
			phone[13] = phone[9];
			phone[12] = phone[8];
			phone[11] = phone[7];
			phone[10] = phone[6];
			phone[8] = phone[5];
			phone[7] = phone[4];
			phone[6] = phone[3];
			phone[3] = phone[2];
			phone[2] = phone[1];
			phone[1] = phone[0];
			
			phone[0] = '(';
			phone[4] = ')';
			phone[5] = ' ';
			phone[9] = '-';
			phone[14] = 0;
			
			return;
		}
	}
	
	if(eF == 4)
	{
		// applicable, phone numbers!
		if(isNumbers(vCardList[getCursor()].cellPhone) && strlen(vCardList[getCursor()].cellPhone) == 7)
		{
			vCardList[getCursor()].cellPhone[7] = vCardList[getCursor()].cellPhone[6];
			vCardList[getCursor()].cellPhone[6] = vCardList[getCursor()].cellPhone[5];
			vCardList[getCursor()].cellPhone[5] = vCardList[getCursor()].cellPhone[4];
			vCardList[getCursor()].cellPhone[4] = vCardList[getCursor()].cellPhone[3];
			vCardList[getCursor()].cellPhone[3] = '-';
			vCardList[getCursor()].cellPhone[8] = 0;
			
			return;
		}
		
		if(isNumbers(vCardList[getCursor()].cellPhone) && strlen(vCardList[getCursor()].cellPhone) == 10)
		{
			vCardList[getCursor()].cellPhone[13] = vCardList[getCursor()].cellPhone[9];
			vCardList[getCursor()].cellPhone[12] = vCardList[getCursor()].cellPhone[8];
			vCardList[getCursor()].cellPhone[11] = vCardList[getCursor()].cellPhone[7];
			vCardList[getCursor()].cellPhone[10] = vCardList[getCursor()].cellPhone[6];
			vCardList[getCursor()].cellPhone[8] = vCardList[getCursor()].cellPhone[5];
			vCardList[getCursor()].cellPhone[7] = vCardList[getCursor()].cellPhone[4];
			vCardList[getCursor()].cellPhone[6] = vCardList[getCursor()].cellPhone[3];
			vCardList[getCursor()].cellPhone[3] = vCardList[getCursor()].cellPhone[2];
			vCardList[getCursor()].cellPhone[2] = vCardList[getCursor()].cellPhone[1];
			vCardList[getCursor()].cellPhone[1] = vCardList[getCursor()].cellPhone[0];
			
			vCardList[getCursor()].cellPhone[0] = '(';
			vCardList[getCursor()].cellPhone[4] = ')';
			vCardList[getCursor()].cellPhone[5] = ' ';
			vCardList[getCursor()].cellPhone[9] = '-';
			vCardList[getCursor()].cellPhone[14] = 0;
			
			return;
		}
	}
	
	if(eF == 8)
	{
		// applicable, zip code!
		
		char *zip;
		
		int whichAddress = 0;
		
		if(vCardList[getCursor()].workAddrPresent == true)
			whichAddress = 1;	
		if(vCardList[getCursor()].homeAddrPresent == true) // default to home address no matter what
			whichAddress = 0;
		
		if(whichAddress == 0)
			zip = vCardList[getCursor()].homeAddr.zip;
		else
			zip = vCardList[getCursor()].workAddr.zip;
		
		if(isNumbers(zip) && strlen(zip) == 9)
		{
			zip[9] = zip[8];
			zip[8] = zip[7];
			zip[7] = zip[6];
			zip[6] = zip[5];
			zip[5] = '-';
			zip[10] = 0;
		}
	}
}

void editAddressAction(char c)
{
	if(c == 0) return;
	
	setPressedChar(c);
	
	int whichAddress = 0;
	
	if(vCardList[getCursor()].workAddrPresent == true)
		whichAddress = 1;	
	if(vCardList[getCursor()].homeAddrPresent == true) // default to home address no matter what
		whichAddress = 0;
	
	int whichPhone = 0;
	
	if(vCardList[getCursor()].workPhone[0] != 0)
		whichPhone = 1;
	if(vCardList[getCursor()].homePhone[0] != 0) // default to home phone no matter what
		whichPhone = 0;
	
	if(c == RET)
	{
		if(editField < 10)
		{
			addressNextField(0,0);
		}
		return;
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
			buf = vCardList[getCursor()].firstName;
			size = 30;
			break;
		case 1:
			buf = vCardList[getCursor()].lastName;
			size = 30;
			break;
		case 2:
			buf = vCardList[getCursor()].nickName;
			size = 80;
			break;
		case 3:
			if(whichPhone == 0)
				buf = vCardList[getCursor()].homePhone;
			else
				buf = vCardList[getCursor()].workPhone;
			
			size = 14;
			break;
		case 4:
			buf = vCardList[getCursor()].cellPhone;
			size = 14;
			break;
		case 5:
			if(whichAddress == 0)
				buf = vCardList[getCursor()].homeAddr.addrLine;
			else
				buf = vCardList[getCursor()].workAddr.addrLine;
				
			size = 60;
			break;
		case 6:
			if(whichAddress == 0)
				buf = vCardList[getCursor()].homeAddr.city;
			else
				buf = vCardList[getCursor()].workAddr.city;
			
			size = 30;
			break;
		case 7:
			if(whichAddress == 0)
				buf = vCardList[getCursor()].homeAddr.state;
			else
				buf = vCardList[getCursor()].workAddr.state;
			
			size = 3;
			break;
		case 8:
			if(whichAddress == 0)
				buf = vCardList[getCursor()].homeAddr.zip;
			else
				buf = vCardList[getCursor()].workAddr.zip;
			
			size = 10;
			break;
		case 9:
			buf = vCardList[getCursor()].email;
			size = 60;
			break;
		case 10:
			buf = vCardList[getCursor()].comment;
			size = 100;
			break;
	}
	
	if(c == CLEAR_KEY)
	{
		memset(buf, 0, size + 2); // extra byte per entry
		resetKBCursor();
		return;
	}	
	
	char *tmpBuffer = (char *)trackMalloc(size + 1,"address kbd");
	memcpy(tmpBuffer,buf,size + 1);
	int oldCursor = getKBCursor();	
	
	genericAction(tmpBuffer, size, c);
	
	int *pts = NULL;
	int numPts = 0;
	
	switch(editField)
	{
		case 0:
			numPts = getWrapPoints(3, 5, tmpBuffer, 3, 3 + 8, 125, 3 + 26, &pts, font_gautami_10);
			break;
		case 1:
			numPts = getWrapPoints(3, 5, tmpBuffer, 131, 3 + 8, 252, 3 + 26, &pts, font_gautami_10);
			break;
		case 2:
			numPts = getWrapPoints(3, 5, tmpBuffer, 3, 30 + 8, 252, 30 + 26, &pts, font_gautami_10);
			break;
		case 3:
			numPts = getWrapPoints(3, 5, tmpBuffer, 3, 57 + 8, 125, 57 + 26, &pts, font_gautami_10);
			break;
		case 4:
			numPts = getWrapPoints(3, 5, tmpBuffer, 131, 57+ 8, 252, 57 + 26, &pts, font_gautami_10);
			break;
		case 5:
			numPts = getWrapPoints(3, 5, tmpBuffer, 3, 84 + 8, 252, 84 + 26, &pts, font_gautami_10);
			break;
		case 6:
			numPts = getWrapPoints(3, 5, tmpBuffer, 3, 111 + 8, 125, 111 + 26, &pts, font_gautami_10);
			break;
		case 7:
			numPts = getWrapPoints(3, 5, tmpBuffer, 131, 111 + 8, 171, 111 + 26, &pts, font_gautami_10);
			break;
		case 8:
			numPts = getWrapPoints(3, 5, tmpBuffer, 177, 111 + 8, 252, 111 + 26, &pts, font_gautami_10);
			break;
		case 9:
			numPts = getWrapPoints(3, 5, tmpBuffer, 3, 138 + 8, 252, 138 + 26, &pts, font_gautami_10);
			break;
		case 10:
			numPts = getWrapPoints(3, 5, tmpBuffer, 3, 165 + 8, 252, 165 + 26, &pts, font_gautami_10);
			break;
	}
	
	free(pts);
	
	if(numPts == 1)
		memcpy(buf,tmpBuffer,size);
	else
		moveKBCursorAbsolute(oldCursor);
	
	trackFree(tmpBuffer);
}
