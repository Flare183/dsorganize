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
#include <string.h>
#include <libfb/libcommon.h>
#include "keyboard.h"
#include "language.h"
#include "general.h"
#include "colors.h"
#include "controls.h"
#include "clipboard.h"
#include "sound.h"
#include "soundipc.h"
#include "settings.h"

static int downX = 0;
static int downY = 0;
static int upX = 0;
static int upY = 0;
static int whichKeyboard = KB_QWERTY;
static int isNumbers = 0;
static uint16 caps;
static uint16 shift;
static uint16 insert;
static uint16 control;
static uint16 select;
static char curChar;
static uint16 special;
static int cursor = 0;
static int kbCursor = 0;
static uint16 blink = 0;
static uint16 bCount = 0;
static int oldCursor = 0;
static int beginHighlight = -1;
static int endHighlight = -1;

static const int keyboard_Hit[60] = {	0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11,
											0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  -1, -1,
											K_C,10, 11, 12, 13, 14, 15, 16, 17, 18, -1, -1,
											19, 20, 21, 22, 23, 24, 25, 26, 27, 28, -1, -1,
											K_T,K_D,29, 30, -1, -1, -1, -1, -1, 31, 32, 33 };

char me_upper[43] =        { "ANIHORTESVLXKQUPC BGDJMYWZF .?     ,       "   };
char me_lower[43] =        { "anihortesvlxkqupc bgdjmywzf .?     ,       "   };
char me_number[43] =       { "123456789-:/[!@#] {$%^}&*+~`.?()_;',\"=\\<>|0" };
char me_lowerspecial[43] = { "‡·‚„‰ÂËÈÍÎÏÌÓÔÒÚÛ Ùıˆ˘˙˚¸˝˛~ÊÁ            "   };
char me_upperspecial[43] = { "¿¡¬√ƒ≈»… ÀÃÕŒœ—“” ‘’÷Ÿ⁄€‹›–ﬁﬂ∆«            "   };

// returns to get some globals cleaned up

void resetKeyboard()
{
	shift = 0;
	caps = 0;
	special = 0;
	insert = 1;
	control = 0;
	select = 0;
	
	curChar = 0;
}

int activeKeyboard()
{
	return whichKeyboard;
}

uint16 isCaps()
{
	return caps;
}

void toggleCaps()
{
	caps = 1 - caps;
}

uint16 isShift()
{
	return shift;
}

void toggleShift()
{
	shift = 1 - shift;
}

uint16 isInsert()
{
	return insert;
}

void toggleIns()
{
	insert = 1 - insert;
}

uint16 isSpecial()
{
	return special;
}

void toggleSpecial()
{
	special = 1 - special;
}

uint16 isSelect()
{
	return select;
}

void setFakeHighlight()
{
	setFakeHighlightEX(0);
}

void setFakeHighlightEX(int subAmount)
{
	int tBegin = beginHighlight - subAmount;
	int tEnd = endHighlight - subAmount;
	
	if(subAmount != 0)
	{
		if(tBegin < 0)
		{
			tBegin = 0;
		}
		if(tEnd < 0)
		{
			tEnd = 0;
		}
	}
	
	setHighlightColor(genericHighlightColor);
	setHighlight(tBegin, tEnd);
}

void clearFakeHighlight()
{
	beginHighlight = -1;
	endHighlight = -1;
}

void toggleSelect()
{
	select = 1 - select;
	
	if(select == 1)
	{
		// set up selection
		
		beginHighlight = getKBCursor();
		endHighlight = getKBCursor();
	}
	else
	{
		clearFakeHighlight();
	}
}

void clearSelect()
{
	clearFakeHighlight();
	select = 0;
}

uint16 isControl()
{
	return control;
}

void toggleControl()
{
	control = 1 - control;
}

void setPressedChar(char c)
{
	curChar = c;
}

char pressedChar()
{
	return curChar;
}

void clearPressedChar()
{
	curChar = 0;
}

void resetCursor()
{
	cursor = 0;
}

void resetKBCursor()
{
	kbCursor = 0;
	blink = 1;
	bCount = 0;
}

void moveCursorAbsolute(int newPos)
{
	if(newPos >= 0)
		cursor = newPos;
}

void moveKBCursorAbsolute(int newPos)
{
	if(newPos >= 0)
		kbCursor = newPos;
	blink = 1;
	bCount = 0;
	
	if(isSelect())
		endHighlight = kbCursor;
}

int getCursor()
{
	return cursor;
}

int getKBCursor()
{
	return kbCursor;
}

bool blinkOn()
{
	return blink == 1;
}

void moveCursorRelative(int newPos)
{
	cursor += newPos;
	
	if(cursor < 0)
		cursor = 0;
}

void moveKBCursorRelative(int newPos)
{
	kbCursor += newPos;
	
	if(kbCursor < 0)
		kbCursor = 0;
	blink = 1;
	bCount = 0;
	
	if(isSelect())
		endHighlight = kbCursor;
}

void pushCursor()
{
	oldCursor = cursor;
	resetCursor();
}

void popCursor()
{
	cursor = oldCursor;
}

void incrimentBCount()
{
	bCount++;
	if(bCount >= 25)
	{
		bCount = 0;
		blink = 1 - blink;
	}
}

void clickSound()
{
	if(keyClickEnabled())
	{
		SendArm7Command(ARM7COMMAND_PLAYCLICK, 0);
	}
}

int activeView()
{
	if(isNumbers == 1)
		return ACTIVE_NUMBERS;
	else
	{
		if((caps == 0 && shift == 0) || (caps == 1 && shift == 1))
		{
			
			if(special == 0)
			{
				// lowercase
				return ACTIVE_LOWERCASE;
			}
			else
			{
				// special 0
				return ACTIVE_LOWERCASE_SPECIAL;
			}
		}
		else
		{
			if(special == 0)
			{
				// uppercase
				return ACTIVE_UPPERCASE;
			}
			else
			{
				// special 1
				return ACTIVE_UPPERCASE_SPECIAL;
			}
		}
	}
	
	return ACTIVE_NONE;
}

char translateCode(int inCode, int keyBoard)
{
	char c = 0;
	
	if(inCode == -1)
		return 0;
		
	if(inCode >= K_C)	// special keys!
	{
		switch(inCode)
		{
			case K_C:
				return CAP;
				break;
			case K_S:
				return SHF;
				break;
			case K_D:
				return DEL;
				break;
			case K_B:
				return BSP;
				break;
			case K_R:
				return RET;
				break;
			case K_W:
				return SPC;
				break;
			case K_T:
				return SPL;
				break;
		}
		
		return 0;
	}
	
	if(keyBoard == NUMBERS)
	{
		if(inCode > 11)
			return 0;
		
		c = l_numbers[inCode];
	}
		
	if(keyBoard == SYMBOLS)
	{
		if(inCode > 11)
			return 0;
		
		c = l_symbols[inCode];
	}
		
	if(keyBoard == LOWERCASE)
	{
		if(inCode > 33)
			return 0;
		
		c = l_lowercase[inCode];
	}
		
	if(keyBoard == UPPERCASE)
	{
		if(inCode > 33)
			return 0;
		
		c = l_uppercase[inCode];
	}	
		
	if(keyBoard == SPECIAL0)
	{
		if(inCode > 33)
			return 0;
		
		c = l_special0[inCode];
	}
		
	if(keyBoard == SPECIAL1)
	{
		if(inCode > 33)
			return 0;
		
		c = l_special1[inCode];
	}
	
	if(c == 32)
		c = 0;
	
	return c;
}

void toggleKeyboard(int x, int y)
{
	switch(whichKeyboard)
	{
		case KB_QWERTY:
			whichKeyboard = KB_MESSAGEASE;
			break;
		case KB_MESSAGEASE:
			whichKeyboard = KB_QWERTY;
			break;
	}
}

char executeClick(int px, int py)
{
	if(py >= DEFAULT_TOP)
	{
		if(py <= DEFAULT_BOTTOM)
		{
			if(px >= INS_LEFT && px <= INS_RIGHT)
			{
				toggleIns();
			}
			
			if(px >= CTRL_LEFT && px <= CTRL_RIGHT)
			{
				toggleControl();
			}
			
			if(px >= SELECT_LEFT && px <= SELECT_RIGHT)
			{
				toggleSelect();
			}
			
			if(px >= CLEAR_LEFT && px <= CLEAR_RIGHT)	
			{				
				return CLEAR_KEY;
			}
		}
		
		return 0;
	}
	
	if(activeKeyboard() == KB_MESSAGEASE)
	{
		downX = px;
		downY = py;
		upX = px;
		upY = py;
		
		px -= 32;
		py -= 37;
		
		if(px >= 1 && py >= 1 && px <= 31 && py <= 31) // caps
		{			
			toggleCaps();
			downX = 0;
			downY = 0;
		}
		
		if(px >= 1 && py >= 33 && px <= 31 && py <= 62) // shift
		{
			toggleShift();
			downX = 0;
			downY = 0;
		}
		
		if(px >= 1 && py >= 64 && px <= 31 && py <= 94) // special
		{
			toggleSpecial();
			downX = 0;
			downY = 0;
		}
		
		if(px >= 160 && py >= 1 && px <= 190 && py <= 31) // backspace
		{
			return translateCode(K_B, SYMBOLS);
		}
		
		if(px >= 160 && py >= 33 && px <= 190 && py <= 62) // return
		{
			return translateCode(K_R, SYMBOLS);
		}
		
		if(px >= 129 && py >= 1 && px <= 158 && py <= 31) // delete
		{
			return translateCode(K_D, SYMBOLS);
		}
		
		if(px >= 129 && py >= 33 && px <= 158 && py <= 62) // space
		{
			shift = 0;
			return translateCode(K_W, SYMBOLS);
		}
		
		if(px >= 129 && py >= 64 && px <= 158 && py <= 94) // number toggle
		{
			isNumbers = 1 - isNumbers;
			downX = 0;
			downY = 0;
		}
		
		curChar = 0;
		return 0;
	}
	
	py -= 37;
	
	if(py < 0)
		return 0;
	
	int tilex = 0;
	int tiley = py / 19;
	
	if(tiley < 0 || tiley > 4)
		return 0;
	
	switch(tiley)
	{
		case 0: // first row
			if(px < 13)
				return 0;
			px -= 13;
			tilex = px / 19;
			break;
		case 1: // second row
			if(px < 23)
				return 0;
			if(px >= 213 && px <= 242) // backspace
				return translateCode(K_B, SYMBOLS);
			
			px -= 23;
			tilex = px / 19;
			break;
		case 2: // third row
			if(px < 13)
				return 0;
			if(px >= 203 && px <= 242) // return
				return translateCode(K_R, SYMBOLS);
			
			px -= 13;
			tilex = px / 19;
			break;
		case 3: // fourth row
			if(px < 13)
				return 0;
			if(px < 42) // shift
			{
				toggleShift();
				curChar = 0;
				return 0;
			}
			
			px -= 42;
			tilex = px / 19;
			break;
		case 4: // fifth row
			if(px < 13)
				return 0;
			if(px >= 89 && px < 184) // space
				return translateCode(K_W, SYMBOLS);
			
			px -= 13;
			tilex = px / 19;
			break;
	}
	
	if(tilex < 0 || tilex > 11)
		return 0;
	
	char c = 0; // capture character.				
	
	if(tiley == 0) // this is the numbers
	{    
		if(shift == 1)
			c = translateCode(keyboard_Hit[tilex], SYMBOLS);
		else
			c = translateCode(keyboard_Hit[tilex], NUMBERS);
	}
	else // this is letters
	{
		if((caps == 0 && shift == 0) || (caps == 1 && shift == 1))
		{
			if(special == 0)
				c = translateCode(keyboard_Hit[tilex+(tiley*12)],LOWERCASE);
			else
				c = translateCode(keyboard_Hit[tilex+(tiley*12)],SPECIAL0);
		}
		else
		{
			if(special == 0)
				c = translateCode(keyboard_Hit[tilex+(tiley*12)],UPPERCASE);
			else
				c = translateCode(keyboard_Hit[tilex+(tiley*12)],SPECIAL1);
		}
	}
	
	if(c==DEL) // Return
	{
		// to ensure we dont lowercase on a delete
	} 
	else if(c == CAP)
	{
		toggleCaps();
		curChar = 0;
		c = 0;
	} 
	else if(c == SPL)
	{
		toggleSpecial();
		curChar = 0;
		c = 0;				
	} 
	else
	{
		shift = 0;
	}
	
	return c;
}

int whichBlock(int px, int py)
{	
	px -= 32;
	py -= 37;
	
	if(py > 0 && py < 32) // first row
	{
		if(px > 32 && px < 64)
			return 1;
		if(px > 64 && px < 96)
			return 2;
		if(px > 96 && px < 128)
			return 3;
	}
	
	if(py > 32 && py < 63) // second row
	{
		if(px > 32 && px < 64)
			return 4;
		if(px > 64 && px < 96)
			return 5;
		if(px > 96 && px < 128)
			return 6;
	}
	
	if(py > 63 && py < 95) // third row
	{
		if(px > 32 && px < 64)
			return 7;
		if(px > 64 && px < 96)
			return 8;
		if(px > 96 && px < 128)
			return 9;
	}
	
	if(px >= 129 && py >= 64 && px <= 158 && py <= 94) // number toggle
		return -1;
	
	return 0;
}

void executeMove(int px, int py)
{
	if(px == 0 || py == 0)
		return;
	
	upX = px;
	upY = py;
}

char executeUp()
{
	if(activeKeyboard() != KB_MESSAGEASE)
		return 0;
		
	int startBlock = whichBlock(downX, downY);
	int endBlock = whichBlock(upX, upY);	
	int switchNumber = (startBlock * 10) + endBlock;
	
	if(startBlock == 0 || endBlock == 0)
		return 0;
	
	char *str = NULL;
	
	switch(activeView())
	{
		case ACTIVE_LOWERCASE:
			str = me_lower;
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
	
	char c = 0;
	
	if(startBlock == 9 && endBlock == -1)
		c = str[42];
	else
	{
		if(endBlock != -1)
		{
			switch(switchNumber)
			{
				// well look for the single tap ones first
				case 11:
					c = str[0];
					break;
				case 22:
					c = str[1];
					break;
				case 33:
					c = str[2];
					break;
				case 44:
					c = str[3];
					break;
				case 55:
					c = str[4];
					break;
				case 66:
					c = str[5];
					break;
				case 77:
					c = str[6];
					break;
				case 88:
					c = str[7];
					break;
				case 99:
					c = str[8];
					break;
				
				// ok, time to look for the drags
				case 12:
					c = str[27];
					break;
				case 14:
					c = str[28];
					break;
				case 15:
					c = str[9];
					break;
				case 21:
					c = str[29];
					break;
				case 23:
					c = str[30];
					break;
				case 25:
					c = str[10];
					break;
				case 32:
					c = str[31];
					break;
				case 35:
					c = str[11];
					break;
				case 36:
					c = str[32];
					break;
				case 41:
					c = str[33];
					break;
				case 45:
					c = str[12];
					break;
				case 47:
					c = str[35];
					break;
				case 51:
					c = str[13];
					break;
				case 52:
					c = str[14];
					break;
				case 53:
					c = str[15];
					break;
				case 54:
					c = str[16];
					break;
				case 56:
					c = str[18];
					break;
				case 57:
					c = str[19];
					break;
				case 58:
					c = str[20];
					break;
				case 59:
					c = str[21];
					break;
				case 63:
					c = str[34];
					break;
				case 65:
					c = str[22];
					break;
				case 69:
					c = str[36];
					break;
				case 74:
					c = str[37];
					break;
				case 75:
					c = str[23];
					break;
				case 78:
					c = str[39];
					break;
				case 85:
					c = str[24];
					break;
				case 87:
					c = str[40];
					break;
				case 89:
					c = str[25];
					break;
				case 95:
					c = str[26];
					break;
				case 96:
					c = str[38];
					break;
				case 98:
					c = str[41];
					break;
			}
		}
	}
	
	if(c == 32)
		c = 0;
	
	if(c != 0)
		shift = 0;
	
	return c;
}

/* accepts three parameters
   tmpBuffer: the buffer to operate on
   size: the number of characters that can be typed in a buffer (char array should be one larger 
																 for null termination)
   c: character to add/subtract/etc
*/

void removeText(char *tmpBuffer, int size)
{
	int tStart = beginHighlight;
	int tFinish = endHighlight;

	while(tmpBuffer[tFinish] != 0)
	{
		tmpBuffer[tStart] = tmpBuffer[tFinish];
		tStart++;
		tFinish++;
	}

	while(tStart < size)
	{
		tmpBuffer[tStart] = 0;
		tStart++;
	}
}

void insertChar(char *tmpBuffer, int size, char c)
{
	if(isInsert())
	{
		if((int)strlen(tmpBuffer) >= size)
		{
			return; 
		}	
		
		int z;
		for(z=size;z > getKBCursor();z--)
		{
			tmpBuffer[z] = tmpBuffer[z-1];		
		}
	}
	
	tmpBuffer[getKBCursor()] = c;
	
	if(getKBCursor() < size)
	{
		moveKBCursorRelative(CURSOR_FORWARD);
	}
}

void genericAction(char *tmpBuffer, int size, char c)
{
	if(isControl())
	{	
		toggleControl();
		
		setPressedChar(c);
		
		// control character
		switch(c)
		{
			case 'x':// cut
				// make sure highlight bounds are correct
				if(beginHighlight == -1 || endHighlight == -1)
				{
					return;
				}
				
				if(beginHighlight > endHighlight)
				{
					quickSwap(&beginHighlight, &endHighlight);
				}
				
				// perform cut
				setClipboardRange(tmpBuffer, beginHighlight, endHighlight);
				removeText(tmpBuffer, size);
				moveKBCursorAbsolute(beginHighlight);
				
				// reset the edit mode
				clearFakeHighlight();
				select = 0; 
				break;
			case 'c':// copy
				// make sure highlight bounds are correct
				if(beginHighlight == -1 || endHighlight == -1)
				{
					return;
				}
				
				if(beginHighlight > endHighlight)
				{
					quickSwap(&beginHighlight, &endHighlight);
				}
				
				// perform copy
				setClipboardRange(tmpBuffer, beginHighlight, endHighlight);
				
				// reset the edit mode
				clearFakeHighlight();
				select = 0; 
				break;
			case 'v':// paste
				if(beginHighlight != -1 && endHighlight != -1)
				{
					if(beginHighlight > endHighlight)
					{
						quickSwap(&beginHighlight, &endHighlight);
					}
					
					// we need to overwrite, but first check to make sure we don't overdo it
					if(strlen(tmpBuffer) - (endHighlight - beginHighlight) + strlen(getClipboard()) > size)
					{
						// after removing the current text and adding the new text, we are going over the size
						return;
					}
					
					// remove the current text
					removeText(tmpBuffer, size);
					moveKBCursorAbsolute(beginHighlight);
					clearFakeHighlight();
					select = 0; 
					
					// cursor is now where we want to add the new text
					// we know also that there is enough room, so just insert the text
					
					// ensure we are inserting, even if overwrite
					uint16 tInsert = insert;
					insert = 1;
					
					char *tAdd = getClipboard();
					
					// loop and add chars
					while(*tAdd != 0)
					{
						insertChar(tmpBuffer, size, *tAdd);
						tAdd++;
					}
					
					// reset insert
					insert = tInsert;
				}
				else
				{
					if(strlen(tmpBuffer) + strlen(getClipboard()) > size)
					{
						// after adding the new text, we are going over the size
						return;
					}
					
					clearFakeHighlight();
					select = 0; 
					
					// add the text
					char *tAdd = getClipboard();
					
					// loop and add chars
					while(*tAdd != 0)
					{
						insertChar(tmpBuffer, size, *tAdd);
						tAdd++;
					}
				}
				
				break;
		}
	}
	else
	{
		if(getKBCursor() < 0)
		{
			moveKBCursorAbsolute(0);
		}
		else if(getKBCursor() >= size)
		{
			moveKBCursorAbsolute(size - 1);
		}
		
		if(beginHighlight != -1 && endHighlight != -1)
		{
			if(beginHighlight > endHighlight)
			{
				quickSwap(&beginHighlight, &endHighlight);
			}
			
			// we have to clear out the data before performing the action
			removeText(tmpBuffer, size);
			
			// move the cursor
			moveKBCursorAbsolute(beginHighlight);
			
			// reset the edit mode
			clearFakeHighlight();
			select = 0; 
			
			if(c == BSP || c == DEL)
			{
				// we've done the work required, exit
				
				return;
			}
		}
		
		if(c == BSP)
		{
			// backspace
			if(getKBCursor() == 0) // handle backspacing nothing
			{
				return; 
			}
			
			int z;
			for(z=getKBCursor()-1;z<=size;z++)
			{
				tmpBuffer[z] = tmpBuffer[z+1];		
			}
			
			moveKBCursorRelative(CURSOR_BACKWARD);
		} 
		else if(c == DEL)
		{
			// del
			if(tmpBuffer[getKBCursor()] == 0) // handle deleting nothing
			{
				return; 
			}
			
			moveKBCursorRelative(CURSOR_FORWARD);
			
			int z;
			for(z=getKBCursor()-1;z<=size;z++)
			{
				tmpBuffer[z] = tmpBuffer[z+1];
			}
			
			moveKBCursorRelative(CURSOR_BACKWARD);
		} 
		else
		{
			// normal add
			insertChar(tmpBuffer, size, c);			
		}
		
		tmpBuffer[size] = 0;
	}
}
