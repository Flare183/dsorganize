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
#include "html.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <libfb\libcommon.h>
#include <vcard.h>
#include "fatwrapper.h"
#include "resources.h"
#include "globals.h"
#include "graphics.h"
#include "shortcuts.h"
#include "general.h"
#include "fonts.h"
#include "database.h"
#include "language.h"

// special tags
#include "tags_bin.h"

#define NOELEMENT -2
#define NONE -1

enum {
	HTML,
	HEAD,
	BODY,
	TITLE,
	P,
	I,
	B,
	U,
	STRIKE,
	SUPER,
	SUB,
	A,
	FONT,
	BR,
	H1,
	H2,
	H3,
	H4,
	H5,
	H6,
	SPAN,
	DIV,
	SMALL,
	BIG,
	UL,
	OL,
	TT,
	CODE,
	FRAMESET,
	NOFRAMES,
	SCRIPT,
	STYLE,
	PRE,
	EM,
	STRONG,
	CITE,
	DFN,
	VAR,
	SAMP,
	KBD,
	TD,
	TR,
	TABLE,
	Q,
	DT,
	DD,
	FORM,
	LABEL,
	INPUT,
	BUTTON,
	SELECT,
	OPTION,
	OPTGROUP,
	TEXTAREA
};

#define MAX_ITEMS 40

static int itemStack[MAX_ITEMS];
static int itemPos;
static uint16 colorStack[MAX_ITEMS];
static int colorPos;
static int sizeStack[MAX_ITEMS];
static int sizePos;
static int olStack[MAX_ITEMS];
static int olPos;
static int olCount;
static u32 tableStack[MAX_ITEMS];
static int tablePos;
static int a;
static u32 controlPos;
static int ulDepth = 0;
static uint16 *curBuffer;
static int destHeight;
static int lastPos = 0;
static int lastPass = 0;
static char lastFont = 0;
static int wrapCount = 0;
static int basePos = 0;
static bool nextGuy = false;
static bool allowURLS = false;
static u32 lastA = 0;
static u32 realA = 0;
static int clickX = 0;
static int clickY = 0;
static int focusControl = 0;
static HTML_RENDERED *tPage = NULL;
static uint16 *fakeSprite = NULL;

extern u32 htmlStyle;

// prototypes
char convertSpecial(char *tag);
int callbackHTMLWrapPass2(int pos, u32 c, int pass, int xPos, int yPos);
u32 getCRCFromOffset(char *inStr, LINK_TYPE inURL);
uint16 hexToU16(char *color);
int getNextColor(HTML_RENDERED *htmlPage);
bool subColorForName(char *color);
char asciiFromUnicode(int tByte);

// fake sprite for callback function

void freeFakeSprite()
{
	if(fakeSprite != NULL)
	{
		free(fakeSprite);
	}
	
	fakeSprite = NULL;
}
void initFakeSprite()
{
	freeFakeSprite();
	
	fakeSprite = (uint16 *)safeMalloc((256*192 / 8) + (5 * 2));
	
	fakeSprite[0] = 0xFFFF;
	fakeSprite[1] = 255;
	fakeSprite[2] = 191;
	fakeSprite[3] = 1;
	fakeSprite[4] = 0;
}

// virtual stack with peek in all elements

void initItemStack()
{
	for(int i=0;i<MAX_ITEMS-1;i++)
		itemStack[i] = NONE;
	
	itemPos = 0;
}

void pushItemStack(int code)
{
	if(itemPos == MAX_ITEMS)
		return;
	
	itemStack[itemPos] = code;
	itemPos++;
}

int popItemStack()
{
	if(itemPos == 0)
		return NONE;
	
	itemPos--;
	return itemStack[itemPos];
}

bool popItemStackByType(int code)
{
	for(int i=itemPos-1;i>0;i--) // find item
	{
		if(itemStack[i] == code) // found
		{
			for(int j=i;j<MAX_ITEMS-2;j++)
				itemStack[j] = itemStack[j+1];
			
			itemStack[MAX_ITEMS-1] = NONE;
			itemPos--;
			
			return true;
		}
	}
	
	return false;
}

void popItemStackUntilType(int code, int override)
{
	for(int i=itemPos-1;i>0;i--) // find item
	{
		if(itemStack[i] == code) // found
		{
			for(int j=i;j<MAX_ITEMS-1;j++)
				itemStack[j] = NONE;
			
			itemPos = i;
			
			return;
		}
		
		if(itemStack[i] == override)
		{
			// we've hit the element that contains this element
			
			for(int j=i+1;j<MAX_ITEMS-1;j++)
				itemStack[j] = NONE;
			
			itemPos = i+1;
			
			return;
		}
	}
}

int peekItemStack()
{
	if(itemPos == 0)
		return NONE;
		
	return itemStack[itemPos - 1];
}

bool isHead()
{
	if(itemPos == 0)
		return false;
	
	for(int i=0;i<itemPos;i++)
	{
		if(itemStack[i] == HEAD)
			return true;
	}
	
	return false;
}

bool isBody()
{
	if(itemPos == 0)
		return true;
	
	for(int i=0;i<itemPos;i++)
	{
		return !isHead();
	}	
	
	return false;
}

bool checkForBadTR()
{
	if(itemPos == 0)
		return false;
	
	int w = 0;
	
	for(int i=itemPos-1;i>0;i--)
	{
		if(w == 1)
		{
			if(itemStack[i] == TABLE) // found a tr and then a table already embedded
				return true;
		}
		if(w == 0)
		{
			if(itemStack[i] == TABLE) // found a tr and then a table already embedded
				return false;
			
			if(itemStack[i] == TR)
				w = 1;
		}
	}
	
	return false;
}

bool isTitle()
{
	if(peekItemStack() == TITLE)
		return true;
	
	return false;
}

bool isWhiteSpace(char c)
{
	if(c == '\r' || c == '\n' || c == ' ' || c == '	' )
		return true;
	return false;
}

// no more virtual stack

// time for table stack

void initTableStack()
{
	for(int i=0;i<MAX_ITEMS-1;i++)
		tableStack[i] = 0;
	
	tablePos = 0;
}

void pushTableStack(uint16 spacing, uint16 padding)
{
	if(tablePos == MAX_ITEMS)
		return;
	
	tableStack[tablePos] = (spacing << 16) | padding;
	tablePos++;
}

void popTableStack(uint16 &spacing, uint16 &padding)
{
	if(tablePos == 0)
	{
		spacing = DEFAULT_TABLE_SPACING;
		padding = DEFAULT_TABLE_PADDING;
		return;
	}
	
	tablePos--;
	
	padding = tableStack[tablePos] & 0xFFFF;
	spacing = (tableStack[tablePos] >> 16) & 0xFFFF;
}

// end table stack

// time for color stack

void initColorStack()
{
	for(int i=0;i<MAX_ITEMS-1;i++)
		colorStack[i] = 0;
	
	colorPos = 0;
}

void pushColorStack(uint16 color)
{
	if(colorPos == MAX_ITEMS)
		return;
	
	colorStack[colorPos] = color;
	colorPos++;
}

uint16 popColorStack()
{
	if(colorPos == 0)
		return C_DEFAULT;
		
	colorPos--;
	return colorStack[colorPos];
}

// end color stack

// time for size stack

void initSizeStack()
{
	for(int i=0;i<MAX_ITEMS-1;i++)
		sizeStack[i] = 0;
	
	sizePos = 0;
}

void pushSizeStack(int size)
{
	if(sizePos == MAX_ITEMS)
		return;
	
	sizeStack[sizePos] = size;
	sizePos++;
}

int popSizeStack()
{
	if(sizePos == 0)
		return 1;
		
	sizePos--;
	return sizeStack[sizePos];
}

// end size stack

// time for ol stack

void initOLStack()
{
	for(int i=0;i<MAX_ITEMS-1;i++)
		olStack[i] = 0;
	
	olPos = 0;
}

void pushOLStack(int order)
{
	if(olPos == MAX_ITEMS)
		return;
	
	olStack[olPos] = order;
	olPos++;
}

int popOLStack()
{
	if(olPos == 0)
		return 0;
		
	olPos--;
	return olStack[olPos];
}

// end color stack

// default font
void setDefaultFont(HTML_RENDERED *htmlPage)
{
	if(htmlPage->content == TYPE_PLAINTEXT)
	{		
		setFont(font_profont_9);
	}
	else
	{
		setFont(font_arial_9);
	}
}

// css parsing

int findPound(char *inStr)
{
	// simply locates the pound in a property
	
	if(strchr(inStr, '#') == NULL)
	{
		// didn't find it
		return 0;
	}
	else
	{
		// addcolorproperty expects the number after
		return (strchr(inStr, '#') - inStr) + 1; 
	}
}

uint16 colorFromName(char *color)
{
	char tColor[64];
	
	strncpy(tColor, color, 63);
	strlwr(tColor);
	
	if(subColorForName(tColor))
		return hexToU16(tColor);
	else
		return 0;
}

void findAndAddColor(char *wElement, char *wClass, char *wProperty)
{	
	if(strncmp(wProperty, "color:", 6) == 0)
	{
		// generic color
		int x = findPound(wProperty);
		uint16 tColor = 0;
		
		if(x > 0)
		{
			tColor = hexToU16(wProperty + x);
		}
		else
		{
			tColor = colorFromName(wProperty + 6);
		}
		
		// we need to loop through all css elements and see if it's there already
		// if not, add it
		
		if(tPage->maxCSSElements == 0)
		{
			// no way its added yet
			// add it in
			tPage->cssData = (CSS_ITEM *)trackMalloc(sizeof(CSS_ITEM), "css structure");
			memset(tPage->cssData, 0, sizeof(CSS_ITEM));
			
			strncpy(tPage->cssData[0].cssElement, wElement, 30);
			strncpy(tPage->cssData[0].cssClass, wClass, 30);
			tPage->cssData[0].colorIndex = getNextColor(tPage);
			tPage->cssData[0].hasFilled = true;
			
			// set the color to a default for display until the css page is loaded
			tPage->colors[tPage->cssData[0].colorIndex] = tColor;
			
			tPage->maxCSSElements = 1;
			
			return;
		}
		else
		{
			for(int i=0;i<tPage->maxCSSElements;i++)
			{
				if(strcmp(wElement, tPage->cssData[i].cssElement) == 0 && strcmp(wClass, tPage->cssData[i].cssClass) == 0)
				{
					// found the element, update colorindex
					tPage->colors[tPage->cssData[i].colorIndex] = tColor;					
					tPage->cssData[i].hasFilled = true;
					
					return;
				}
			}
			
			// element not found, must add it
			
			tPage->maxCSSElements++;
			
			tPage->cssData = (CSS_ITEM *)trackRealloc(tPage->cssData, sizeof(CSS_ITEM) * tPage->maxCSSElements);
			memset(&tPage->cssData[tPage->maxCSSElements - 1], 0, sizeof(CSS_ITEM));
			
			strncpy(tPage->cssData[tPage->maxCSSElements - 1].cssElement, wElement, 30);
			strncpy(tPage->cssData[tPage->maxCSSElements - 1].cssClass, wClass, 30);
			
			tPage->cssData[tPage->maxCSSElements - 1].colorIndex = getNextColor(tPage);
			tPage->cssData[tPage->maxCSSElements - 1].hasFilled = true;
			
			// set the color to a default for display until the css page is loaded
			tPage->colors[tPage->cssData[tPage->maxCSSElements - 1].colorIndex] = tColor;
		}
	}
}

void addColorProperty(char *tag, char *property)
{
	// this function expects all lowercase, as parseproperty should set
	char tElement[64];
	char tClass[64];
	char tSubElement[64];
	
	int whichType = 0;
	
	memset(tElement, 0, 64);
	memset(tClass, 0, 64);
	memset(tSubElement, 0, 64);
	
	while(*tag != 0)
	{
		char tChr = *tag;
		
		if(tChr == '.')
		{
			if(whichType == 0)
			{
				// move on to class from element
				whichType = 1;
			}
		}
		else if(tChr == ':')
		{
			// move on to subelement from class
			whichType = 2;
		}
		else
		{
			char *tStr = NULL;
			
			switch(whichType)
			{
				case 0:
					tStr = tElement;
					break;
				case 1:
					tStr = tClass;
					break;
				case 2:
					tStr = tSubElement;
					break;
				default:
					tStr = NULL;
					break;
			}
			
			if(tStr != NULL)
			{
				int x = strlen(tStr);
				
				if(x < 128)
				{
					tStr[x] = tChr;
					tStr[x+1] = 0;
				}
			}	
		}
		
		tag++;
	}
	
	if(strcmp(tElement, "strike") == 0)
	{
		// handle special case because this could be <s> or <strike>
		
		strcpy(tElement, "s");
	}
	
	// check to ensure that we actually care about this tag
	if(strcmp(tElement, "body") == 0)
	{
		// parse the body tag differently
		if(strncmp(property, "background-color:", 17) == 0)
		{
			// body bgcolor
			int x = findPound(property);
			
			if(x > 0)
			{
				tPage->colors[C_BG] = hexToU16(property + x);
			}
		}
		else if(strncmp(property, "color:", 6) == 0)
		{
			// body forecolor
			int x = findPound(property);
			
			if(x > 0)
			{			
				tPage->colors[C_DEFAULT] = hexToU16(property + x);
			}
		}
	}
	else if(strcmp(tElement, "a") == 0)
	{
		if(strlen(tClass) == 0)
		{
			if(strncmp(property, "color:", 6) == 0)
			{
				// global element
				if(strlen(tSubElement) == 0 || strcmp(tSubElement, "link") == 0)
				{
					// a link color
					int x = findPound(property);
					
					if(x > 0)
					{			
						tPage->colors[C_LINK] = hexToU16(property + x);
					}
				}
				else if(strcmp(tSubElement, "visited") == 0)
				{
					// a visited color
					int x = findPound(property);
					
					if(x > 0)
					{			
						tPage->colors[C_VLINK] = hexToU16(property + x);
					}
				}
				else if(strcmp(tSubElement, "active") == 0)
				{
					// a active color
					int x = findPound(property);
					
					if(x > 0)
					{	
						tPage->colors[C_ALINK] = hexToU16(property + x);
					}
				}
			}
		}
		else
		{
			// parse unique a here
			
			if(strlen(tSubElement) == 0 || strcmp(tSubElement, "link") == 0)
			{
				findAndAddColor(tElement, tClass, property);
			}
		}
	}
	else if(strcmp(tElement, "input") == 0)
	{
		// forms styling
		
		if(strlen(tClass) == 0)
		{		
			// global element
			if(strncmp(property, "color:", 6) == 0)
			{
				// body bgcolor
				int x = findPound(property);
				
				if(x > 0)
				{
					tPage->colors[F_TEXT] = hexToU16(property + x);
				}
			}
			if(strncmp(property, "background:", 11) == 0)
			{
				// body bgcolor
				int x = findPound(property);
				
				if(x > 0)
				{
					tPage->colors[F_BG] = hexToU16(property + x);
				}
			}
			if(strncmp(property, "border:", 7) == 0)
			{
				// body bgcolor
				int x = findPound(property);
				
				if(x > 0)
				{
					tPage->colors[F_BORDER] = hexToU16(property + x);
				}
			}
		}
	}
	else
	{
		// parse unique other here
		
		findAndAddColor(tElement, tClass, property);
	}
}

void parseProperty(char *tag, char *property)
{
	// tags can have more than one type on them
	// properties are singular
	
	if(tag == NULL || property == NULL)
		return;
	
	if(strlen(tag) == 0)
		return;
	if(strlen(property) == 0)
		return;
	
	char tmpTag[129];
	char tmpProperty[129];
	
	strncpy(tmpProperty, property, 128);
	strlwr(tmpProperty);
	
	if(strchr(tag, ',') == NULL)
	{
		// treat as last tag
		
		strncpy(tmpTag, tag, 128);
	}
	else
	{
		// iterate through all elements, comma separated
		
		memset(tmpTag, 0, 129);
		
		while(*tag != 0)
		{
			char tChr = *tag;
			
			if(tChr == ',')
			{
				strlwr(tmpTag);
				
				addColorProperty(tmpTag, tmpProperty);
				
				memset(tmpTag, 0, 129);
			}
			else
			{
				int x = strlen(tmpTag);
				
				if(x < 128)
				{
					tmpTag[x] = tChr;
					tmpTag[x+1] = 0;
				}
			}
			
			tag++;
		}
	}
	
	strlwr(tmpTag);	
	addColorProperty(tmpTag, tmpProperty);
}

void parseCSS(char *cssSrc, HTML_RENDERED *htmlPage)
{
	char curTag[129];
	char curProperty[129];
	int propertyDepth = 0;
	
	tPage = htmlPage;
	
	memset(curTag, 0, 129);
	memset(curProperty, 0, 129);
	
	if(strncmp(cssSrc, "<!--", 4) == 0)
	{
		// skip comment tag
		cssSrc+=4;
	}
	
	if(cssSrc[strlen(cssSrc)-1] == '<')
		cssSrc[strlen(cssSrc)-1] = 0;
	if(strncmp(&cssSrc[strlen(cssSrc)-3], "-->", 3) == 0)
		cssSrc[strlen(cssSrc)-3] = 0;
	
	while(*cssSrc != 0)
	{
		if(*cssSrc == '/' && *(cssSrc + 1) == '*')
		{
			// comment
			
			while(!(*cssSrc == '*' && *(cssSrc + 1) == '/') && *cssSrc != 0)
				cssSrc++;
			
			if(*cssSrc != 0)
				cssSrc+=2; // skip closing comment
		}
		char tChr = *cssSrc;
		
		switch(propertyDepth)
		{
			case 0:
				// tag acquisition
				
				switch(tChr)
				{
					case '{':
						// into property
						propertyDepth = 1;
						memset(curProperty, 0, 129);
						
						break;
					case ' ':
					case '\t':
					case '\n':
					case '\r':
						// ignore these whitespaces
						break;
					default:
					{
						// add to curTag
						
						int x = strlen(curTag);
						
						if(x < 128)
						{
							curTag[x] = tChr;
							curTag[x+1] = 0;
						}
						break;
					}
				}
				
				break;
			case 1:				
				// property acquisition
				
				switch(tChr)
				{
					case '}':
						// out to tag
						propertyDepth = 0;
						
						parseProperty(curTag, curProperty);
						
						memset(curTag, 0, 129);
						memset(curProperty, 0, 129);
						
						break;
					case ';':
						// end of particular property
						
						parseProperty(curTag, curProperty);
						memset(curProperty, 0, 129);
						
						break;
					case ' ':
					case '\t':
					case '\n':
					case '\r':
						// ignore these whitespaces
						break;
					default:
					{
						// add to curProperty
						
						int x = strlen(curProperty);
						
						if(x < 128)
						{
							curProperty[x] = tChr;
							curProperty[x+1] = 0;
						}
						break;
					}
				}
				
				break;
			default:				
				// ignore this because its too deep
				
				switch(tChr)
				{
					case '{':
						propertyDepth++;
						break;
					case '}':
						propertyDepth--;
						break;
				}
				
				break;
		}
		
		cssSrc++;
	}
	
	if(htmlPage->maxCSSElements == 0)
		return;
	
	// now loop through all css elements and set the ones that aren't set yet to defaults
	for(int i=0;i<htmlPage->maxCSSElements;i++)
	{
		if(!htmlPage->cssData[i].hasFilled)
		{
			htmlPage->colors[htmlPage->cssData[i].colorIndex] = htmlPage->colors[htmlPage->cssData[i].relativeIndex];
			htmlPage->cssData[i].hasFilled = true;
		}
	}
}

void fixBadColor(HTML_RENDERED *htmlPage)
{
	// we will attempt to find colors that are the same as the background to display
	// this is for pages like ds-x.com where the background is white due to no bg image
	// parsing, yet the text is white due to css
	
	uint16 tBG = htmlPage->colors[C_BG] | BIT(15);
	
	// check for default color
	if((htmlPage->colors[C_DEFAULT] | BIT(15)) == tBG)
	{
		htmlPage->colors[C_DEFAULT] = ~tBG | BIT(15);
	}
	
	// check for link color
	if((htmlPage->colors[C_LINK] | BIT(15)) == tBG)
	{
		htmlPage->colors[C_LINK] = ~tBG | BIT(15);
	}
	
	if(htmlPage->maxColors > 0)
	{	
		// now loop through all colors and set ones that are transparent to an inverse
		for(u32 i=C_START;i<htmlPage->maxColors+C_START;i++)
		{
			if((htmlPage->colors[i] | BIT(15)) == tBG)
			{
				htmlPage->colors[i] = ~tBG | BIT(15);
			}
		}
	}
}

// this is split into massive functions due to the need for it to be as unrolled and fast as possible
int callbackHTMLWrap(int pos, u32 c, int pass, int xPos, int yPos)
{	
	if(pass == 2)
	{
		lastPos = tPage->controlCodes[0].position - a;
		controlPos = 0;
		lastFont = 255;
		
		setCallBack(callbackHTMLWrapPass2);
	}
	
	if(lastPos != pos)
		return 0;
	
	if(tPage->controlCodes[controlPos].whichForm)
	{	
		fakeSprite[1] = tPage->formCodes[tPage->controlCodes[controlPos].whichForm - 1].width - 1;
		fakeSprite[2] = tPage->formCodes[tPage->controlCodes[controlPos].whichForm - 1].height - 1;
		
		setFont(font_arial_9);
		
		font_arial_9[30] = fakeSprite;
		
		goto skipNext;
	}
	
	if(tPage->controlCodes[controlPos].whichA)
		tPage->urlCodes[tPage->controlCodes[controlPos].whichA-1].yPos = yPos;
	
	if(F_GETSIZE(tPage->controlCodes[controlPos].fontOptions) != lastFont)
	{
		lastFont = F_GETSIZE(tPage->controlCodes[controlPos].fontOptions);
		switch(lastFont)
		{
			case 0:
				setFont(font_gautami_10);
				break;
			case 1:
				setFont(font_arial_9);
				break;
			case 2:
				setFont(font_arial_11);
				break;
			case 3:
				setFont(font_arial_13);
				break;
			case 6:
				setFont(font_profont_9);
				break;
			// bold overrides
			case 4:
				setFont(font_arial_9);
				break;
			case 5:
				setFont(font_arial_11);
				break;					
		}
	}
	
skipNext:
	do {	
		if((int)controlPos < tPage->maxControls)
			controlPos++;
		else
			controlPos = 0;
		
		lastPos = tPage->controlCodes[controlPos].position;
	} while(lastPos == -1);
	
	lastPos -= a;
	
	return 0;
}

int callbackHTMLWrapPass2(int pos, u32 c, int pass, int xPos, int yPos)
{	
	if(nextGuy)
	{
		if(!tPage->wrapCodes)
			tPage->wrapCodes = (WRAP_ITEM *)trackMalloc(sizeof(WRAP_ITEM), "wrap stack");
		else
			tPage->wrapCodes = (WRAP_ITEM *)trackRealloc(tPage->wrapCodes, sizeof(WRAP_ITEM) * (tPage->maxWraps + 1));
		
		tPage->wrapCodes[tPage->maxWraps].position = pos + a;
		if(controlPos > 0)
			tPage->wrapCodes[tPage->maxWraps].lastItem = &(tPage->controlCodes[controlPos-1]);
		else
			tPage->wrapCodes[tPage->maxWraps].lastItem = NULL;
		tPage->wrapCodes[tPage->maxWraps].controlPos = controlPos;
		tPage->wrapCodes[tPage->maxWraps].yPos = yPos;
		
		tPage->maxWraps++;
		nextGuy = false;
	}
	else if(c == 10 || c == 13 || c == 27)
	{
		wrapCount++;
		
		if(wrapCount == MAX_WRAPS)
		{
			nextGuy = true;
			wrapCount = 0;
		}
	}
	
	if(lastPos != pos)
		return 0;
	
	if(tPage->controlCodes[controlPos].whichForm)
	{	
		fakeSprite[1] = tPage->formCodes[tPage->controlCodes[controlPos].whichForm - 1].width - 1;
		fakeSprite[2] = tPage->formCodes[tPage->controlCodes[controlPos].whichForm - 1].height - 1;
		
		tPage->formCodes[tPage->controlCodes[controlPos].whichForm - 1].xPos = xPos;
		tPage->formCodes[tPage->controlCodes[controlPos].whichForm - 1].yPos = yPos;
		
		setFont(font_arial_9);
		
		font_arial_9[30] = fakeSprite;
	}
	else if(F_GETSIZE(tPage->controlCodes[controlPos].fontOptions) != lastFont)
	{
		lastFont = F_GETSIZE(tPage->controlCodes[controlPos].fontOptions);
		switch(lastFont)
		{
			case 0:
				setFont(font_gautami_10);
				break;
			case 1:
				setFont(font_arial_9);
				break;
			case 2:
				setFont(font_arial_11);
				break;
			case 3:
				setFont(font_arial_13);
				break;
			case 6:
				setFont(font_profont_9);
				break;
			// bold overrides
			case 4:
				setFont(font_arial_9);
				break;
			case 5:
				setFont(font_arial_11);
				break;					
		}
	}
	
	do {	
		if((int)controlPos < tPage->maxControls)
			controlPos++;
		else
			controlPos = 0;
		
		lastPos = tPage->controlCodes[controlPos].position;
	} while(lastPos == -1);
	
	lastPos -= a;
	
	return 0;
}

int callbackHTML(int pos, u32 c, int pass, int xPos, int yPos)
{
	if(pass != lastPass)
	{
		lastPos = tPage->controlCodes[basePos].position - a;
		controlPos = basePos;
		lastPass = pass;
		
		if(!basePos)
		{			
			setColor(tPage->colors[C_DEFAULT]);
			setBold(false);
			setItalics(false);
			setUnderline(false);
			setStrikeThrough(false);
			setDefaultFont(tPage);
		}
		else
		{
			setBold(tPage->controlCodes[basePos-1].fontOptions & F_BOLD);
			setItalics(tPage->controlCodes[basePos-1].fontOptions & F_ITALICS);
			setUnderline(tPage->controlCodes[basePos-1].fontOptions & F_UNDERLINE);
			setStrikeThrough(tPage->controlCodes[basePos-1].fontOptions & F_STRIKETHROUGH);
			
			setColor(tPage->colors[tPage->controlCodes[basePos-1].fontOptions & 0xFFFF]);
			
			switch(F_GETSIZE(tPage->controlCodes[basePos-1].fontOptions))
			{
				case 0:
					setFont(font_gautami_10);
					break;
				case 1:
					setFont(font_arial_9);
					break;
				case 2:
					setFont(font_arial_11);
					break;
				case 3:
					setFont(font_arial_13);
					break;
				case 6:
					setFont(font_profont_9);
					break;
				// bold overrides
				case 4:
					setFont(font_arial_9);
					setBold(true);
					break;
				case 5:
					setFont(font_arial_11);
					setBold(true);
					break;					
			}
		}	
	}
	
	if(lastPos != pos)
		return 0;
	
	if(pass == 2 && (tPage->controlCodes[controlPos].fontOptions & F_HR) && curBuffer)
	{				
		if((yPos + 5) >= 0 && (yPos + 5) < destHeight)
		{
			for(int j=0;j<tPage->destWidth-3;j++)
			{					
				uint16 *t = curBuffer + ((yPos + 5) * 256) + 3 + j;
				*t = BIT(15) | ~*t;
			}
		}
	}
	else if(tPage->controlCodes[controlPos].whichForm)
	{	
		int fPos = tPage->controlCodes[controlPos].whichForm - 1;
		
		if(pass == 2)
		{
			setBold(false);
			setItalics(false);
			setUnderline(false);
			setStrikeThrough(false);
			
			switch(tPage->formCodes[fPos].whatType)
			{
				case FORM_TEXT:
				case FORM_PASSWORD:	
				{
					int tColor = tPage->colors[F_BORDER];
					
					if(focusControl == fPos + 1)
						tColor = 0x7E00;
					
					drawRect(xPos, yPos, xPos + tPage->formCodes[fPos].width, yPos + 15, tPage->colors[F_BG], curBuffer, 383);
					
					drawRect(xPos, yPos, xPos + tPage->formCodes[fPos].width, yPos, tColor, curBuffer, 383);
					drawRect(xPos, yPos + 15, xPos + tPage->formCodes[fPos].width, yPos + 15, tColor, curBuffer, 383);
					
					drawRect(xPos, yPos, xPos, yPos + 15, tColor, curBuffer, 383);
					drawRect(xPos + tPage->formCodes[fPos].width, yPos, xPos + tPage->formCodes[fPos].width, yPos + 15, tColor, curBuffer, 383);
					
					clearCallBack();
					
					setColor(tPage->colors[F_TEXT]);
					setFont(font_gautami_10);
					
					bool deleteMe = false;
					char *tempSpace;
					
					if(tPage->formCodes[fPos].whatType == FORM_PASSWORD)
					{
						// gotta mask this shit
						
						int tSize = strlen((char *)tPage->formCodes[fPos].state);
						tempSpace = (char *)trackMalloc(tSize+1, "temp text");
						memset(tempSpace, '*', tSize);
						tempSpace[tSize] = 0;
						deleteMe = true;
					}
					else
						tempSpace = (char *)tPage->formCodes[fPos].state;
					
					dispString(3, 4, tempSpace, curBuffer, 1, xPos, yPos, xPos + tPage->formCodes[fPos].width, yPos + 15);
					
					if(deleteMe)
						trackFree(tempSpace);
					
					setCallBack(callbackHTML);
					
					break;
				}
				case FORM_CHECK:
				{
					int tColor = tPage->colors[F_BORDER];
					
					if(focusControl == fPos + 1)
						tColor = 0x7E00;
					
					drawRect(xPos, yPos, xPos + 15, yPos + 15, tPage->colors[F_BG], curBuffer, 383);
					
					drawRect(xPos, yPos, xPos + 15, yPos, tColor, curBuffer, 383);
					drawRect(xPos, yPos + 15, xPos + 15, yPos + 15, tColor, curBuffer, 383);
					
					drawRect(xPos, yPos, xPos, yPos + 15, tColor, curBuffer, 383);
					drawRect(xPos + 15, yPos, xPos + 15, yPos + 15, tColor, curBuffer, 383);
					
					if(tPage->formCodes[fPos].state)
						drawRect(xPos + 4, yPos + 4, xPos + 11, yPos + 11, tPage->colors[F_TEXT], curBuffer, 383);
					
					break;
				}
				case FORM_RADIO:
				{
					wb_radio[5] = tPage->colors[F_BORDER];
					
					if(focusControl == fPos + 1)
						wb_radio[5] = 0x7E00;
					
					wb_radio[6] = tPage->colors[F_BG];
					
					if(tPage->formCodes[fPos].maxLength)
						wb_radio[7] = tPage->colors[F_TEXT];
					else
						wb_radio[7] = wb_radio[6];
					
					dispCustomSprite(xPos, yPos, wb_radio, 31775, 0xFFFF, curBuffer, 256, 384);
					
					break;
				}
				case FORM_SUBMIT:
				case FORM_BUTTON:
				case FORM_RESET:
				{
					int tColor = tPage->colors[F_BORDER];
					
					if(focusControl == fPos + 1)
						tColor = 0x7E00;
					
					drawRect(xPos, yPos, xPos + tPage->formCodes[fPos].width, yPos + 15, tPage->colors[F_BG], curBuffer, 383);
					
					drawRect(xPos, yPos, xPos + tPage->formCodes[fPos].width, yPos, tColor, curBuffer, 383);
					drawRect(xPos, yPos + 15, xPos + tPage->formCodes[fPos].width, yPos + 15, tColor, curBuffer, 383);
					
					drawRect(xPos, yPos, xPos, yPos + 15, tColor, curBuffer, 383);
					drawRect(xPos + tPage->formCodes[fPos].width, yPos, xPos + tPage->formCodes[fPos].width, yPos + 15, tColor, curBuffer, 383);
					
					clearCallBack();
					
					setColor(tPage->colors[F_TEXT]);
					setFont(font_gautami_10);
					
					dispString(5, 4, (char *)tPage->formCodes[fPos].state, curBuffer, 1, xPos, yPos, xPos + tPage->formCodes[fPos].width, yPos + 15);
					
					setCallBack(callbackHTML);
					
					if((tPage->formCodes[fPos].whatType == FORM_BUTTON || tPage->formCodes[fPos].whatType == FORM_RESET) && focusControl == fPos + 1)
						focusControl = 0;
					
					break;
				}
				case FORM_TEXTAREA:	
				{
					int tColor = tPage->colors[F_BORDER];
					
					if(focusControl == fPos + 1)
						tColor = 0x7E00;
					
					drawRect(xPos, yPos, xPos + tPage->formCodes[fPos].width, yPos + tPage->formCodes[fPos].height, tPage->colors[F_BG], curBuffer, 383);
					
					drawRect(xPos, yPos, xPos + tPage->formCodes[fPos].width, yPos, tColor, curBuffer, 383);
					drawRect(xPos, yPos + tPage->formCodes[fPos].height, xPos + tPage->formCodes[fPos].width, yPos + tPage->formCodes[fPos].height, tColor, curBuffer, 383);
					
					drawRect(xPos, yPos, xPos, yPos + tPage->formCodes[fPos].height, tColor, curBuffer, 383);
					drawRect(xPos + tPage->formCodes[fPos].width, yPos, xPos + tPage->formCodes[fPos].width, yPos + tPage->formCodes[fPos].height, tColor, curBuffer, 383);
					
					clearCallBack();
					
					setColor(tPage->colors[F_TEXT]);
					setFont(font_gautami_10);
					
					u32 *skipAmount = (u32 *)tPage->formCodes[fPos].state;
					dispString(3, 4, (char *)(tPage->formCodes[fPos].state) + 4 + (*skipAmount), curBuffer, 1, xPos, yPos, xPos + tPage->formCodes[fPos].width, yPos + tPage->formCodes[fPos].height);
					
					setCallBack(callbackHTML);
					
					break;
				}
				case FORM_SELECT:
				{
					FORM_SELECT_ITEM *fsi = (FORM_SELECT_ITEM *)tPage->formCodes[fPos].state;
					int tColor = tPage->colors[F_BORDER];
					
					if(focusControl == fPos + 1)
						tColor = 0x7E00;
					
					switch(fsi->type)
					{
						case 0: // drop down combo
						{
							drawRect(xPos, yPos, xPos + tPage->formCodes[fPos].width, yPos + 15, tPage->colors[F_BG], curBuffer, 383);
							
							drawRect(xPos, yPos, xPos + tPage->formCodes[fPos].width, yPos, tColor, curBuffer, 383);
							drawRect(xPos, yPos + 15, xPos + tPage->formCodes[fPos].width, yPos + 15, tColor, curBuffer, 383);
							
							drawRect(xPos, yPos, xPos, yPos + 15, tColor, curBuffer, 383);
							drawRect(xPos + tPage->formCodes[fPos].width, yPos, xPos + tPage->formCodes[fPos].width, yPos + 15, tColor, curBuffer, 383);
							
							drawRect(xPos + tPage->formCodes[fPos].width - 10, yPos, xPos + tPage->formCodes[fPos].width - 10, yPos + 15, tColor, curBuffer, 383);
							drawRect(xPos + 10, yPos, xPos + 10, yPos + 15, tColor, curBuffer, 383);
							
							spr_left[5] = tPage->colors[F_TEXT];
							spr_right[4] = tPage->colors[F_TEXT];
							
							dispCustomSprite(xPos + 3, yPos + 3, spr_left, 31775, 0xFFFF, curBuffer, 256, 384);
							dispCustomSprite(xPos + tPage->formCodes[fPos].width - 7, yPos + 3, spr_right, 31775, 0xFFFF, curBuffer, 256, 384);
							
							spr_left[5] = 32767;
							spr_right[4] = 32767;
							
							if(tPage->formCodes[fPos].maxLength > 0)
							{
								int tCaption = fsi->cursor;
								if(tCaption < 0)
									tCaption = 0;
								if(tCaption >= tPage->formCodes[fPos].maxLength)
									tCaption = tPage->formCodes[fPos].maxLength - 1;
								
								clearCallBack();
								
								setColor(tPage->colors[F_TEXT]);
								setFont(font_gautami_10);
								
								dispString(3, 4, fsi->items[tCaption].caption, curBuffer, 1, xPos + 10, yPos, xPos + tPage->formCodes[fPos].width - 10, yPos + 15);
								
								setCallBack(callbackHTML);
							}
							
							break;
						}
						case 1: // multiselect
						{
							drawRect(xPos, yPos, xPos + tPage->formCodes[fPos].width, yPos + tPage->formCodes[fPos].height, tPage->colors[F_BG], curBuffer, 383);
							
							drawRect(xPos, yPos, xPos + tPage->formCodes[fPos].width, yPos, tColor, curBuffer, 383);
							drawRect(xPos, yPos + tPage->formCodes[fPos].height, xPos + tPage->formCodes[fPos].width, yPos + tPage->formCodes[fPos].height, tColor, curBuffer, 383);
							
							drawRect(xPos, yPos, xPos, yPos + tPage->formCodes[fPos].height, tColor, curBuffer, 383);
							drawRect(xPos + tPage->formCodes[fPos].width, yPos, xPos + tPage->formCodes[fPos].width, yPos + tPage->formCodes[fPos].height, tColor, curBuffer, 383);
							
							drawRect(xPos + tPage->formCodes[fPos].width - 10, yPos, xPos + tPage->formCodes[fPos].width - 10, yPos + tPage->formCodes[fPos].height, tColor, curBuffer, 383);
							drawRect(xPos + tPage->formCodes[fPos].width - 10, yPos + 15, xPos + tPage->formCodes[fPos].width, yPos + 15, tColor, curBuffer, 383);
							drawRect(xPos + tPage->formCodes[fPos].width - 10, yPos + tPage->formCodes[fPos].height - 15, xPos + tPage->formCodes[fPos].width, yPos + tPage->formCodes[fPos].height - 15, tColor, curBuffer, 383);
							
							up_arrow[5] = tPage->colors[F_TEXT];
							down_arrow[4] = tPage->colors[F_TEXT];
							
							dispCustomSprite(xPos + tPage->formCodes[fPos].width - 8, yPos + 5, up_arrow, 31775, 0xFFFF, curBuffer, 256, 384);
							dispCustomSprite(xPos + tPage->formCodes[fPos].width - 8, yPos + tPage->formCodes[fPos].height - 8, down_arrow, 31775, 0xFFFF, curBuffer, 256, 384);
							
							up_arrow[5] = 32767;
							down_arrow[4] = 32767;
							
							int maxLines = (tPage->formCodes[fPos].height - 3) / 12;
							int y = fsi->cursor;
							
							if(y > tPage->formCodes[fPos].maxLength - (maxLines - 1))
								y = tPage->formCodes[fPos].maxLength - (maxLines - 1);
							
							if(y < 0)
								y = 0;
							
							int z = y + maxLines;
							
							if(z >= tPage->formCodes[fPos].maxLength)
								z = tPage->formCodes[fPos].maxLength;
							
							clearCallBack();
							
							setColor(tPage->colors[F_TEXT]);
							setFont(font_gautami_10);
							
							for(int x=y;x<z;x++)
							{
								int tDepth = fsi->items[x].depth;
								
								if(fsi->items[x].depth < 0)
								{
									tDepth = -fsi->items[x].depth;
									tDepth--;
									
									drawRect(xPos + 2, yPos + 2 + 12*(x-y), xPos + tPage->formCodes[fPos].width - 12, yPos + 13 + 12*(x-y), RGB15(15,15,15) | BIT(15), curBuffer, 384);
								}
								else if(fsi->items[x].highlighted)
									drawRect(xPos + 2, yPos + 2 + 12*(x-y), xPos + tPage->formCodes[fPos].width - 12, yPos + 13 + 12*(x-y), 0x6776 | BIT(15), curBuffer, 384);
								
								dispString(3, 4, fsi->items[x].caption, curBuffer, 1, xPos + (tDepth*5), yPos + 12*(x-y), xPos + tPage->formCodes[fPos].width - 10, yPos + 15 + 12*(x-y));
							}
							
							setCallBack(callbackHTML);
							
							// draw scroll
							
							if(maxLines	> 3 && maxLines < tPage->formCodes[fPos].maxLength)
							{
								int maxMovement = tPage->formCodes[fPos].height - 46;
								
								double z = (double)maxMovement * (double)fsi->cursor;
								z = z / (double)(tPage->formCodes[fPos].maxLength - maxLines);
								
								drawRect(xPos + tPage->formCodes[fPos].width - 8, yPos + 17 + (int)z, xPos + tPage->formCodes[fPos].width - 2, yPos + 29 + (int)z, tPage->colors[F_TEXT], curBuffer, 383);
							}
							
							break;
						}
					}
					
					break;
				}
				case FORM_IMAGE:	
				{
					int tColor = tPage->colors[F_BORDER];
					
					IMAGE_TYPE *tIMG = (IMAGE_TYPE *)tPage->formCodes[fPos].state;
					
					if(tIMG->state == IMAGE_NEEDSDOWNLOAD || tIMG->state == IMAGE_CANTLOAD)
					{
						fakeSprite[1] = tPage->formCodes[fPos].width - 1;
						fakeSprite[2] = tPage->formCodes[fPos].height - 1;
						
						font_arial_9[30] = fakeSprite;
						
						calculateNewXY(&xPos, &yPos); // to ensure it wraps
						
						drawRect(xPos, yPos, xPos + tPage->formCodes[fPos].width - 1, yPos + tPage->formCodes[fPos].height - 1, tPage->colors[F_BG], curBuffer, 383);
						
						drawRect(xPos, yPos, xPos + tPage->formCodes[fPos].width - 1, yPos, tColor, curBuffer, 383);
						drawRect(xPos, yPos + tPage->formCodes[fPos].height - 1, xPos + tPage->formCodes[fPos].width - 1, yPos + tPage->formCodes[fPos].height - 1, tColor, curBuffer, 383);
						
						drawRect(xPos, yPos, xPos, yPos + tPage->formCodes[fPos].height - 1, tColor, curBuffer, 383);
						drawRect(xPos + tPage->formCodes[fPos].width - 1, yPos, xPos + tPage->formCodes[fPos].width - 1, yPos + tPage->formCodes[fPos].height - 1, tColor, curBuffer, 383);
						
						// display alt text
						clearCallBack();
						
						setColor(tPage->colors[F_TEXT]);
						setFont(font_gautami_10);
						
						if(tIMG->state == IMAGE_CANTLOAD)
						{
							dispCustomSprite(xPos + 5, yPos + 4, wb_stop, 31775, 0xFFFF, curBuffer, xPos + tPage->formCodes[fPos].width - 1, yPos + tPage->formCodes[fPos].height - 1);
						}
						else
						{
							dispCustomSprite(xPos + 5, yPos + 4, picfile, 31775, 0xFFFF, curBuffer, xPos + tPage->formCodes[fPos].width - 1, yPos + tPage->formCodes[fPos].height - 1);
						}
						
						dispString(5 + 20, 4, tPage->formCodes[fPos].name, curBuffer, 1, xPos, yPos, xPos + tPage->formCodes[fPos].width - 1, yPos + tPage->formCodes[fPos].height - 1);
						
						setCallBack(callbackHTML);
					}
				}
			}
		}
		
		setFont(font_arial_9);
		
		bool rendered = false;
		
		if(tPage->formCodes[fPos].whatType == FORM_IMAGE)
		{
			IMAGE_TYPE *tIMG = (IMAGE_TYPE *)tPage->formCodes[fPos].state;
			
			if(tIMG->state == IMAGE_READYFORDISPLAY)
			{
				u32 size = (((tIMG->graphicsData[0] + 1) * (tIMG->graphicsData[1] + 1)) * 2) + 4;				
				
				font_arial_9[30] = tIMG->graphicsData + ((size >> 1) * tIMG->aniData.curAni);
				rendered = true;
			}
		}
		
		if(!rendered)
		{
			fakeSprite[1] = tPage->formCodes[fPos].width - 1;
			fakeSprite[2] = tPage->formCodes[fPos].height - 1;
			
			font_arial_9[30] = fakeSprite;
		}
	}
	else
	{
		if(pass == 2)
		{
			setBold(tPage->controlCodes[controlPos].fontOptions & F_BOLD);
			setItalics(tPage->controlCodes[controlPos].fontOptions & F_ITALICS);
			setUnderline(tPage->controlCodes[controlPos].fontOptions & F_UNDERLINE);
			setStrikeThrough(tPage->controlCodes[controlPos].fontOptions & F_STRIKETHROUGH);
			
			setColor(tPage->colors[tPage->controlCodes[controlPos].fontOptions & 0xFFFF]);
		}
		
		switch(F_GETSIZE(tPage->controlCodes[controlPos].fontOptions))
		{
			case 0:
				setFont(font_gautami_10);
				break;
			case 1:
				setFont(font_arial_9);
				break;
			case 2:
				setFont(font_arial_11);
				break;
			case 3:
				setFont(font_arial_13);					
				break;
			case 6:
				setFont(font_profont_9);
				break;
			// bold overrides
			case 4:
				setFont(font_arial_9);
				setBold(true);
				break;
			case 5:
				setFont(font_arial_11);
				setBold(true);
				break;					
		}
	}
	
	do {	
		if((int)controlPos < tPage->maxControls)
			controlPos++;
		else
			controlPos = 0;
			
		lastPos = tPage->controlCodes[controlPos].position;
	} while(lastPos == -1);
	
	lastPos -= a;
	
	return 0;
}

// same as the last one with url code handling to make _SURE_ it doesnt' slow normally
int callbackHTMLURLS(int pos, u32 c, int pass, int xPos, int yPos)
{
	if(pass != lastPass)
	{
		lastPos = tPage->controlCodes[basePos].position - a;
		controlPos = basePos;
		lastPass = pass;
		
		if(!basePos)
		{			
			setColor(tPage->colors[C_DEFAULT]);
			setBold(false);
			setItalics(false);
			setUnderline(false);
			setStrikeThrough(false);
			setDefaultFont(tPage);
		}
		else
		{
			setBold(tPage->controlCodes[basePos-1].fontOptions & F_BOLD);
			setItalics(tPage->controlCodes[basePos-1].fontOptions & F_ITALICS);
			setUnderline(tPage->controlCodes[basePos-1].fontOptions & F_UNDERLINE);
			setStrikeThrough(tPage->controlCodes[basePos-1].fontOptions & F_STRIKETHROUGH);
			
			setColor(tPage->colors[tPage->controlCodes[basePos-1].fontOptions & 0xFFFF]);
			
			switch(F_GETSIZE(tPage->controlCodes[basePos-1].fontOptions))
			{
				case 0:
					setFont(font_gautami_10);
					break;
				case 1:
					setFont(font_arial_9);
					break;
				case 2:
					setFont(font_arial_11);
					break;
				case 3:
					setFont(font_arial_13);				
					break;
				case 6:
					setFont(font_profont_9);
					break;
				// bold overrides
				case 4:
					setFont(font_arial_9);
					setBold(true);
					break;
				case 5:
					setFont(font_arial_11);
					setBold(true);
					break;					
			}
		}	
	}
	
	if(lastPos != pos)
	{	
		if(lastA)
		{
			if(clickX >= xPos && clickY >= yPos)
			{
				// on the horizontal
				if(clickX <= xPos + (int)getCharWidth(c) + 1 && clickY <= yPos + (int)getFontHeight())
					realA = lastA;
			}
		}
		
		return 0;
	}
	
	if(pass == 2 && (tPage->controlCodes[controlPos].fontOptions & F_HR) && curBuffer)
	{				
		if((yPos + 5) >= 0 && (yPos + 5) < destHeight)
		{
			for(int j=0;j<tPage->destWidth-3;j++)
			{					
				uint16 *t = curBuffer + ((yPos + 5) * 256) + 3 + j;
				*t = BIT(15) | ~*t;
			}
		}
	}
	else if(tPage->controlCodes[controlPos].whichForm)
	{	
		int fPos = tPage->controlCodes[controlPos].whichForm - 1;	
		
		if(pass == 2)
		{
			setBold(false);
			setItalics(false);
			setUnderline(false);
			setStrikeThrough(false);
			
			switch(tPage->formCodes[fPos].whatType)
			{
				case FORM_TEXT:
				case FORM_PASSWORD:
				{
					if(clickX >= xPos && clickY >= yPos)
					{
						// on the horizontal
						if(clickX <= xPos + tPage->formCodes[fPos].width && clickY <= yPos + 15)
							focusControl = fPos + 1;
					}
					
					int tColor = tPage->colors[F_BORDER];
					
					if(focusControl == fPos + 1)
						tColor = 0x7E00;
					
					drawRect(xPos, yPos, xPos + tPage->formCodes[fPos].width, yPos + 15, tPage->colors[F_BG], curBuffer, 383);
					
					drawRect(xPos, yPos, xPos + tPage->formCodes[fPos].width, yPos, tColor, curBuffer, 383);
					drawRect(xPos, yPos + 15, xPos + tPage->formCodes[fPos].width, yPos + 15, tColor, curBuffer, 383);
					
					drawRect(xPos, yPos, xPos, yPos + 15, tColor, curBuffer, 383);
					drawRect(xPos + tPage->formCodes[fPos].width, yPos, xPos + tPage->formCodes[fPos].width, yPos + 15, tColor, curBuffer, 383);
					
					clearCallBack();
					
					setColor(tPage->colors[F_TEXT]);
					setFont(font_gautami_10);
					
					bool deleteMe = false;
					char *tempSpace;
					
					if(tPage->formCodes[fPos].whatType == FORM_PASSWORD)
					{
						// gotta mask this shit
						
						int tSize = strlen((char *)tPage->formCodes[fPos].state);
						tempSpace = (char *)trackMalloc(tSize+1, "temp text");
						memset(tempSpace, '*', tSize);
						tempSpace[tSize] = 0;
						deleteMe = true;
					}
					else
						tempSpace = (char *)tPage->formCodes[fPos].state;
					
					dispString(3, 4, tempSpace, curBuffer, 1, xPos, yPos, xPos + tPage->formCodes[fPos].width, yPos + 15);
					
					if(deleteMe)
						trackFree(tempSpace);
					
					setCallBack(callbackHTMLURLS);
					
					break;
				}
				case FORM_CHECK:
				{
					if(clickX >= xPos && clickY >= yPos)
					{
						// on the horizontal
						if(clickX <= xPos + 15 && clickY <= yPos + 15)
						{
							if(tPage->formCodes[fPos].state)
								tPage->formCodes[fPos].state = (void *)0;
							else
								tPage->formCodes[fPos].state = (void *)1;
						
							focusControl = fPos + 1;
						}
					}
					
					int tColor = tPage->colors[F_BORDER];
					
					if(focusControl == fPos + 1)
						tColor = 0x7E00;
					
					drawRect(xPos, yPos, xPos + 15, yPos + 15, tPage->colors[F_BG], curBuffer, 383);
					
					drawRect(xPos, yPos, xPos + 15, yPos, tColor, curBuffer, 383);
					drawRect(xPos, yPos + 15, xPos + 15, yPos + 15, tColor, curBuffer, 383);
					
					drawRect(xPos, yPos, xPos, yPos + 15, tColor, curBuffer, 383);
					drawRect(xPos + 15, yPos, xPos + 15, yPos + 15, tColor, curBuffer, 383);
					
					if(tPage->formCodes[fPos].state)
						drawRect(xPos + 4, yPos + 4, xPos + 11, yPos + 11, tPage->colors[F_TEXT], curBuffer, 383);
					
					break;
				}
				case FORM_RADIO:
				{
					if(clickX >= xPos && clickY >= yPos)
					{
						// on the horizontal
						if(clickX <= xPos + 15 && clickY <= yPos + 15)
						{
							tPage->formCodes[fPos].maxLength = 1;
							
							for(int ci=0;ci<tPage->maxFormControls-1;++ci)
							{
								if(ci == fPos)
									continue;
								
								if(tPage->formCodes[ci].whatType == FORM_RADIO && tPage->formCodes[ci].formID == tPage->formCodes[fPos].formID)
								{
									if(strcmp(tPage->formCodes[fPos].name, tPage->formCodes[ci].name) == 0)
										tPage->formCodes[ci].maxLength = 0;
								}
							}
							
							focusControl = fPos + 1;
						}						
					}
					
					wb_radio[5] = tPage->colors[F_BORDER];
					
					if(focusControl == fPos + 1)
						wb_radio[5] = 0x7E00;
					
					wb_radio[6] = tPage->colors[F_BG];
					
					if(tPage->formCodes[fPos].maxLength)
						wb_radio[7] = tPage->colors[F_TEXT];
					else
						wb_radio[7] = wb_radio[6];
					
					dispCustomSprite(xPos, yPos, wb_radio, 31775, 0xFFFF, curBuffer, 256, 384);
					
					break;
				}
				case FORM_SUBMIT:
				case FORM_BUTTON:
				case FORM_RESET:
				{
					if(clickX >= xPos && clickY >= yPos)
					{
						// on the horizontal
						if(clickX <= xPos + tPage->formCodes[fPos].width && clickY <= yPos + 15)
						{
							if(tPage->formCodes[fPos].whatType == FORM_RESET) // gotta reset fields
							{
								for(int ci=0;ci<tPage->maxFormControls-1;++ci)
								{
									if(tPage->formCodes[ci].formID == tPage->formCodes[fPos].formID)
									{
										switch(tPage->formCodes[ci].whatType)
										{
											case FORM_TEXT:
											case FORM_PASSWORD:
												memset(tPage->formCodes[ci].state, 0, tPage->formCodes[ci].maxLength + 1);
												memcpy(tPage->formCodes[ci].state, tPage->formCodes[ci].resetState, strlen((char *)tPage->formCodes[ci].resetState) + 1);
												
												break;
											case FORM_TEXTAREA:
												memset(tPage->formCodes[ci].state, 0, tPage->formCodes[ci].maxLength + 5);
												memcpy((char *)tPage->formCodes[ci].state + 4, tPage->formCodes[ci].resetState, strlen((char *)tPage->formCodes[ci].resetState) + 1);
												
												break;
											case FORM_CHECK:
												tPage->formCodes[ci].state = tPage->formCodes[ci].resetState;
												
												break;
											case FORM_RADIO:
												tPage->formCodes[ci].maxLength = (int)tPage->formCodes[ci].resetState;
												
												break;
											case FORM_SELECT:
												FORM_SELECT_ITEM *fsi = (FORM_SELECT_ITEM *)tPage->formCodes[ci].state;
												
												switch(fsi->type)
												{
													case 0:
														fsi->cursor = (int)tPage->formCodes[ci].resetState;
														break;
													case 1:
														for(int ri=0;ri<tPage->formCodes[ci].maxLength;ri++)
															fsi->items[ri].highlighted = ((char *)tPage->formCodes[ci].resetState)[ri];
														break;
												}
												
												break;
										}
									}
								}
							}
							
							focusControl = fPos + 1;
						}
					}
					
					int tColor = tPage->colors[F_BORDER];
					
					if(focusControl == fPos + 1)
						tColor = 0x7E00;
					
					drawRect(xPos, yPos, xPos + tPage->formCodes[fPos].width, yPos + 15, tPage->colors[F_BG], curBuffer, 383);
					
					drawRect(xPos, yPos, xPos + tPage->formCodes[fPos].width, yPos, tColor, curBuffer, 383);
					drawRect(xPos, yPos + 15, xPos + tPage->formCodes[fPos].width, yPos + 15, tColor, curBuffer, 383);
					
					drawRect(xPos, yPos, xPos, yPos + 15, tColor, curBuffer, 383);
					drawRect(xPos + tPage->formCodes[fPos].width, yPos, xPos + tPage->formCodes[fPos].width, yPos + 15, tColor, curBuffer, 383);
					
					clearCallBack();
					
					setColor(tPage->colors[F_TEXT]);
					setFont(font_gautami_10);
					
					dispString(5, 4, (char *)tPage->formCodes[fPos].state, curBuffer, 1, xPos, yPos, xPos + tPage->formCodes[fPos].width, yPos + 15);
					
					setCallBack(callbackHTMLURLS);
					
					break;
				}
				case FORM_TEXTAREA:	
				{
					if(clickX >= xPos && clickY >= yPos)
					{
						// on the horizontal
						if(clickX <= xPos + tPage->formCodes[fPos].width && clickY <= yPos + tPage->formCodes[fPos].height)
							focusControl = fPos + 1;
					}
					
					int tColor = tPage->colors[F_BORDER];
					
					if(focusControl == fPos + 1)
						tColor = 0x7E00;
					
					drawRect(xPos, yPos, xPos + tPage->formCodes[fPos].width, yPos + tPage->formCodes[fPos].height, tPage->colors[F_BG], curBuffer, 383);
					
					drawRect(xPos, yPos, xPos + tPage->formCodes[fPos].width, yPos, tColor, curBuffer, 383);
					drawRect(xPos, yPos + tPage->formCodes[fPos].height, xPos + tPage->formCodes[fPos].width, yPos + tPage->formCodes[fPos].height, tColor, curBuffer, 383);
					
					drawRect(xPos, yPos, xPos, yPos + tPage->formCodes[fPos].height, tColor, curBuffer, 383);
					drawRect(xPos + tPage->formCodes[fPos].width, yPos, xPos + tPage->formCodes[fPos].width, yPos + tPage->formCodes[fPos].height, tColor, curBuffer, 383);
					
					clearCallBack();
					
					setColor(tPage->colors[F_TEXT]);
					setFont(font_gautami_10);
					
					u32 *skipAmount = (u32 *)tPage->formCodes[fPos].state;
					dispString(3, 4, (char *)(tPage->formCodes[fPos].state) + 4 + (*skipAmount), curBuffer, 1, xPos, yPos, xPos + tPage->formCodes[fPos].width, yPos + tPage->formCodes[fPos].height);
					
					setCallBack(callbackHTMLURLS);
					
					break;
				}
				case FORM_SELECT:
				{
					FORM_SELECT_ITEM *fsi = (FORM_SELECT_ITEM *)tPage->formCodes[fPos].state;
					
					switch(fsi->type)
					{
						case 0: // drop down combo
						{
							if(clickX >= xPos && clickY >= yPos)
							{
								// on the horizontal
								if(clickX <= xPos + tPage->formCodes[fPos].width && clickY <= yPos + tPage->formCodes[fPos].height)
								{
									if(fsi->focused)
									{
										
										if(clickX < (xPos + 10))
										{
											fsi->cursor--;
											
											if(fsi->cursor < 0)
												fsi->cursor = 0;
										}
										else if(clickX >= (xPos + tPage->formCodes[fPos].width - 10))
										{
											fsi->cursor++;
											
											if(fsi->cursor >= tPage->formCodes[fPos].maxLength)
												fsi->cursor = tPage->formCodes[fPos].maxLength - 1;
										}
									}
									
									focusControl = fPos + 1;
									fsi->focused = 1;
								}
								else
									fsi-> focused = 0;
							}
							else
								fsi->focused = 0;
							
							int tColor = tPage->colors[F_BORDER];
							
							if(focusControl == fPos + 1)
								tColor = 0x7E00;
							
							drawRect(xPos, yPos, xPos + tPage->formCodes[fPos].width, yPos + 15, tPage->colors[F_BG], curBuffer, 383);
							
							drawRect(xPos, yPos, xPos + tPage->formCodes[fPos].width, yPos, tColor, curBuffer, 383);
							drawRect(xPos, yPos + 15, xPos + tPage->formCodes[fPos].width, yPos + 15, tColor, curBuffer, 383);
							
							drawRect(xPos, yPos, xPos, yPos + 15, tColor, curBuffer, 383);
							drawRect(xPos + tPage->formCodes[fPos].width, yPos, xPos + tPage->formCodes[fPos].width, yPos + 15, tColor, curBuffer, 383);
							
							drawRect(xPos + tPage->formCodes[fPos].width - 10, yPos, xPos + tPage->formCodes[fPos].width - 10, yPos + 15, tColor, curBuffer, 383);
							drawRect(xPos + 10, yPos, xPos + 10, yPos + 15, tColor, curBuffer, 383);
							
							spr_left[5] = tPage->colors[F_TEXT];
							spr_right[4] = tPage->colors[F_TEXT];
							
							dispCustomSprite(xPos + 3, yPos + 3, spr_left, 31775, 0xFFFF, curBuffer, 256, 384);
							dispCustomSprite(xPos + tPage->formCodes[fPos].width - 7, yPos + 3, spr_right, 31775, 0xFFFF, curBuffer, 256, 384);
							
							spr_left[5] = 32767;
							spr_right[4] = 32767;
							
							if(tPage->formCodes[fPos].maxLength > 0)
							{
								int tCaption = fsi->cursor;
								if(tCaption < 0)
									tCaption = 0;
								if(tCaption >= tPage->formCodes[fPos].maxLength)
									tCaption = tPage->formCodes[fPos].maxLength - 1;
								
								clearCallBack();
								
								setColor(tPage->colors[F_TEXT]);
								setFont(font_gautami_10);
								
								dispString(3, 4, fsi->items[tCaption].caption, curBuffer, 1, xPos + 10, yPos, xPos + tPage->formCodes[fPos].width - 10, yPos + 15);
								
								setCallBack(callbackHTMLURLS);
							}
							
							break;
						}
						case 1: // multiselect
						{
							bool okToSelect = false;
							
							if(clickX >= xPos && clickY >= yPos)
							{
								// on the horizontal
								if(clickX <= xPos + tPage->formCodes[fPos].width && clickY <= yPos + tPage->formCodes[fPos].height)
								{
									if(fsi->focused)
									{
										int tX = clickX - xPos;
										int tY = clickY - yPos;
										
										// we can operate
										if(tX > 2 && tX < tPage->formCodes[fPos].width - 12)
											okToSelect = true;
										else if(tX >= tPage->formCodes[fPos].width - 10)
										{
											// scrollbar clicked
											
											int maxLines = (tPage->formCodes[fPos].height - 3) / 12;
											
											if(tY <= 15)
											{
												fsi->cursor--;
												if(fsi->cursor < 0)
													fsi->cursor = 0;
											}
											else if(tY >= tPage->formCodes[fPos].height - 15)
											{
												int maxCur = tPage->formCodes[fPos].maxLength - (maxLines - 1);
												
												fsi->cursor++;
												
												if(fsi->cursor >= maxCur)
													fsi->cursor = maxCur-1;
											}
										}
									}
									
									focusControl = fPos + 1;
									fsi->focused = 1;
								}
								else
									fsi-> focused = 0;
							}
							else
								fsi->focused = 0;
							
							int tColor = tPage->colors[F_BORDER];
							
							if(focusControl == fPos + 1)
								tColor = 0x7E00;
							
							drawRect(xPos, yPos, xPos + tPage->formCodes[fPos].width, yPos + tPage->formCodes[fPos].height, tPage->colors[F_BG], curBuffer, 383);
							
							drawRect(xPos, yPos, xPos + tPage->formCodes[fPos].width, yPos, tColor, curBuffer, 383);
							drawRect(xPos, yPos + tPage->formCodes[fPos].height, xPos + tPage->formCodes[fPos].width, yPos + tPage->formCodes[fPos].height, tColor, curBuffer, 383);
							
							drawRect(xPos, yPos, xPos, yPos + tPage->formCodes[fPos].height, tColor, curBuffer, 383);
							drawRect(xPos + tPage->formCodes[fPos].width, yPos, xPos + tPage->formCodes[fPos].width, yPos + tPage->formCodes[fPos].height, tColor, curBuffer, 383);
							
							drawRect(xPos + tPage->formCodes[fPos].width - 10, yPos, xPos + tPage->formCodes[fPos].width - 10, yPos + tPage->formCodes[fPos].height, tColor, curBuffer, 383);
							drawRect(xPos + tPage->formCodes[fPos].width - 10, yPos + 15, xPos + tPage->formCodes[fPos].width, yPos + 15, tColor, curBuffer, 383);
							drawRect(xPos + tPage->formCodes[fPos].width - 10, yPos + tPage->formCodes[fPos].height - 15, xPos + tPage->formCodes[fPos].width, yPos + tPage->formCodes[fPos].height - 15, tColor, curBuffer, 383);
							
							up_arrow[5] = tPage->colors[F_TEXT];
							down_arrow[4] = tPage->colors[F_TEXT];
							
							dispCustomSprite(xPos + tPage->formCodes[fPos].width - 8, yPos + 5, up_arrow, 31775, 0xFFFF, curBuffer, 256, 384);
							dispCustomSprite(xPos + tPage->formCodes[fPos].width - 8, yPos + tPage->formCodes[fPos].height - 8, down_arrow, 31775, 0xFFFF, curBuffer, 256, 384);
							
							up_arrow[5] = 32767;
							down_arrow[4] = 32767;
							
							int maxLines = (tPage->formCodes[fPos].height - 3) / 12;
							int y = fsi->cursor;
							
							if(y > tPage->formCodes[fPos].maxLength - (maxLines - 1))
								y = tPage->formCodes[fPos].maxLength - (maxLines - 1);
							
							if(y < 0)
								y = 0;
							
							int z = y + maxLines;
							
							if(z >= tPage->formCodes[fPos].maxLength)
								z = tPage->formCodes[fPos].maxLength;
							
							clearCallBack();
							
							setColor(tPage->colors[F_TEXT]);
							setFont(font_gautami_10);
							
							for(int x=y;x<z;x++)
							{
								int tDepth = fsi->items[x].depth;
								
								if(fsi->items[x].depth < 0)
								{
									tDepth = -fsi->items[x].depth;
									tDepth--;
									
									drawRect(xPos + 2, yPos + 2 + 12*(x-y), xPos + tPage->formCodes[fPos].width - 12, yPos + 13 + 12*(x-y), RGB15(15,15,15) | BIT(15), curBuffer, 384);
								}
								else
								{
									if(okToSelect)
									{
										if(clickY >= (yPos + 2 + 12*(x-y)) && clickY <= (yPos + 13 + 12*(x-y)))
											fsi->items[x].highlighted = 1 - fsi->items[x].highlighted;
									}
									
									if(fsi->items[x].highlighted)
										drawRect(xPos + 2, yPos + 2 + 12*(x-y), xPos + tPage->formCodes[fPos].width - 12, yPos + 13 + 12*(x-y), 0x6776 | BIT(15), curBuffer, 384);
								}
								
								dispString(3, 4, fsi->items[x].caption, curBuffer, 1, xPos + (tDepth*5), yPos + 12*(x-y), xPos + tPage->formCodes[fPos].width - 10, yPos + 15 + 12*(x-y));
							}
							
							setCallBack(callbackHTMLURLS);
							
							// draw scroll
							
							if(maxLines	> 3 && maxLines < tPage->formCodes[fPos].maxLength)
							{
								int maxMovement = tPage->formCodes[fPos].height - 46;
								
								double z = (double)maxMovement * (double)fsi->cursor;
								z = z / (double)(tPage->formCodes[fPos].maxLength - maxLines);
								
								drawRect(xPos + tPage->formCodes[fPos].width - 8, yPos + 17 + (int)z, xPos + tPage->formCodes[fPos].width - 2, yPos + 29 + (int)z, tPage->colors[F_TEXT], curBuffer, 383);
							}
							
							break;
						}
					}
					
					break;
				}
				case FORM_IMAGE:	
				{
					if(clickX >= xPos && clickY >= yPos)
					{
						// on the horizontal
						if(clickX < xPos + tPage->formCodes[fPos].width && clickY < yPos + tPage->formCodes[fPos].height)
						{
							realA = tPage->controlCodes[controlPos].whichA;
						}
					}
					
					int tColor = tPage->colors[F_BORDER];
					
					IMAGE_TYPE *tIMG = (IMAGE_TYPE *)tPage->formCodes[fPos].state;
					
					if(tIMG->state == IMAGE_NEEDSDOWNLOAD || tIMG->state == IMAGE_CANTLOAD)
					{
						fakeSprite[1] = tPage->formCodes[fPos].width - 1;
						fakeSprite[2] = tPage->formCodes[fPos].height - 1;
						
						font_arial_9[30] = fakeSprite;
						
						calculateNewXY(&xPos, &yPos); // to ensure it wraps
						
						drawRect(xPos, yPos, xPos + tPage->formCodes[fPos].width - 1, yPos + tPage->formCodes[fPos].height - 1, tPage->colors[F_BG], curBuffer, 383);
						
						drawRect(xPos, yPos, xPos + tPage->formCodes[fPos].width - 1, yPos, tColor, curBuffer, 383);
						drawRect(xPos, yPos + tPage->formCodes[fPos].height - 1, xPos + tPage->formCodes[fPos].width - 1, yPos + tPage->formCodes[fPos].height - 1, tColor, curBuffer, 383);
						
						drawRect(xPos, yPos, xPos, yPos + tPage->formCodes[fPos].height - 1, tColor, curBuffer, 383);
						drawRect(xPos + tPage->formCodes[fPos].width - 1, yPos, xPos + tPage->formCodes[fPos].width - 1, yPos + tPage->formCodes[fPos].height - 1, tColor, curBuffer, 383);
						
						// display alt text
						clearCallBack();
						
						setColor(tPage->colors[F_TEXT]);
						setFont(font_gautami_10);
						
						if(tIMG->state == IMAGE_CANTLOAD)
						{
							dispCustomSprite(xPos + 5, yPos + 4, wb_stop, 31775, 0xFFFF, curBuffer, xPos + tPage->formCodes[fPos].width - 1, yPos + tPage->formCodes[fPos].height - 1);
							dispString(5 + 20, 4, tPage->formCodes[fPos].name, curBuffer, 1, xPos, yPos, xPos + tPage->formCodes[fPos].width - 1, yPos + tPage->formCodes[fPos].height - 1);
						}
						else
						{
							dispString(5, 4, tPage->formCodes[fPos].name, curBuffer, 1, xPos, yPos, xPos + tPage->formCodes[fPos].width - 1, yPos + tPage->formCodes[fPos].height - 1);
						}
						
						setCallBack(callbackHTMLURLS);
					}
				}
			}
		}
		
		setFont(font_arial_9);
		
		bool rendered = false;
		
		if(tPage->formCodes[fPos].whatType == FORM_IMAGE)
		{	
			IMAGE_TYPE *tIMG = (IMAGE_TYPE *)tPage->formCodes[fPos].state;
			
			if(tIMG->state == IMAGE_READYFORDISPLAY)
			{
				u32 size = (((tIMG->graphicsData[0] + 1) * (tIMG->graphicsData[1] + 1)) * 2) + 4;				
				
				font_arial_9[30] = tIMG->graphicsData + ((size >> 1) * tIMG->aniData.curAni);
				rendered = true;
			}
		}
		
		if(!rendered)
		{
			fakeSprite[1] = tPage->formCodes[fPos].width - 1;
			fakeSprite[2] = tPage->formCodes[fPos].height - 1;
			
			font_arial_9[30] = fakeSprite;
		}
	}
	else
	{
		if(pass == 2)
		{
			setBold(tPage->controlCodes[controlPos].fontOptions & F_BOLD);
			setItalics(tPage->controlCodes[controlPos].fontOptions & F_ITALICS);
			setUnderline(tPage->controlCodes[controlPos].fontOptions & F_UNDERLINE);
			setStrikeThrough(tPage->controlCodes[controlPos].fontOptions & F_STRIKETHROUGH);
			
			setColor(tPage->colors[tPage->controlCodes[controlPos].fontOptions & 0xFFFF]);
			lastA = tPage->controlCodes[controlPos].whichA;		
			
			if(lastA)
			{
				if(clickX >= xPos && clickY >= yPos)
				{
					// on the horizontal
					if(clickX <= xPos + (int)getCharWidth(c) + 1 && clickY <= yPos + (int)getFontHeight())
						realA = lastA;
				}
			}
		}
		
		switch(F_GETSIZE(tPage->controlCodes[controlPos].fontOptions))
		{
			case 0:
				setFont(font_gautami_10);
				break;
			case 1:
				setFont(font_arial_9);
				break;
			case 2:
				setFont(font_arial_11);
				break;
			case 3:
				setFont(font_arial_13);					
				break;
			case 6:
				setFont(font_profont_9);
				break;
			// bold overrides
			case 4:
				setFont(font_arial_9);
				setBold(true);
				break;
			case 5:
				setFont(font_arial_11);
				setBold(true);
				break;					
		}
	}
	
	do {	
		if((int)controlPos < tPage->maxControls)
			controlPos++;
		else
			controlPos = 0;
			
		lastPos = tPage->controlCodes[controlPos].position;
	} while(lastPos == -1);
	
	lastPos -= a;
	
	return 0;
}

void updateItem(int position, bool bold, bool italics, bool underline, int fontSize, uint16 color, bool isHR, bool strike, u32 whichA, u32 whichForm)
{
	u32 tmpOptions = F_SETSIZE(fontSize) | (color & 0x0000FFFF);
	
	tmpOptions |= (bold ? F_BOLD : 0);
	tmpOptions |= (italics ? F_ITALICS : 0);
	tmpOptions |= (underline ? F_UNDERLINE : 0);
	tmpOptions |= (strike ? F_STRIKETHROUGH : 0);
	tmpOptions |= (isHR ? F_HR : 0);
	
	if(controlPos == 0)
	{
		tPage->controlCodes = (CONTROL_ITEM *)trackMalloc(sizeof(CONTROL_ITEM), "tPage->controlCodes");
		controlPos = 1;
		
		tPage->controlCodes[0].position = position;
		tPage->controlCodes[0].whichA = whichA;
		tPage->controlCodes[0].whichForm = whichForm;
		tPage->controlCodes[0].fontOptions = tmpOptions;
		return;
	}
	
	for(unsigned int i=0;i<controlPos;i++)
	{
		if(tPage->controlCodes[i].position == position)
		{
			tPage->controlCodes[i].whichA = whichA;
			tPage->controlCodes[i].whichForm = whichForm;
			tPage->controlCodes[i].fontOptions = tmpOptions;
			return;
		}
	}
	
	controlPos++;
	tPage->controlCodes = (CONTROL_ITEM *)trackRealloc(tPage->controlCodes, sizeof(CONTROL_ITEM) * controlPos);

	tPage->controlCodes[controlPos - 1].position = position;
	tPage->controlCodes[controlPos - 1].whichA = whichA;
	tPage->controlCodes[controlPos - 1].whichForm = whichForm;
	tPage->controlCodes[controlPos - 1].fontOptions = tmpOptions;
}

void moveLastItemBack()
{
	if(controlPos == 0)
	{
		// no item to move back
		return;
	}
	
	tPage->controlCodes[controlPos - 1].position--;
}

void setClicking(u32 tmpURL, uint16 action, HTML_RENDERED *htmlPage)
{
	// there's no way a link could have been clicked if there are no controls, so its safe to just jump into a loop
	
	tmpURL++; // offset to get proper url
	
	for(int i=0;i<htmlPage->maxControls;i++)
	{
		if(htmlPage->controlCodes[i].whichA == tmpURL) // update color on this link
		{
			u32 tmpOptions = htmlPage->controlCodes[i].fontOptions & 0xFFFF0000; // zero out font color
			
			htmlPage->controlCodes[i].fontOptions = tmpOptions | action;
		}
	}	
}

uint16 hexToU16(char *color)
{
	char r = 0;
	char g = 0;
    char b = 0;
	char tColor[128];
	
	strncpy(tColor, color, 127);	
	strlwr(tColor);
	
	if(strlen(tColor) > 6) // fix for babelfish, AGAIN
		tColor[6] = 0;
	
	if(strlen(tColor) == 3)
	{
		// special
		
		r = ((charFromHex(tColor[0]) << 4) | charFromHex(tColor[0])) >> 3;
		g = ((charFromHex(tColor[1]) << 4) | charFromHex(tColor[1])) >> 3;
		b = ((charFromHex(tColor[2]) << 4) | charFromHex(tColor[2])) >> 3;
	}
	else
	{
		while(strlen(tColor) != 6)
		{
			char str[10];
			strcpy(str,"0");
			strcat(str,tColor);
			
			strcpy(tColor,str);
		}
		
		r = ((charFromHex(tColor[0]) << 4) | charFromHex(tColor[1])) >> 3;
		g = ((charFromHex(tColor[2]) << 4) | charFromHex(tColor[3])) >> 3;
		b = ((charFromHex(tColor[4]) << 4) | charFromHex(tColor[5])) >> 3;
	}
	
	return RGB15(r,g,b);
}

void getTagLocations(char *str, u32 &start, u32 &end)
{
	while(*str != '=')
	{
		str++;
		start++;
		end++;
		
		if(*str == 0)
		{
			start = 0;
			end = 0;
			return;
		}
	}
	
	str++;
	start++;
	end++;
	
	while(*str == ' ')
	{
		str++;
		start++;
		end++;
		
		if(*str == 0)
		{
			start = 0;
			end = 0;
			return;
		}
	}
	
	int isQuoted = 0;
	
	// our position is now at the start of the element quote
	if(*str == 34) // quote
	{
		str++;
		start++;
		end++;
		
		isQuoted = 1;
	}
	else if(*str == 39)
	{
		str++;
		start++;
		end++;
		
		isQuoted = 2;
	}
	
	while(!(*str == 34 && isQuoted == 1) && !(*str == 39 && isQuoted == 2) && *str != 0 && !(*str == ' ' && !isQuoted))
	{
		str++;
		end++;
	}
}

void getElementText(char *tag, char *searchString, char *outText)
{
	char *tTag = (char *)safeMalloc(1024);
	char *andTag = (char *)safeMalloc(1024);
	
	strcpy(tTag, tag);
	
	strlwr(tTag);
	
	char *str = strstr(tTag, searchString);
	if(str == NULL)
	{
		outText[0] = 0;
		
		free(tTag);
		free(andTag);
		return;
	}
	
	while(*str != '=')
	{
		str++;
		
		if(*str == 0)
		{
			outText[0] = 0;
			
			free(tTag);
			free(andTag);
			return;
		}
	}
	
	str++;
	while(*str == ' ')
	{
		str++;
		
		if(*str == 0)
		{
			outText[0] = 0;
			
			free(tTag);
			free(andTag);
			return;
		}
	}
	
	int isQuoted = 0;
	
	// our position is now at the start of the element quote
	if(*str == 34) // quote
	{
		str++;	
		isQuoted = 1;
	}
	else if(*str == 39) // quote
	{
		str++;	
		isQuoted = 2;
	}
	
	outText[0] = 0;
	int i = 0;
	bool inAmpersand = false;
	
	while(!(*str == 34 && isQuoted == 1) && !(*str == 39 && isQuoted == 2) && *str != 0 && !(*str == ' ' && !isQuoted))
	{
		if(inAmpersand)
		{
			if(tag[(str - tTag)] == ';')
			{
				// end & tag
				
				outText[i] = convertSpecial(andTag);	
				outText[i+1] = 0;
				i++;
				
				inAmpersand = false;
			}
			else
			{
				int tStore = strlen(andTag);
				if(tStore < 1023)
					andTag[tStore] = tag[(str - tTag)];
			}
		}
		else
		{
			if(tag[(str - tTag)] == '&' && tag[(str - tTag) + 1] != ' ')
			{
				memset(andTag, 0, 1024);							
				inAmpersand = true;
			}
			else
			{
				outText[i] = tag[(str - tTag)];
				outText[i+1] = 0;
				i++;
			}
		}
		
		str++;
	}
	
	free(tTag);
	free(andTag);
}

bool subColorForName(char *color)
{
	if(strcmp(color,"black") == 0)
		strcpy(color, "0");
	else if(strcmp(color,"white") == 0)
		strcpy(color, "ffffff");
	else if(strcmp(color,"red") == 0)
		strcpy(color, "ff0000");
	else if(strcmp(color,"blue") == 0)
		strcpy(color, "ff");
	else if(strcmp(color,"green") == 0)
		strcpy(color, "8000");
	else if(strcmp(color,"yellow") == 0)
		strcpy(color, "ffff00");
	else if(strcmp(color,"orange") == 0)
		strcpy(color, "ffa500");
	else if(strcmp(color,"violet") == 0)
		strcpy(color, "ee82ee");
	else if(strcmp(color,"purple") == 0)
		strcpy(color, "800080");
	else if(strcmp(color,"pink") == 0)
		strcpy(color, "ffc0cb");
	else if(strcmp(color,"silver") == 0)
		strcpy(color, "c0c0c0");
	else if(strcmp(color,"gold") == 0)
		strcpy(color, "ffd700");
	else if(strcmp(color,"gray") == 0)
		strcpy(color, "808080");
	else if(strcmp(color,"aqua") == 0)
		strcpy(color, "ffff");
	else if(strcmp(color,"skyblue") == 0)
		strcpy(color, "87ceeb");
	else if(strcmp(color,"lightblue") == 0)
		strcpy(color, "add8e6");
	else if(strcmp(color,"fuchsia") == 0)
		strcpy(color, "ff00ff");
	else if(strcmp(color,"khaki") == 0)
		strcpy(color, "f0e68c");
	else
		return false;
	
	return true;
}

uint16 getFontColor(char *tag, char *searchString, uint16 def)
{
	char tColor[50];	
	char *color;
	
	memset(tColor, 0, 50);
	getElementText(tag, searchString, tColor);
	strlwr(tColor);
	
	if(strlen(tColor) == 0)
		return def;
	
	if(tColor[0] == '#')
		color = tColor + 1;
	else
		color = tColor;
	
	subColorForName(color);
	
	return hexToU16(color);
}

void separateTagFromElements(char *str, char *elements)
{
	char *tmp = strchr(str, ' ');
	
	if(tmp == NULL) // no elements
	{
		// Ensure that it's not a bad tag like <br/>
		if(str[strlen(str) - 1] == '/')
		{
			// It is
			str[strlen(str) - 1] = 0;
			strcpy(elements, "/");
			
			return;
		}
		
		strcpy(elements, "");
		return;
	}
	
	*tmp = 0;
	tmp++;
	strcpy(elements,tmp);
}

bool isSelfTerminating(char *elements)
{
	u32 elementLength = strlen(elements);
	
	if(elementLength == 0)
	{
		return false;
	}
	
	if(elements[elementLength - 1] == '/')
	{
		return true;
	}
	
	return false;
}

uint16 getColorIndex(uint16 color, HTML_RENDERED *htmlPage)
{
	if(htmlPage->maxColors == 0)
	{
		htmlPage->colors[C_START] = color;
		htmlPage->maxColors++;
		
		return C_START;
	}
	
	color |= BIT(15);
	
	for(u32 x = 0;x < htmlPage->maxColors - C_START; x++)
	{
		if(htmlPage->colors[C_START + x] == color)
			return C_START + x;
	}
	
	if((htmlPage->maxColors + C_START) == 512) // ran out of stack
		return C_START;
	
	htmlPage->colors[C_START + htmlPage->maxColors] = color;
	htmlPage->maxColors++;
	
	return C_START + htmlPage->maxColors - 1;
}

void fixAndTags()
{
	// Grab the actual data
	int whichTag = 0;	
	TAG_TYPE *tags = (TAG_TYPE *)tags_bin;
	
	// loop until end line
	while(tags[whichTag].tag[0] != ' ')
	{
		for(int x = 0; x < 20; x++)
		{
			if(tags[whichTag].tag[x] == 0)
			{
				// Already done this
				return;
			}
			
			if(tags[whichTag].tag[x] == ' ')
			{
				// Null terminate
				tags[whichTag].tag[x] = 0;
				break;
			}
		}
		whichTag++;
	}
	
	// Null terminate last tag
	tags[whichTag].tag[0] = 0;
}

char findAndTag(char *tag)
{
	// Grab the actual data
	int whichTag = 0;	
	TAG_TYPE *tags = (TAG_TYPE *)tags_bin;
	
	// loop until end line
	while(tags[whichTag].tag[0] != ' ')
	{
		if(strcmp(tags[whichTag].tag, tag) == 0)
		{
			return tags[whichTag].outChar;
		}
		
		whichTag++;
	}
	
	return 0;
}

char convertSpecial(char *tag)
{
	if(tag[0] == '#' && tag[1] != 0)
	{
		int tC = 0;
		
		if(tag[1] == 'x' || tag[1] == 'X')
		{
			sscanf(&tag[2], "%x", &tC);
		}
		else
		{		
			sscanf(&tag[1], "%d", &tC);
		}
		
		switch(tC)
		{
			case 155: // alternate for &gt;
				return '>';
			case 0:
				return '?';
			default:
				return asciiFromUnicode(tC);
		}
	}
	
	char outTag = findAndTag(tag);
	
	if(outTag == 0)
	{
		// Maybe it's just bad html
		strlwr(tag);
		
		outTag = findAndTag(tag);
	}
	
	if(outTag == 0)
	{
		// Still couldn't find it
		return '?';
	}
	
	return outTag;
}

u32 getNumber(char *str)
{
	int x = 0;
	int num = 0;
	
	while(str[x] != 0)
	{
		num *= 10;
		
		if(str[x] >= '0' && str[x] <= '9')
			num += str[x] - '0';
		
		x++;
	}
	
	return num;
}

void takeCareOfItem(int whatVal, int override, char *&tempOut, int &tmpDst, bool &tmpBold, bool &tmpItalics, bool &tmpUnderline, bool &tmpStrikeThrough, int &tmpSize, uint16 &tmpColor, u32 &tmpA, bool &inCode, bool &oldUnderline, int &framesCount, bool &inNoFrames, bool &inScript, bool &inStyle, HTML_RENDERED *htmlPage)
{
	int tVal = popItemStack();
	
	while(tVal != whatVal && tVal != NONE)
	{
		if(tVal == override)
		{
			pushItemStack(override);
			return;
		}
		
		switch(tVal)
		{
			case H1:
				tmpSize = popSizeStack();
				if(htmlStyle > 0)
					tmpColor = popColorStack();
				
				updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
				
				break;
			case H2:
				tmpSize = popSizeStack();
				if(htmlStyle > 0)
					tmpColor = popColorStack();
				
				updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
				
				break;
			case H3:
				tmpSize = popSizeStack();
				if(htmlStyle > 0)
					tmpColor = popColorStack();
				
				updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
				
				break;
			case H4:
				tmpSize = popSizeStack();
				if(htmlStyle > 0)
					tmpColor = popColorStack();
				
				updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
				
				break;
			case H5:
				tmpSize = popSizeStack();
				if(htmlStyle > 0)
					tmpColor = popColorStack();
				
				updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
				
				break;
			case H6:
				tmpSize = popSizeStack();
				if(htmlStyle > 0)
					tmpColor = popColorStack();
				
				updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
				
				break;
			case SMALL:
				tmpSize = popSizeStack();
				if(htmlStyle > 0)
					tmpColor = popColorStack();
				
				updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
				break;
			case TT:
				tmpSize = popSizeStack();
				if(htmlStyle > 0)
					tmpColor = popColorStack();
				
				updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);								
				break;
			case CODE:
				tmpSize = popSizeStack();
				if(htmlStyle > 0)
					tmpColor = popColorStack();
				
				updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
				inCode = false;
				break;
			case PRE:
				tmpSize = popSizeStack();
				if(htmlStyle > 0)
					tmpColor = popColorStack();
				
				updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
				inCode = false;
				break;
			case SAMP:
				tmpSize = popSizeStack();
				if(htmlStyle > 0)
					tmpColor = popColorStack();
				
				updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
				inCode = false;
				break;
			case KBD:
				tmpSize = popSizeStack();
				if(htmlStyle > 0)
					tmpColor = popColorStack();
				
				updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
				inCode = false;
				break;
			case BIG:
				tmpSize = popSizeStack();
				if(htmlStyle > 0)
					tmpColor = popColorStack();
				
				updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
				break;
			case SUB:
				tmpSize = popSizeStack();
				if(htmlStyle > 0)
					tmpColor = popColorStack();
				
				updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
				
				switch(tmpSize)
				{
					case 0:
					case 6:
						tempOut[tmpDst] = POS_UP;
						tmpDst++;
						tempOut[tmpDst] = POS_UP;
						tmpDst++;
						tempOut[tmpDst] = POS_UP;
						tmpDst++;
						tempOut[tmpDst] = POS_UP;
						tmpDst++;
						break;
					case 1:
					case 4:
						tempOut[tmpDst] = POS_UP;
						tmpDst++;
						tempOut[tmpDst] = POS_UP;
						tmpDst++;
						tempOut[tmpDst] = POS_UP;
						tmpDst++;
						tempOut[tmpDst] = POS_UP;
						tmpDst++;
						tempOut[tmpDst] = POS_UP;
						tmpDst++;
						tempOut[tmpDst] = POS_UP;
						tmpDst++;
						tempOut[tmpDst] = POS_UP;
						tmpDst++;
						break;
					case 2:
					case 5:
						tempOut[tmpDst] = POS_UP;
						tmpDst++;
						tempOut[tmpDst] = POS_UP;
						tmpDst++;
						tempOut[tmpDst] = POS_UP;
						tmpDst++;
						tempOut[tmpDst] = POS_UP;
						tmpDst++;
						tempOut[tmpDst] = POS_UP;
						tmpDst++;
						tempOut[tmpDst] = POS_UP;
						tmpDst++;
						tempOut[tmpDst] = POS_UP;
						tmpDst++;
						tempOut[tmpDst] = POS_UP;
						tmpDst++;
						tempOut[tmpDst] = POS_UP;
						tmpDst++;
						tempOut[tmpDst] = POS_UP;
						tmpDst++;
						break;
					case 3:
						tempOut[tmpDst] = POS_UP;
						tmpDst++;
						tempOut[tmpDst] = POS_UP;
						tmpDst++;
						tempOut[tmpDst] = POS_UP;
						tmpDst++;
						tempOut[tmpDst] = POS_UP;
						tmpDst++;
						tempOut[tmpDst] = POS_UP;
						tmpDst++;
						tempOut[tmpDst] = POS_UP;
						tmpDst++;
						tempOut[tmpDst] = POS_UP;
						tmpDst++;
						tempOut[tmpDst] = POS_UP;
						tmpDst++;
						tempOut[tmpDst] = POS_UP;
						tmpDst++;
						tempOut[tmpDst] = POS_UP;
						tmpDst++;
						tempOut[tmpDst] = POS_UP;
						tmpDst++;
						tempOut[tmpDst] = POS_UP;
						tmpDst++;
						tempOut[tmpDst] = POS_UP;
						tmpDst++;
						break;
				}
				break;
			case SUPER:
				tmpSize = popSizeStack();
				if(htmlStyle > 0)
					tmpColor = popColorStack();
				
				updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
				
				tempOut[tmpDst] = POS_DOWN;
				tmpDst++;
				tempOut[tmpDst] = POS_DOWN;
				tmpDst++;
				
				if(tmpSize == 0 || tmpSize == 6)
				{
					tempOut[tmpDst] = POS_DOWN;
					tmpDst++;
					tempOut[tmpDst] = POS_DOWN;
					tmpDst++;
					tempOut[tmpDst] = POS_DOWN;
					tmpDst++;
				}
				break;
			case OL:
				olCount = popOLStack();
				ulDepth--;
				
				if(ulDepth < 0)
					ulDepth = 0;
				
				break;
			case UL:
				olCount = popOLStack();
				ulDepth--;
				
				if(ulDepth < 0)
					ulDepth = 0;
				
				break;
			case B:
				tmpBold = false;
				
				if(htmlStyle > 0)
					tmpColor = popColorStack();				
				
				updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
				break;
			case I:
				tmpItalics = false;
				
				if(htmlStyle > 0)
					tmpColor = popColorStack();
				
				updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
				break;
			case EM:
				tmpItalics = false;
				
				if(htmlStyle > 0)
					tmpColor = popColorStack();
				
				updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
			case STRONG:
				tmpBold = false;
				
				if(htmlStyle > 0)
					tmpColor = popColorStack();
				
				updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
				break;
			case CITE:
				tmpItalics = false;
				
				if(htmlStyle > 0)
					tmpColor = popColorStack();
				
				updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
				break;
			case DFN:
				tmpItalics = false;
				
				if(htmlStyle > 0)
					tmpColor = popColorStack();
				
				updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
				break;
			case VAR:
				tmpItalics = false;
				
				if(htmlStyle > 0)
					tmpColor = popColorStack();
				
				updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
				break;
			case U:
				tmpUnderline = false;
				
				if(htmlStyle > 0)
					tmpColor = popColorStack();
				
				updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
				break;
			case STRIKE:
				tmpStrikeThrough = false;
				
				if(htmlStyle > 0)
					tmpColor = popColorStack();
				
				updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
				break;
			case FONT:
				tmpColor = popColorStack();
				
				updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
				break;
			case A:
				tmpColor = popColorStack();
				tmpUnderline = oldUnderline;
				tmpA = 0;
				
				updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
				break;
			case FRAMESET:
				framesCount--;
				
				if(framesCount < 0)
					framesCount = 0;
				break;
			case NOFRAMES:
				if(framesCount > 0)
					inNoFrames = false;
				break;
			case SCRIPT:
				inScript = false;
				break;
			case STYLE:
				inStyle = false;
				break;
			case DIV:
				if(htmlStyle > 0)
				{
					tmpColor = popColorStack();
					
					updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
				}
				break;
			case SPAN:
				if(htmlStyle > 0)
				{
					tmpColor = popColorStack();
					
					updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
				}
				break;
			case P:
				if(htmlStyle > 0)
				{
					tmpColor = popColorStack();
					
					updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
				}
				break;
		}
		
		tVal = popItemStack();
	}
}

int getNextColor(HTML_RENDERED *htmlPage)
{
	if(htmlPage->maxColors + C_START >= 512)
	{
		return 512;
	}
	else if(htmlPage->maxColors + C_START < 0)
	{
		return C_START;
	}
	
	int tColor = htmlPage->maxColors + C_START; // start with the first non-special element	
	htmlPage->maxColors++;
	
	return tColor;
}

int getCSSElement(char *className, char *elementName, int elementType, HTML_RENDERED *htmlPage)
{
	// if they didn't specify anything, we must assume its just a normal element of elementType
	if(!className)
		return elementType;
	
	// same goes for blank elements
	if(strlen(className) == 0)
		return elementType;
	
	// we have a valid element
	// try to find it on the css list and return the right colorindex
	// if it's not found, just create a new color index to be filled later by a downloaded css script
	// and fill that colorindex with the color from the elementtype's color index
	
	char tmpElement[30];
	strncpy(tmpElement, elementName, 30);
	strlwr(tmpElement);
	
	char tmpClass[30];
	strncpy(tmpClass, className, 30);
	strlwr(tmpClass);
	
	if(htmlPage->maxCSSElements == 0) // we don't have any elements added, so it cant possibly be in the list
	{
		htmlPage->cssData = (CSS_ITEM *)trackMalloc(sizeof(CSS_ITEM), "css structure");
		memset(htmlPage->cssData, 0, sizeof(CSS_ITEM));
		
		strncpy(htmlPage->cssData[0].cssElement, tmpElement, 30);
		strncpy(htmlPage->cssData[0].cssClass, tmpClass, 30);
		htmlPage->cssData[0].colorIndex = getNextColor(htmlPage);
		
		// set the color to a default for display until the css page is loaded
		htmlPage->colors[htmlPage->cssData[0].colorIndex] = htmlPage->colors[elementType];
		htmlPage->cssData[0].relativeIndex = elementType;
		htmlPage->cssData[0].hasFilled = false;
		
		htmlPage->maxCSSElements = 1;
		
		// give them back a color to work with
		return htmlPage->cssData[0].colorIndex;
	}
	else
	{
		for(int i=0;i<htmlPage->maxCSSElements;i++)
		{
			if(strcmp(tmpElement, htmlPage->cssData[i].cssElement) == 0 && strcmp(tmpClass, htmlPage->cssData[i].cssClass) == 0)
			{	
				// found the element, return the colorindex
				return htmlPage->cssData[i].colorIndex;
			}
		}
		
		// we didn't find it by class and element, so lets try just by class
		
		for(int i=0;i<htmlPage->maxCSSElements;i++)
		{
			if(strcmp(tmpClass, htmlPage->cssData[i].cssClass) == 0 && strlen(htmlPage->cssData[i].cssElement) == 0)
			{	
				// found the element, return the colorindex
				return htmlPage->cssData[i].colorIndex;
			}
		}
		
		// if we hit here, that means we never found the element
		// lets create a new one
		
		htmlPage->maxCSSElements++;
		
		htmlPage->cssData = (CSS_ITEM *)trackRealloc(htmlPage->cssData, sizeof(CSS_ITEM) * htmlPage->maxCSSElements);
		memset(&htmlPage->cssData[htmlPage->maxCSSElements - 1], 0, sizeof(CSS_ITEM));
		
		strncpy(htmlPage->cssData[htmlPage->maxCSSElements - 1].cssElement, tmpElement, 30);
		strncpy(htmlPage->cssData[htmlPage->maxCSSElements - 1].cssClass, tmpClass, 30);
		
		htmlPage->cssData[htmlPage->maxCSSElements - 1].colorIndex = getNextColor(htmlPage);
		
		// set the color to a default for display until the css page is loaded
		htmlPage->colors[htmlPage->cssData[htmlPage->maxCSSElements - 1].colorIndex] = htmlPage->colors[elementType];
		htmlPage->cssData[htmlPage->maxCSSElements - 1].relativeIndex = elementType;
		htmlPage->cssData[htmlPage->maxCSSElements - 1].hasFilled = false;
		
		// give them back a color to work with
		return htmlPage->cssData[htmlPage->maxCSSElements - 1].colorIndex;
	}
}

char asciiFromUnicode(int tByte)
{
	if(tByte < 256)
	{
		return (char)tByte;
	}
	
	switch(tByte)
	{
		case 0x2013:
			return 0x96;
		case 0x2014:
			return 0x97;
		case 0x2019:
			return 0x27;
		case 0x2022:
			return 0x95;
		default:														
			return '?';
	}
} 

char convertUTF(char *input, int *pos)
{
	if(!(input[(*pos)] & 0x80)) // single byte
	{	
		return input[(*pos)];
	}
	else if((input[(*pos)] & 0xE0) == 0xC0) // two bytes
	{
		int tByte = ((input[(*pos)] & 0x1F) << 6) | (input[(*pos) + 1] & 0x3F);
		(*pos)++; // incriment one now, one later
		
		return asciiFromUnicode(tByte);
	}
	else if((input[(*pos)] & 0xF0) == 0xE0) // three bytes
	{
		int tByte = ((input[(*pos)] & 0x0F) << 12) | ((input[(*pos) + 1] & 0x3F) << 6) | (input[(*pos) + 2] & 0x3F);		
		(*pos) += 2; // incriment two now, one later
		
		return asciiFromUnicode(tByte);
	}
	else if((input[(*pos)] & 0xF8) == 0xF0) // four bytes (no current known needed characters)
	{
		int tByte = ((input[(*pos)] & 0x07) << 15) | ((input[(*pos) + 1] & 0x3F) << 12) | ((input[(*pos) + 2] & 0x3F) << 6) | (input[(*pos) + 3] & 0x3F);
		(*pos) += 3; // incriment two now, one later
		
		return asciiFromUnicode(tByte);
	}
	
	return '?';
}

char convert8859(char *input, int *pos)
{
	switch(input[(*pos)])
	{
		case 0x91:
		case 0x92:
			// Quote
			return '\'';
	}
	
	return input[(*pos)];
}

void loadHTMLFromMemory(char *tempStr, HTML_RENDERED *htmlPage)
{	
	char *tempOut = tempStr;
	
	if(!tempOut)
	{
		char *t = "<html><head><title>Error with Renderer!</title></head><body><h1>Error!</h1><p>Not enough memory to render!</p></body></html>";
		
		tempStr = (char *)trackMalloc(strlen(t)+2048+1, "tmp render");
		memcpy(tempStr+2048, t, strlen(t)+1);
		memset(tempStr, ' ', 2048);
		
		tempOut = tempStr;
	}
	
	tPage = htmlPage;
	
	bool tmpBold = false;
	bool tmpItalics = false;
	bool tmpUnderline = false;
	bool tmpStrikeThrough = false;
	bool oldUnderline = false;
	bool firstTag = false;
	bool inTag = false;
	bool inCode = false;
	bool inAmpersand = false;
	bool inScript = false;
	bool inStyle = false;
	bool inBody = false;
	bool inProperty = false;
	bool noTitles = false;
	bool wasNameTag = false;
	bool inSelect = false;
	bool inNoFrames = false;
	bool isPElement = false;
	bool needsSpacing = false;
	bool wasEquals = false;
	bool inLI = false;
	bool cssTag = false;
	bool wasHead = false;
	
	char *tag = (char *)safeMalloc(1025);
	char *element = (char *)safeMalloc(1025);
	char *tmpTag = (char *)safeMalloc(1025);
	
	u32 crlfLoc[NUM_CRLFS];
	memset(crlfLoc, 0, sizeof(u32) * NUM_CRLFS);
	
	DUPLICATE_IMAGE_TYPE *duplicateImages = NULL;
	u32 duplicateImageCount = 0;
	
	u16 cellSpacing = DEFAULT_TABLE_SPACING;
	u16 cellPadding = DEFAULT_TABLE_PADDING;
	u32 tmpA = 0;
	u32 whichForm = 0;
	u32 curLevel = 0;
	u32 lastTagStart = 0;
	uint16 tmpColor = 0;
	int tmpSize = 1;
	int tTagLen = 0;
	int tmpSrc = 0;
	int tmpDst = 0;
	int framesCount = 0;
	char *cssBuf = NULL;
	
	initItemStack();
	initColorStack();
	initSizeStack();
	initOLStack();
	freeHTML(htmlPage);
	
	initFakeSprite();
	
	memset(tag, 0, 1024);
	memset(element, 0, 1024);
	
	tPage->controlCodes = NULL;
	controlPos = 0;
	tPage->maxControls = 0;
	ulDepth = 0;
	olCount = 0;
	focusControl = 0;
	
	// set up colors
	htmlPage->colors[C_BG] = 0xffff;
	htmlPage->colors[C_LINK] = 0x7c00;
	htmlPage->colors[C_ALINK] = 0x4010;
	htmlPage->colors[C_VLINK] = 0x4010;
	htmlPage->colors[C_DEFAULT] = 0x0000;
	
	htmlPage->colors[F_BORDER] = 0x0000;
	htmlPage->colors[F_TEXT] = 0x0000;
	htmlPage->colors[F_BG] = 0x7BDE;
	
	// customizable colors afterwards
	htmlPage->maxColors = 0;
	
	// normal stuff
	htmlPage->maxControls = 0;
	htmlPage->urlCodes = NULL;
	htmlPage->formCodes = NULL;
	htmlPage->formData = NULL;
	htmlPage->maxFormControls = 0;
	htmlPage->maxCSSElements = 0;
	htmlPage->maxURLs = 0;
	htmlPage->maxWraps = 0;
	htmlPage->maxForms = 0;
	htmlPage->cssLocation = NULL;
	
	if(htmlPage->mS != 0)
		*(htmlPage->mS) = strlen(tempStr);
	
	if(htmlPage->content == TYPE_PLAINTEXT)
	{
		// i know this is ghetto, but it forces the html renderer to do the parsing work
		
		tempStr[2048 - 6] = '<';
		tempStr[2048 - 5] = 'c';
		tempStr[2048 - 4] = 'o';
		tempStr[2048 - 3] = 'd';
		tempStr[2048 - 2] = 'e';
		tempStr[2048 - 1] = '>';
	}
	
	while(tempStr[tmpSrc] != 0)
	{
		if((tempStr[tmpSrc] != 13 && tempStr[tmpSrc] != 10) || inCode)
		{	
			if(!inTag)
			{				
				if(cssTag)
				{
					// append to css buffer
					int x = strlen(cssBuf);
					
					if(x < (CSS_SIZE - 1))
					{
						cssBuf[x] = tempStr[tmpSrc];
						cssBuf[x+1] = 0;
					}
				}
				if(tempStr[tmpSrc] == '<' && (htmlPage->content == TYPE_HTML || (htmlPage->content == TYPE_PLAINTEXT && !inCode)))
				{	
					if(tempStr[tmpSrc+1] == '!' && tempStr[tmpSrc+2] == '-' && tempStr[tmpSrc+3] == '-') // comment!
					{	
						bool hasFound = false;
						
						while(!hasFound) // skip tag
						{
							if(tempStr[tmpSrc] == '-' && tempStr[tmpSrc+1] == '-' && tempStr[tmpSrc+2] == '>')
							{
								hasFound = true;
								tmpSrc +=2; // don't skip last one because it happens at the end of the loop
							}
							else
							{
								if(tempStr[tmpSrc] == 0)
								{
									hasFound = true;
									tmpSrc--; // to re-find later
								}
								else
								{
									tmpSrc++;
								}
							}
						}
					}
					else
					{					
						if((inScript || inStyle) && tempStr[tmpSrc + 1] != '/')
						{
							// ignore this cuz it's script crap
						}
						else						
						{
							inTag = true;
							inProperty = false;
							wasEquals = false;
							memset(tag, 0, 1024);
							memset(crlfLoc, 0, sizeof(u32) * NUM_CRLFS);
							tTagLen = 0;
							
							lastTagStart = tmpSrc + 1; // point to start of tag
						}
					}
				}
				else
				{
					if(!noTitles && isTitle())
					{
						int tStore = strlen(htmlPage->title);
						if(tStore < 255)
						{
							if(inAmpersand)
							{
								if(tempStr[tmpSrc] == ';')
								{
									// end & tag
									
									htmlPage->title[tStore] = convertSpecial(tag);
									inAmpersand = false;
								}
								else
								{
									if(tTagLen < 1023)
										tag[tTagLen++] = tempStr[tmpSrc];
								}
							}
							else
							{
								bool shouldEncode = true;
								
								if(tempStr[tmpSrc] == '&' && tempStr[tmpSrc + 1] != ' ')
								{
									// do a lazy test for stupid people like the guy at textfiles.com who cant write html for shit
									shouldEncode = false;
									
									for(int i=tmpSrc+1;i<tmpSrc+10;i++)
									{
										if(tempStr[i] == ';')
										{									
											memset(tag, 0, 1024);							
											inAmpersand = true;
											tTagLen = 0;
											break;
										}
										
										if(tempStr[i] == 0 || tempStr[i] == '<' || tempStr[i] == '"')
										{
											shouldEncode = true;
											break;
										}
									}
								}
								
								if(shouldEncode)
								{
									switch(htmlPage->encoding)
									{
										case CHARSET_ASCII:
											htmlPage->title[tStore] = tempStr[tmpSrc];
											
											break;
										case CHARSET_UTF8:
											htmlPage->title[tStore] = convertUTF(tempStr, &tmpSrc);
											
											break;
										case CHARSET_8859:
											htmlPage->title[tStore] = convert8859(tempStr, &tmpSrc);
											
											break;
									}
								}
							}
						}
					}
					else if(inBody || wasHead || isBody())
					{						
						if((!framesCount || (framesCount && inNoFrames)) && !inScript && !inStyle)
						{
							if(inAmpersand)
							{
								if(tempStr[tmpSrc] == ';')
								{
									// end & tag
									
									tempOut[tmpDst] = convertSpecial(tag);
									
									tmpDst++;							
									inAmpersand = false;
								}
								else
								{
									if(tTagLen < 1023)
										tag[tTagLen++] = tempStr[tmpSrc];
								}
							}
							else
							{
								bool shouldEncode = true;
								
								if(tempStr[tmpSrc] == '&' && tempStr[tmpSrc + 1] != ' ' && htmlPage->content == TYPE_HTML)
								{
									// do a lazy test for stupid people like the guy at textfiles.com who cant write html for shit
									shouldEncode = false;
									
									for(int i=tmpSrc+1;i<tmpSrc+10;i++)
									{
										if(tempStr[i] == ';')
										{									
											memset(tag, 0, 1024);							
											inAmpersand = true;
											tTagLen = 0;
											break;
										}
										
										if(tempStr[i] == 0 || tempStr[i] == '<' || tempStr[i] == '"')
										{
											shouldEncode = true;
											break;
										}
									}
								}
								
								if(shouldEncode)
								{
									if(tempStr[tmpSrc] == 13 || tempStr[tmpSrc] == 10 || tempStr[tmpSrc] == FORM_IDENTIFIER)
									{
										// this can only be hit when we are in code, so no need to test for it
										
										if((tempStr[tmpSrc] == 13 && tempStr[tmpSrc+1] != 10) || tempStr[tmpSrc] == 10)
										{
											tempOut[tmpDst] = '\n';
											tmpDst++;
										}
									}								
									else
									{
										if(!inCode && ((tmpDst > 0 && tempStr[tmpSrc] == ' ' && tempOut[tmpDst-1] == ' ') || tempStr[tmpSrc] == '	')) 
										{
											// do nothing still
										}
										else
										{
											switch(htmlPage->encoding)
											{
												case CHARSET_ASCII:
													tempOut[tmpDst] = tempStr[tmpSrc];
													tmpDst++;
													
													break;
												case CHARSET_UTF8:
													tempOut[tmpDst] = convertUTF(tempStr, &tmpSrc);
													tmpDst++;
													
													break;
												case CHARSET_8859:
													tempOut[tmpDst] = convert8859(tempStr, &tmpSrc);
													tmpDst++;
													
													break;
											}
										}
									}
								}
							}
						}
					}
				}
			}
			else
			{
				if(tempStr[tmpSrc] == '>' && !inProperty)
				{
					// handle tag finished					
					separateTagFromElements(tag, element);
					strlwr(tag);
					
					if(tag[0] != '/') // open tag
					{
						if(allowURLS)
						{					
							if(strcmp(tag, "form") == 0)
							{
								whichForm++;
								htmlPage->maxForms = whichForm;
								
								if(whichForm == 1)
									htmlPage->formData = (FORM_SUBMIT_ITEM *)trackMalloc(sizeof(FORM_SUBMIT_ITEM), "forms data");
								else
									htmlPage->formData = (FORM_SUBMIT_ITEM *)trackRealloc(htmlPage->formData, sizeof(FORM_SUBMIT_ITEM) * htmlPage->maxForms);
								
								memset(&htmlPage->formData[whichForm-1], 0, sizeof(FORM_SUBMIT_ITEM));
								
								memset(tmpTag, 0, 1025);
								
								// url hack to ensure any length url without taking up memory
								char *tmpLoc = cistrstr(element, "action");
								
								if(tmpLoc)
								{
									int placeIn = tmpLoc - element + strlen(tag) + 7; // place into the tag relative to the start of the tag;
									
									placeIn += lastTagStart; // now it points absolutely to the string start where it can be found
									
									getTagLocations(cistrstr(element, "action") + 6, htmlPage->formData[whichForm-1].url.startPos, htmlPage->formData[whichForm-1].url.endPos);
									
									htmlPage->formData[whichForm-1].url.startPos += placeIn;
									htmlPage->formData[whichForm-1].url.endPos += placeIn;
									
									for(int tF=0;tF<NUM_CRLFS;tF++)
									{
										if(crlfLoc[tF] != 0 && crlfLoc[tF] <= htmlPage->formData[whichForm-1].url.startPos)
										{
											htmlPage->formData[whichForm-1].url.startPos++;
											htmlPage->formData[whichForm-1].url.endPos++;
										}
									}
								}
								
								memset(tmpTag, 0, 1025);
								getElementText(element, "method", tmpTag);
								
								strlwr(tmpTag);
								
								if(strcmp(tmpTag, "post") == 0)
									htmlPage->formData[whichForm-1].method = M_POST;
								else
									htmlPage->formData[whichForm-1].method = M_GET;
								
								goto endOfLoop; // nasty, but skips doing all the if's later
							}
							
							else if(strcmp(tag, "input") == 0)
							{	
								htmlPage->maxFormControls++;
								
								if(htmlPage->maxFormControls == 1)
									htmlPage->formCodes = (FORM_ITEM *)trackMalloc(sizeof(FORM_ITEM), "forms data");
								else
									htmlPage->formCodes = (FORM_ITEM *)trackRealloc(htmlPage->formCodes, sizeof(FORM_ITEM) * htmlPage->maxFormControls);
								
								memset(&htmlPage->formCodes[htmlPage->maxFormControls-1], 0, sizeof(FORM_ITEM));
								
								htmlPage->formCodes[htmlPage->maxFormControls-1].formID = whichForm;
								
								memset(tmpTag, 0, 1025);
								
								getElementText(element, "name", tmpTag);							
								strncpy(htmlPage->formCodes[htmlPage->maxFormControls-1].name, tmpTag, 63);
								
								memset(tmpTag, 0, 1025);
								getElementText(element, "type", tmpTag);
								strlwr(tmpTag);
								
								if(strlen(tmpTag) == 0 || strcmp(tmpTag, "text") == 0 || strcmp(tmpTag, "password") == 0) // default is text
								{
									if(strcmp(tmpTag, "password") == 0)
										htmlPage->formCodes[htmlPage->maxFormControls-1].whatType = FORM_PASSWORD;
									else
										htmlPage->formCodes[htmlPage->maxFormControls-1].whatType = FORM_TEXT;
									
									memset(tmpTag, 0, 1025);
									getElementText(element, "size", tmpTag);
									strlwr(tmpTag);
									
									htmlPage->formCodes[htmlPage->maxFormControls-1].width = getNumber(tmpTag);
									
									if(!htmlPage->formCodes[htmlPage->maxFormControls-1].width)
										htmlPage->formCodes[htmlPage->maxFormControls-1].width = 128;
									else
										htmlPage->formCodes[htmlPage->maxFormControls-1].width *= 10;
									
									if(htmlPage->formCodes[htmlPage->maxFormControls-1].width > (htmlPage->destWidth - 5))
										htmlPage->formCodes[htmlPage->maxFormControls-1].width = htmlPage->destWidth - 5;
										
									htmlPage->formCodes[htmlPage->maxFormControls-1].height = 15;
									
									memset(tmpTag, 0, 1025);
									getElementText(element, "maxlength", tmpTag);
									strlwr(tmpTag);
									
									htmlPage->formCodes[htmlPage->maxFormControls-1].maxLength = getNumber(tmpTag);
									
									if(!htmlPage->formCodes[htmlPage->maxFormControls-1].maxLength)
										htmlPage->formCodes[htmlPage->maxFormControls-1].maxLength = 255;
									
									htmlPage->formCodes[htmlPage->maxFormControls-1].state = trackMalloc(htmlPage->formCodes[htmlPage->maxFormControls-1].maxLength + 1, "textbox");
									memset(htmlPage->formCodes[htmlPage->maxFormControls-1].state, 0, htmlPage->formCodes[htmlPage->maxFormControls-1].maxLength + 1);
									
									getElementText(element, "value", (char *)htmlPage->formCodes[htmlPage->maxFormControls-1].state);
									
									htmlPage->formCodes[htmlPage->maxFormControls-1].resetState = trackMalloc(strlen((char *)htmlPage->formCodes[htmlPage->maxFormControls-1].state) + 1, "textbox reset");
									memcpy(htmlPage->formCodes[htmlPage->maxFormControls-1].resetState, htmlPage->formCodes[htmlPage->maxFormControls-1].state, strlen((char *)htmlPage->formCodes[htmlPage->maxFormControls-1].state) + 1);
									
									if(tmpDst > 0 && tempOut[tmpDst-1] != ' ' && tempOut[tmpDst-1] != '\n')
									{						
										tempOut[tmpDst] = ' ';
										tmpDst++;						
									}
									
									updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, htmlPage->maxFormControls);
									
									tempOut[tmpDst] = FORM_IDENTIFIER;
									tmpDst++;
									
									updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
									
									tempOut[tmpDst] = ' ';
									tmpDst++;
									
									goto endOfLoop; // nasty, but skips doing all the if's later
								}
								
								if(strcmp(tmpTag, "checkbox") == 0 || strcmp(tmpTag, "radio") == 0)
								{
									if(strcmp(tmpTag, "checkbox") == 0)
										htmlPage->formCodes[htmlPage->maxFormControls-1].whatType = FORM_CHECK;
									else
										htmlPage->formCodes[htmlPage->maxFormControls-1].whatType = FORM_RADIO;
									
									memset(tmpTag, 0, 1025);
									strcpy(tmpTag, element);
									
									strlwr(tmpTag);
									
									if(htmlPage->formCodes[htmlPage->maxFormControls-1].whatType == FORM_RADIO) // make sure all others are unchecked
									{
										if(strstr(tmpTag, "checked"))
										{
											htmlPage->formCodes[htmlPage->maxFormControls-1].maxLength = 1;
											htmlPage->formCodes[htmlPage->maxFormControls-1].resetState = (void *)1;
											
											if(htmlPage->maxFormControls > 1)
											{
												for(int ci=0;ci<htmlPage->maxFormControls-1;++ci)
												{
													if(htmlPage->formCodes[ci].whatType == FORM_RADIO && htmlPage->formCodes[ci].formID == htmlPage->formCodes[htmlPage->maxFormControls-1].formID)
													{
														if(strcmp(htmlPage->formCodes[htmlPage->maxFormControls-1].name, htmlPage->formCodes[ci].name) == 0)
														{
															htmlPage->formCodes[ci].maxLength = 0;
															htmlPage->formCodes[ci].resetState = (void *)0;
														}
													}
												}
											}
										}
										else
										{
											htmlPage->formCodes[htmlPage->maxFormControls-1].maxLength = 0;
											htmlPage->formCodes[htmlPage->maxFormControls-1].resetState = (void *)0;
										}
										
										htmlPage->formCodes[htmlPage->maxFormControls-1].state = trackMalloc(256, "radio");
										memset(htmlPage->formCodes[htmlPage->maxFormControls-1].state, 0, 256);
										
										getElementText(element, "value", (char *)htmlPage->formCodes[htmlPage->maxFormControls-1].state);
									}
									else
									{
										if(strstr(tmpTag, "checked"))
											htmlPage->formCodes[htmlPage->maxFormControls-1].state = (void *)1;
										else
											htmlPage->formCodes[htmlPage->maxFormControls-1].state = (void *)0;
										
										htmlPage->formCodes[htmlPage->maxFormControls-1].resetState = htmlPage->formCodes[htmlPage->maxFormControls-1].state;
									}
									
									htmlPage->formCodes[htmlPage->maxFormControls-1].width = 15;									
									htmlPage->formCodes[htmlPage->maxFormControls-1].height = 15;
									
									if(tmpDst > 0 && tempOut[tmpDst-1] != ' ' && tempOut[tmpDst-1] != '\n')
									{						
										tempOut[tmpDst] = ' ';
										tmpDst++;						
									}
									
									updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, htmlPage->maxFormControls);
									
									tempOut[tmpDst] = FORM_IDENTIFIER;
									tmpDst++;
									
									updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
									
									tempOut[tmpDst] = ' ';
									tmpDst++;
									
									goto endOfLoop; // nasty, but skips doing all the if's later
								}
								
								if(strcmp(tmpTag, "hidden") == 0)
								{
									htmlPage->formCodes[htmlPage->maxFormControls-1].whatType = FORM_HIDDEN;
									
									memset(tmpTag, 0, 1025);
									getElementText(element, "value", tmpTag);
									
									htmlPage->formCodes[htmlPage->maxFormControls-1].state = trackMalloc(strlen(tmpTag)+1, "hidden");
									memcpy(htmlPage->formCodes[htmlPage->maxFormControls-1].state, tmpTag, strlen(tmpTag)+1);
									
									goto endOfLoop; // nasty, but skips doing all the if's later
								}
								
								if(strcmp(tmpTag, "submit") == 0 || strcmp(tmpTag, "reset") == 0 || strcmp(tmpTag, "button") == 0)
								{
									char tTest[32];
									memcpy(tTest, tmpTag, 32);
									
									memset(tmpTag, 0, 1025);
									getElementText(element, "value", tmpTag);
									
									if(strcmp(tTest, "submit") == 0)
									{
										if(strlen(tmpTag) == 0)
											strcpy(tmpTag, "Submit");
										htmlPage->formCodes[htmlPage->maxFormControls-1].whatType = FORM_SUBMIT;
									}
									else if(strcmp(tTest, "reset") == 0)
									{
										if(strlen(tmpTag) == 0)
											strcpy(tmpTag, "Reset");
										htmlPage->formCodes[htmlPage->maxFormControls-1].whatType = FORM_RESET;
									}
									else
									{
										if(strlen(tmpTag) == 0)
											strcpy(tmpTag, "Button");
										htmlPage->formCodes[htmlPage->maxFormControls-1].whatType = FORM_BUTTON;
									}
									
									htmlPage->formCodes[htmlPage->maxFormControls-1].state = trackMalloc(strlen(tmpTag)+1, "button");
									memcpy(htmlPage->formCodes[htmlPage->maxFormControls-1].state, tmpTag, strlen(tmpTag)+1);
									
									htmlPage->formCodes[htmlPage->maxFormControls-1].resetState = trackMalloc(strlen(tmpTag)+1, "button");
									memcpy(htmlPage->formCodes[htmlPage->maxFormControls-1].resetState, tmpTag, strlen(tmpTag)+1);
									
									if(tmpDst > 0 && tempOut[tmpDst-1] != ' ' && tempOut[tmpDst-1] != '\n')
									{						
										tempOut[tmpDst] = ' ';
										tmpDst++;						
									}
									
									updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, htmlPage->maxFormControls);
									
									tempOut[tmpDst] = FORM_IDENTIFIER;
									tmpDst++;
									
									updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
									
									tempOut[tmpDst] = ' ';
									tmpDst++;
									
									htmlPage->formCodes[htmlPage->maxFormControls-1].width = getStringWidth(tmpTag, font_gautami_10) + 10;									
									htmlPage->formCodes[htmlPage->maxFormControls-1].height = 15;
									
									goto endOfLoop; // nasty, but skips doing all the if's later
								}
								
								goto endOfLoop; // nasty, but skips doing all the if's later
							}
							
							else if(strcmp(tag, "button") == 0)
							{
								htmlPage->maxFormControls++;
								
								if(htmlPage->maxFormControls == 1)
									htmlPage->formCodes = (FORM_ITEM *)trackMalloc(sizeof(FORM_ITEM), "forms data");
								else
									htmlPage->formCodes = (FORM_ITEM *)trackRealloc(htmlPage->formCodes, sizeof(FORM_ITEM) * htmlPage->maxFormControls);
								
								memset(&htmlPage->formCodes[htmlPage->maxFormControls-1], 0, sizeof(FORM_ITEM));
								
								htmlPage->formCodes[htmlPage->maxFormControls-1].formID = whichForm;
								
								memset(tmpTag, 0, 1025);
								
								getElementText(element, "name", tmpTag);							
								strncpy(htmlPage->formCodes[htmlPage->maxFormControls-1].name, tmpTag, 63);
								
								memset(tmpTag, 0, 1025);
								getElementText(element, "value", tmpTag);
								
								htmlPage->formCodes[htmlPage->maxFormControls-1].resetState = trackMalloc(strlen(tmpTag)+1, "button");
								memcpy(htmlPage->formCodes[htmlPage->maxFormControls-1].resetState, tmpTag, strlen(tmpTag)+1);
								
								memset(tmpTag, 0, 1025);
								getElementText(element, "type", tmpTag);
								strlwr(tmpTag);
								
								if(strcmp(tmpTag, "reset") == 0)
									htmlPage->formCodes[htmlPage->maxFormControls-1].whatType = FORM_RESET;
								else if(strcmp(tmpTag, "button") == 0)
									htmlPage->formCodes[htmlPage->maxFormControls-1].whatType = FORM_BUTTON;
								else
									htmlPage->formCodes[htmlPage->maxFormControls-1].whatType = FORM_SUBMIT;
								
								htmlPage->formCodes[htmlPage->maxFormControls-1].state = trackMalloc(1024, "button");
								memset(htmlPage->formCodes[htmlPage->maxFormControls-1].state, 0, 1024);
								
								// fill with default value
								
								tmpSrc++;
								int tLen = 0;
								
								inAmpersand = false;
								
								while(true)
								{
									if(tempStr[tmpSrc] == '<')
									{
										char endTag[10];
										
										strncpy(endTag, &tempStr[tmpSrc], 9);
										endTag[9] = 0;
										
										strlwr(endTag);
										
										if(strcmp(endTag, "</button>") == 0) // finished
										{
											((char *)htmlPage->formCodes[htmlPage->maxFormControls-1].state)[tLen] = 0;
											tmpSrc += 8;
											
											break;
										}
									}
									
									if(tempStr[tmpSrc] == 0)
									{
										((char *)htmlPage->formCodes[htmlPage->maxFormControls-1].state)[tLen] = 0;
										tmpSrc--;
										
										break;
									}
									
									if(inAmpersand)
									{
										if(tempStr[tmpSrc] == ';')
										{
											// end & tag
											
											((char *)htmlPage->formCodes[htmlPage->maxFormControls-1].state)[tLen] = convertSpecial(tag);
											tLen++;
											
											inAmpersand = false;
										}
										else
										{
											int tStore = strlen(tag);
											if(tStore < 1023)
												tag[tStore] = tempStr[tmpSrc];
										}
									}
									else
									{
										if(tempStr[tmpSrc] == '&' && tempStr[tmpSrc + 1] != ' ')
										{
											memset(tag, 0, 1024);							
											inAmpersand = true;
										}
										else
										{
											((char *)htmlPage->formCodes[htmlPage->maxFormControls-1].state)[tLen] = tempStr[tmpSrc];
											tLen++;
										}
									}
									
									tmpSrc++;
								}
								
								inAmpersand = false;
								
								if(tmpDst > 0 && tempOut[tmpDst-1] != ' ' && tempOut[tmpDst-1] != '\n')
								{						
									tempOut[tmpDst] = ' ';
									tmpDst++;						
								}
								
								updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, htmlPage->maxFormControls);
								
								tempOut[tmpDst] = FORM_IDENTIFIER;
								tmpDst++;
								
								updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
								
								tempOut[tmpDst] = ' ';
								tmpDst++;
								
								htmlPage->formCodes[htmlPage->maxFormControls-1].width = getStringWidth(tmpTag, font_gautami_10) + 10;								
								htmlPage->formCodes[htmlPage->maxFormControls-1].height = 15;
									
								goto endOfLoop; // nasty, but skips doing all the if's later
							}
							
							else if(strcmp(tag, "textarea") == 0)
							{
								htmlPage->maxFormControls++;
								
								if(htmlPage->maxFormControls == 1)
									htmlPage->formCodes = (FORM_ITEM *)trackMalloc(sizeof(FORM_ITEM), "forms data");
								else
									htmlPage->formCodes = (FORM_ITEM *)trackRealloc(htmlPage->formCodes, sizeof(FORM_ITEM) * htmlPage->maxFormControls);
								
								memset(&htmlPage->formCodes[htmlPage->maxFormControls-1], 0, sizeof(FORM_ITEM));
								
								htmlPage->formCodes[htmlPage->maxFormControls-1].formID = whichForm;
								
								memset(tmpTag, 0, 1025);
								
								getElementText(element, "name", tmpTag);							
								strncpy(htmlPage->formCodes[htmlPage->maxFormControls-1].name, tmpTag, 63);
								
								htmlPage->formCodes[htmlPage->maxFormControls-1].whatType = FORM_TEXTAREA;
								
								memset(tmpTag, 0, 1025);
								getElementText(element, "rows", tmpTag);
								strlwr(tmpTag);
								
								int tNumber = getNumber(tmpTag);
								int numEnters;
								
								if(tNumber <= 0)
									tNumber = 2;
								if(tNumber > 10)
									tNumber = 10;
								
								numEnters = tNumber;
								
								htmlPage->formCodes[htmlPage->maxFormControls-1].height = (tNumber * 12) + 3;
								
								memset(tmpTag, 0, 1025);
								getElementText(element, "cols", tmpTag);
								strlwr(tmpTag);
								
								tNumber = getNumber(tmpTag) * 10;
								
								if(tNumber < 120)
									tNumber = 120;
								if(tNumber > (htmlPage->destWidth - 5))
									tNumber = (htmlPage->destWidth - 5);
								
								htmlPage->formCodes[htmlPage->maxFormControls-1].width = tNumber;								
								htmlPage->formCodes[htmlPage->maxFormControls-1].maxLength = 4096;
									
								htmlPage->formCodes[htmlPage->maxFormControls-1].state = trackMalloc(htmlPage->formCodes[htmlPage->maxFormControls-1].maxLength + 5, "textbox");
								memset(htmlPage->formCodes[htmlPage->maxFormControls-1].state, 0, htmlPage->formCodes[htmlPage->maxFormControls-1].maxLength + 5);
								
								// fill with default value
								
								tmpSrc++;
								int tLen = 4;
								
								inAmpersand = false;
								
								while(true)
								{
									if(tempStr[tmpSrc] == '<')
									{
										char endTag[12];
										
										strncpy(endTag, &tempStr[tmpSrc], 11);
										endTag[11] = 0;
										
										strlwr(endTag);
										
										if(strcmp(endTag, "</textarea>") == 0) // finished
										{
											((char *)htmlPage->formCodes[htmlPage->maxFormControls-1].state)[tLen] = 0;
											tmpSrc += 10;
											
											break;
										}
									}
									
									if(tempStr[tmpSrc] == 0)
									{
										((char *)htmlPage->formCodes[htmlPage->maxFormControls-1].state)[tLen] = 0;
										tmpSrc--;
										
										break;
									}
									
									if(inAmpersand)
									{
										if(tempStr[tmpSrc] == ';')
										{
											// end & tag
											
											((char *)htmlPage->formCodes[htmlPage->maxFormControls-1].state)[tLen] = convertSpecial(tag);
											tLen++;
											
											inAmpersand = false;
										}
										else
										{
											int tStore = strlen(tag);
											if(tStore < 1023)
												tag[tStore] = tempStr[tmpSrc];
										}
									}
									else
									{
										if(tempStr[tmpSrc] == '&' && tempStr[tmpSrc + 1] != ' ')
										{
											memset(tag, 0, 1024);							
											inAmpersand = true;
										}
										else
										{
											((char *)htmlPage->formCodes[htmlPage->maxFormControls-1].state)[tLen] = tempStr[tmpSrc];
											tLen++;
										}
									}
									
									tmpSrc++;
								}
								
								inAmpersand = false;
								
								htmlPage->formCodes[htmlPage->maxFormControls-1].resetState = trackMalloc(strlen((char *)htmlPage->formCodes[htmlPage->maxFormControls-1].state + 4) + 1, "textarea reset");
								memcpy(htmlPage->formCodes[htmlPage->maxFormControls-1].resetState, (char *)htmlPage->formCodes[htmlPage->maxFormControls-1].state + 4, strlen((char *)htmlPage->formCodes[htmlPage->maxFormControls-1].state + 4) + 1);
								
								if(tmpDst > 0 && tempOut[tmpDst-1] != ' ' && tempOut[tmpDst-1] != '\n')
								{						
									tempOut[tmpDst] = ' ';
									tmpDst++;						
								}
								
								updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, htmlPage->maxFormControls);
								
								tempOut[tmpDst] = FORM_IDENTIFIER;
								tmpDst++;
								
								// lets give some space
								updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, 0, tmpColor, false, tmpStrikeThrough, tmpA, 0);
								
								tempOut[tmpDst] = ' ';
								tmpDst++;
								
								goto endOfLoop; // nasty, but skips doing all the if's later
							}
							
							else if(strcmp(tag, "select") == 0)
							{
								htmlPage->maxFormControls++;
								
								if(htmlPage->maxFormControls == 1)
									htmlPage->formCodes = (FORM_ITEM *)trackMalloc(sizeof(FORM_ITEM), "forms data");
								else
									htmlPage->formCodes = (FORM_ITEM *)trackRealloc(htmlPage->formCodes, sizeof(FORM_ITEM) * htmlPage->maxFormControls);
								
								memset(&htmlPage->formCodes[htmlPage->maxFormControls-1], 0, sizeof(FORM_ITEM));
								
								htmlPage->formCodes[htmlPage->maxFormControls-1].formID = whichForm;
								htmlPage->formCodes[htmlPage->maxFormControls-1].resetState = NULL;
								
								memset(tmpTag, 0, 1025);
								
								getElementText(element, "name", tmpTag);							
								strncpy(htmlPage->formCodes[htmlPage->maxFormControls-1].name, tmpTag, 63);
								
								htmlPage->formCodes[htmlPage->maxFormControls-1].whatType = FORM_SELECT;
								
								memset(tmpTag, 0, 1025);
								strcpy(tmpTag, element);
								
								strlwr(tmpTag);
								
								int numEnters = 0;
								FORM_SELECT_ITEM *fsi = (FORM_SELECT_ITEM *)trackMalloc(sizeof(FORM_SELECT_ITEM), "form select struct");
								fsi->items = NULL;
								fsi->cursor = 0;
								fsi->focused = 0;
								
								if(strstr(tmpTag, "multiple")) // it's a list, not a combo
								{
									memset(tmpTag, 0, 1025);
									getElementText(element, "size", tmpTag);
									strlwr(tmpTag);
									
									int tNumber = getNumber(tmpTag);
									
									if(tNumber < 3)
										tNumber = 3;
									if(tNumber > 10)
										tNumber = 10;
									
									numEnters = tNumber;
									
									htmlPage->formCodes[htmlPage->maxFormControls-1].height = (tNumber * 12) + 3;
									fsi->type = 1;
								}
								else
								{
									htmlPage->formCodes[htmlPage->maxFormControls-1].height = 15;
									fsi->type = 0;
								}
								
								htmlPage->formCodes[htmlPage->maxFormControls-1].state = fsi;
								
								htmlPage->formCodes[htmlPage->maxFormControls-1].width = htmlPage->destWidth - 5;
								htmlPage->formCodes[htmlPage->maxFormControls-1].maxLength = 0;
								
								if(tmpDst > 0 && tempOut[tmpDst-1] != ' ' && tempOut[tmpDst-1] != '\n')
								{						
									tempOut[tmpDst] = ' ';
									tmpDst++;						
								}
								
								updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, htmlPage->maxFormControls);
								
								tempOut[tmpDst] = FORM_IDENTIFIER;
								tmpDst++;
								
								updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
								
								tempOut[tmpDst] = ' ';
								tmpDst++;
								
								inSelect = true;
								curLevel = 0;
								
								goto endOfLoop; // nasty, but skips doing all the if's later
							}
							
							else if(strcmp(tag, "optgroup") == 0)
							{
								if(inSelect)
								{
									FORM_SELECT_ITEM *fsi = (FORM_SELECT_ITEM *)htmlPage->formCodes[htmlPage->maxFormControls-1].state;								
									
									if(fsi->type == 1)
									{
										int tmpLength = ++htmlPage->formCodes[htmlPage->maxFormControls-1].maxLength;
										
										if(tmpLength == 1)
										{
											fsi->items = (SELECT_ITEM *)trackMalloc(sizeof(SELECT_ITEM), "select data");
											htmlPage->formCodes[htmlPage->maxFormControls-1].resetState = trackMalloc(1, "select reset");
										}
										else
										{
											fsi->items = (SELECT_ITEM *)trackRealloc(fsi->items, sizeof(SELECT_ITEM) * tmpLength);
											htmlPage->formCodes[htmlPage->maxFormControls-1].resetState = trackRealloc(htmlPage->formCodes[htmlPage->maxFormControls-1].resetState, tmpLength);
										}
										
										((char *)htmlPage->formCodes[htmlPage->maxFormControls-1].resetState)[tmpLength-1] = 0;
										
										curLevel++;
										fsi->items[tmpLength-1].depth = -curLevel;
										
										memset(tmpTag, 0, 1025);
										
										getElementText(element, "label", tmpTag);							
										strncpy(fsi->items[tmpLength-1].caption, tmpTag, 127);
									}
								}
								
								goto endOfLoop; // nasty, but skips doing all the if's later
							}
							
							else if(strcmp(tag, "option") == 0)
							{
								if(inSelect)
								{
									FORM_SELECT_ITEM *fsi = (FORM_SELECT_ITEM *)htmlPage->formCodes[htmlPage->maxFormControls-1].state;								
									
									int tmpLength = ++htmlPage->formCodes[htmlPage->maxFormControls-1].maxLength;
									
									if(tmpLength == 1)
									{
										fsi->items = (SELECT_ITEM *)trackMalloc(sizeof(SELECT_ITEM), "select data");
										if(fsi->type == 1)
										{
											htmlPage->formCodes[htmlPage->maxFormControls-1].resetState = trackMalloc(1, "select reset");
											((char *)htmlPage->formCodes[htmlPage->maxFormControls-1].resetState)[tmpLength-1] = 0;
										}
									}	
									else
									{
										fsi->items = (SELECT_ITEM *)trackRealloc(fsi->items, sizeof(SELECT_ITEM) * tmpLength);
										if(fsi->type == 1)
										{
											htmlPage->formCodes[htmlPage->maxFormControls-1].resetState = trackRealloc(htmlPage->formCodes[htmlPage->maxFormControls-1].resetState, tmpLength);
											((char *)htmlPage->formCodes[htmlPage->maxFormControls-1].resetState)[tmpLength-1] = 0;
										}
									}
									
									fsi->items[tmpLength-1].depth = curLevel;
									fsi->items[tmpLength-1].highlighted = 0;
									
									memset(tmpTag, 0, 1025);
									
									getElementText(element, "value", tmpTag);							
									strncpy(fsi->items[tmpLength-1].value, tmpTag, 63);
									
									memset(fsi->items[tmpLength-1].caption, 0, 128);
									
									memset(tmpTag, 0, 1025);
									strcpy(tmpTag, element);
									
									strlwr(tmpTag);
									if(strstr(tmpTag, "selected"))
									{
										if(fsi->type == 0)
										{
											fsi->cursor = tmpLength-1;
											htmlPage->formCodes[htmlPage->maxFormControls-1].resetState = (void *)fsi->cursor;
										}
										else
										{
											fsi->items[tmpLength-1].highlighted = 1;
											((char *)htmlPage->formCodes[htmlPage->maxFormControls-1].resetState)[tmpLength-1] = 1;
										}
									}
									
									tmpSrc++;
									int tLen = 0;
									
									inAmpersand = false;
									
									while(true)
									{
										if(tempStr[tmpSrc] == '<')
										{
											char endTag[10];
											
											strncpy(endTag, &tempStr[tmpSrc], 9);
											endTag[9] = 0;
											
											strlwr(endTag);
											
											if(strcmp(endTag, "</option>") == 0) // finished
											{
												fsi->items[tmpLength-1].caption[tLen] = 0;
												tmpSrc += 8;
												
												break;
											}
										}
										
										if(tempStr[tmpSrc] == 0)
										{
											fsi->items[tmpLength-1].caption[tLen] = 0;
											tmpSrc--;
											
											break;
										}
										
										if(inAmpersand)
										{
											if(tempStr[tmpSrc] == ';')
											{
												// end & tag
												
												fsi->items[tmpLength-1].caption[tLen] = convertSpecial(tag);
												tLen++;
												
												inAmpersand = false;
											}
											else
											{
												int tStore = strlen(tag);
												if(tStore < 1023)
													tag[tStore] = tempStr[tmpSrc];
											}
										}
										else
										{
											if(tempStr[tmpSrc] == '&' && tempStr[tmpSrc + 1] != ' ')
											{
												memset(tag, 0, 1024);							
												inAmpersand = true;
											}
											else
											{
												fsi->items[tmpLength-1].caption[tLen] = tempStr[tmpSrc];
												tLen++;
											}
										}
										
										tmpSrc++;
									}
									
									inAmpersand = false;
								}
								
								goto endOfLoop; // nasty, but skips doing all the if's later
							}
						}
						
						if(htmlStyle == 0)
						{
							bool used = true;
							
							if(strcmp(tag, "h1") == 0)
							{
								pushItemStack(H1);
								
								if(!inLI)
								{
									if(tmpDst > 0 && tempOut[tmpDst-1] != '\n')
									{						
										tempOut[tmpDst] = '\n';
										tmpDst++;
									}
									
									if(tmpDst > 1 && tempOut[tmpDst-2] != '\n')
									{
										tempOut[tmpDst] = '\n';
										tmpDst++;
									}
								}
							}
							
							else if(strcmp(tag, "h2") == 0)
							{
								pushItemStack(H2);
								
								if(!inLI)
								{
									if(tmpDst > 0 && tempOut[tmpDst-1] != '\n')
									{						
										tempOut[tmpDst] = '\n';
										tmpDst++;
									}
									
									if(tmpDst > 1 && tempOut[tmpDst-2] != '\n')
									{
										tempOut[tmpDst] = '\n';
										tmpDst++;
									}
								}
							}
							
							else if(strcmp(tag, "h3") == 0)
							{
								pushItemStack(H3);
								
								if(!inLI)
								{
									if(tmpDst > 0 && tempOut[tmpDst-1] != '\n')
									{						
										tempOut[tmpDst] = '\n';
										tmpDst++;
									}
									
									if(tmpDst > 1 && tempOut[tmpDst-2] != '\n')
									{
										tempOut[tmpDst] = '\n';
										tmpDst++;
									}
								}
							}
							
							else if(strcmp(tag, "h4") == 0)
							{
								pushItemStack(H4);
								
								if(!inLI)
								{
									if(tmpDst > 0 && tempOut[tmpDst-1] != '\n')
									{						
										tempOut[tmpDst] = '\n';
										tmpDst++;
									}
									
									if(tmpDst > 1 && tempOut[tmpDst-2] != '\n')
									{
										tempOut[tmpDst] = '\n';
										tmpDst++;
									}
								}
							}
							
							else if(strcmp(tag, "h5") == 0)
							{
								pushItemStack(H5);
								
								if(!inLI)
								{
									if(tmpDst > 0 && tempOut[tmpDst-1] != '\n')
									{						
										tempOut[tmpDst] = '\n';
										tmpDst++;
									}
									
									if(tmpDst > 1 && tempOut[tmpDst-2] != '\n')
									{
										tempOut[tmpDst] = '\n';
										tmpDst++;
									}
								}
							}
							
							else if(strcmp(tag, "h6") == 0)
							{
								pushItemStack(H6);
								
								if(!inLI)
								{
									if(tmpDst > 0 && tempOut[tmpDst-1] != '\n')
									{						
										tempOut[tmpDst] = '\n';
										tmpDst++;
									}
									
									if(tmpDst > 1 && tempOut[tmpDst-2] != '\n')
									{
										tempOut[tmpDst] = '\n';
										tmpDst++;
									}
								}
							}
							
							else if(strcmp(tag, "small") == 0)
							{
								pushItemStack(SMALL);
							}
							
							else if(strcmp(tag, "big") == 0)
							{
								pushItemStack(BIG);
							}
							
							else if(strcmp(tag, "tt") == 0)
							{
								pushItemStack(TT);
							}
							
							else if(strcmp(tag, "code") == 0)
							{
								pushItemStack(CODE);
								inCode = true;
							}
							
							else if(strcmp(tag, "pre") == 0)
							{
								pushItemStack(PRE);
								inCode = true;
							}
							
							else
								used = false;
							
							if(used)
								goto endOfLoop; // nasty, but skips doing all the if's later
						}
						else
						{
							bool used = true;
							
							if(strcmp(tag, "h1") == 0)
							{
								pushItemStack(H1);
								
								if(!inLI)
								{
									if(tmpDst > 0 && tempOut[tmpDst-1] != '\n')
									{						
										tempOut[tmpDst] = '\n';
										tmpDst++;
									}
									
									if(tmpDst > 1 && tempOut[tmpDst-2] != '\n')
									{
										tempOut[tmpDst] = '\n';
										tmpDst++;
									}
								}
								
								pushSizeStack(tmpSize);
								tmpSize = 3;
								
								memset(tmpTag, 0, 1025);
								getElementText(element, "class", tmpTag);				
								pushColorStack(tmpColor);
								tmpColor = getCSSElement(tmpTag, "h1", tmpColor, htmlPage);
								
								updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
							}
							
							else if(strcmp(tag, "h2") == 0)
							{
								pushItemStack(H2);
								
								if(!inLI)
								{
									if(tmpDst > 0 && tempOut[tmpDst-1] != '\n')
									{						
										tempOut[tmpDst] = '\n';
										tmpDst++;
									}
									
									if(tmpDst > 1 && tempOut[tmpDst-2] != '\n')
									{
										tempOut[tmpDst] = '\n';
										tmpDst++;
									}
								}
								
								pushSizeStack(tmpSize);
								tmpSize = 5;
								
								memset(tmpTag, 0, 1025);
								getElementText(element, "class", tmpTag);				
								pushColorStack(tmpColor);
								tmpColor = getCSSElement(tmpTag, "h2", tmpColor, htmlPage);
								
								updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
							}
							
							else if(strcmp(tag, "h3") == 0)
							{
								pushItemStack(H3);
								
								if(!inLI)
								{
									if(tmpDst > 0 && tempOut[tmpDst-1] != '\n')
									{						
										tempOut[tmpDst] = '\n';
										tmpDst++;
									}
									
									if(tmpDst > 1 && tempOut[tmpDst-2] != '\n')
									{
										tempOut[tmpDst] = '\n';
										tmpDst++;
									}
								}
								
								pushSizeStack(tmpSize);
								tmpSize = 2;
								
								memset(tmpTag, 0, 1025);
								getElementText(element, "class", tmpTag);				
								pushColorStack(tmpColor);
								tmpColor = getCSSElement(tmpTag, "h3", tmpColor, htmlPage);
								
								updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
							}
							
							else if(strcmp(tag, "h4") == 0)
							{
								pushItemStack(H4);
								
								if(!inLI)
								{
									if(tmpDst > 0 && tempOut[tmpDst-1] != '\n')
									{						
										tempOut[tmpDst] = '\n';
										tmpDst++;
									}
									
									if(tmpDst > 1 && tempOut[tmpDst-2] != '\n')
									{
										tempOut[tmpDst] = '\n';
										tmpDst++;
									}
								}
								
								pushSizeStack(tmpSize);
								tmpSize = 4;
								
								memset(tmpTag, 0, 1025);
								getElementText(element, "class", tmpTag);				
								pushColorStack(tmpColor);
								tmpColor = getCSSElement(tmpTag, "h4", tmpColor, htmlPage);
								
								updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
							}
							
							else if(strcmp(tag, "h5") == 0)
							{
								pushItemStack(H5);
								
								if(!inLI)
								{
									if(tmpDst > 0 && tempOut[tmpDst-1] != '\n')
									{						
										tempOut[tmpDst] = '\n';
										tmpDst++;
									}
									
									if(tmpDst > 1 && tempOut[tmpDst-2] != '\n')
									{
										tempOut[tmpDst] = '\n';
										tmpDst++;
									}
								}
								
								pushSizeStack(tmpSize);
								tmpSize = 1;
								
								memset(tmpTag, 0, 1025);
								getElementText(element, "class", tmpTag);				
								pushColorStack(tmpColor);
								tmpColor = getCSSElement(tmpTag, "h5", tmpColor, htmlPage);
								
								updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
							}
							
							else if(strcmp(tag, "h6") == 0)
							{
								pushItemStack(H6);
								
								if(!inLI)
								{
									if(tmpDst > 0 && tempOut[tmpDst-1] != '\n')
									{						
										tempOut[tmpDst] = '\n';
										tmpDst++;
									}
									
									if(tmpDst > 1 && tempOut[tmpDst-2] != '\n')
									{
										tempOut[tmpDst] = '\n';
										tmpDst++;
									}
								}
								
								pushSizeStack(tmpSize);
								tmpSize = 0;
								
								memset(tmpTag, 0, 1025);
								getElementText(element, "class", tmpTag);				
								pushColorStack(tmpColor);
								tmpColor = getCSSElement(tmpTag, "h6", tmpColor, htmlPage);
								
								updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
							}
							
							else if(strcmp(tag, "small") == 0)
							{
								pushItemStack(SMALL);
								
								pushSizeStack(tmpSize);
								tmpSize = 0;
								
								memset(tmpTag, 0, 1025);
								getElementText(element, "class", tmpTag);				
								pushColorStack(tmpColor);
								tmpColor = getCSSElement(tmpTag, "small", tmpColor, htmlPage);
								
								updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
							}
							
							else if(strcmp(tag, "tt") == 0)
							{
								pushItemStack(TT);
								
								pushSizeStack(tmpSize);
								tmpSize = 6;
								
								memset(tmpTag, 0, 1025);
								getElementText(element, "class", tmpTag);				
								pushColorStack(tmpColor);
								tmpColor = getCSSElement(tmpTag, "tt", tmpColor, htmlPage);
								
								updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
							}
							
							else if(strcmp(tag, "code") == 0)
							{
								pushItemStack(CODE);
								
								inCode = true;
								pushSizeStack(tmpSize);
								tmpSize = 6;
								
								memset(tmpTag, 0, 1025);
								getElementText(element, "class", tmpTag);				
								pushColorStack(tmpColor);
								tmpColor = getCSSElement(tmpTag, "code", tmpColor, htmlPage);
								
								updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
							}
							
							else if(strcmp(tag, "pre") == 0)
							{
								pushItemStack(PRE);
								
								inCode = true;
								pushSizeStack(tmpSize);
								tmpSize = 6;
								
								memset(tmpTag, 0, 1025);
								getElementText(element, "class", tmpTag);				
								pushColorStack(tmpColor);
								tmpColor = getCSSElement(tmpTag, "pre", tmpColor, htmlPage);
								
								updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
							}
							
							else if(strcmp(tag, "samp") == 0)
							{
								pushItemStack(SAMP);
								
								inCode = true;
								pushSizeStack(tmpSize);
								tmpSize = 6;
								
								memset(tmpTag, 0, 1025);
								getElementText(element, "class", tmpTag);				
								pushColorStack(tmpColor);
								tmpColor = getCSSElement(tmpTag, "samp", tmpColor, htmlPage);
								
								updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
							}
							
							else if(strcmp(tag, "kbd") == 0)
							{
								pushItemStack(KBD);
								
								inCode = true;
								pushSizeStack(tmpSize);
								tmpSize = 6;
								
								memset(tmpTag, 0, 1025);
								getElementText(element, "class", tmpTag);				
								pushColorStack(tmpColor);
								tmpColor = getCSSElement(tmpTag, "kbd", tmpColor, htmlPage);
								
								updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
							}
							
							else if(strcmp(tag, "big") == 0)
							{
								pushItemStack(BIG);
								
								pushSizeStack(tmpSize);
								tmpSize = 2;
								
								memset(tmpTag, 0, 1025);
								getElementText(element, "class", tmpTag);				
								pushColorStack(tmpColor);
								tmpColor = getCSSElement(tmpTag, "big", tmpColor, htmlPage);
								
								updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
							}
							
							else if(strcmp(tag, "sub") == 0)
							{
								pushItemStack(SUB);
								
								int oldSize = tmpSize;
								
								pushSizeStack(tmpSize);
								
								if(tmpSize != 6)
									tmpSize = 0;
								
								memset(tmpTag, 0, 1025);
								getElementText(element, "class", tmpTag);				
								pushColorStack(tmpColor);
								tmpColor = getCSSElement(tmpTag, "sub", tmpColor, htmlPage);
								
								updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
								
								switch(oldSize)
								{
									case 0:
									case 6:
										tempOut[tmpDst] = POS_DOWN;
										tmpDst++;
										tempOut[tmpDst] = POS_DOWN;
										tmpDst++;
										tempOut[tmpDst] = POS_DOWN;
										tmpDst++;
										tempOut[tmpDst] = POS_DOWN;
										tmpDst++;
										break;
									case 1:
									case 4:
										tempOut[tmpDst] = POS_DOWN;
										tmpDst++;
										tempOut[tmpDst] = POS_DOWN;
										tmpDst++;
										tempOut[tmpDst] = POS_DOWN;
										tmpDst++;
										tempOut[tmpDst] = POS_DOWN;
										tmpDst++;
										tempOut[tmpDst] = POS_DOWN;
										tmpDst++;
										tempOut[tmpDst] = POS_DOWN;
										tmpDst++;
										tempOut[tmpDst] = POS_DOWN;
										tmpDst++;
										break;
									case 2:
									case 5:
										tempOut[tmpDst] = POS_DOWN;
										tmpDst++;
										tempOut[tmpDst] = POS_DOWN;
										tmpDst++;
										tempOut[tmpDst] = POS_DOWN;
										tmpDst++;
										tempOut[tmpDst] = POS_DOWN;
										tmpDst++;
										tempOut[tmpDst] = POS_DOWN;
										tmpDst++;
										tempOut[tmpDst] = POS_DOWN;
										tmpDst++;
										tempOut[tmpDst] = POS_DOWN;
										tmpDst++;
										tempOut[tmpDst] = POS_DOWN;
										tmpDst++;
										tempOut[tmpDst] = POS_DOWN;
										tmpDst++;
										tempOut[tmpDst] = POS_DOWN;
										tmpDst++;
										break;
									case 3:
										tempOut[tmpDst] = POS_DOWN;
										tmpDst++;
										tempOut[tmpDst] = POS_DOWN;
										tmpDst++;
										tempOut[tmpDst] = POS_DOWN;
										tmpDst++;
										tempOut[tmpDst] = POS_DOWN;
										tmpDst++;
										tempOut[tmpDst] = POS_DOWN;
										tmpDst++;
										tempOut[tmpDst] = POS_DOWN;
										tmpDst++;
										tempOut[tmpDst] = POS_DOWN;
										tmpDst++;
										tempOut[tmpDst] = POS_DOWN;
										tmpDst++;
										tempOut[tmpDst] = POS_DOWN;
										tmpDst++;
										tempOut[tmpDst] = POS_DOWN;
										tmpDst++;
										tempOut[tmpDst] = POS_DOWN;
										tmpDst++;
										tempOut[tmpDst] = POS_DOWN;
										tmpDst++;
										tempOut[tmpDst] = POS_DOWN;
										tmpDst++;
										break;
								}
							}
							
							else if(strcmp(tag, "sup") == 0)
							{
								pushItemStack(SUPER);
								
								int oldSize = tmpSize;
								
								pushSizeStack(tmpSize);
								
								if(tmpSize != 6)
									tmpSize = 0;
								
								memset(tmpTag, 0, 1025);
								getElementText(element, "class", tmpTag);				
								pushColorStack(tmpColor);
								tmpColor = getCSSElement(tmpTag, "sup", tmpColor, htmlPage);
								
								updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
								
								tempOut[tmpDst] = POS_UP;
								tmpDst++;
								tempOut[tmpDst] = POS_UP;
								tmpDst++;
								
								if(oldSize == 0 || oldSize == 6)
								{
									tempOut[tmpDst] = POS_UP;
									tmpDst++;
									tempOut[tmpDst] = POS_UP;
									tmpDst++;
									tempOut[tmpDst] = POS_UP;
									tmpDst++;
								}
							}
							
							else if(!(inBody || isBody()) && strcmp(tag, "link") == 0)
							{
								memset(tmpTag, 0, 1025);
								getElementText(element, "rel", tmpTag);	
								strlwr(tmpTag);
								
								if(strcmp(tmpTag, "stylesheet") == 0)
								{
									// it's a style link, lets pay attention
									// but first, check to make sure the type is css
									
									memset(tmpTag, 0, 1025);
									getElementText(element, "type", tmpTag);	
									strlwr(tmpTag);
									
									if(strcmp(tmpTag, "text/css") == 0)
									{
										// it's a css file, hoorj!
										
										memset(tmpTag, 0, 1025);
										getElementText(element, "href", tmpTag);
										
										bool hasLoaded = false;
										
										if(htmlPage->cssLocation != 0)
										{
											// we already chose a stylesheet for this page
											// check to see if it's handheld, if so go with this one instead
											// if not, go with the original
											
											char tmpMedia[128];
											memset(tmpMedia, 0, 128);
											getElementText(element, "media", tmpMedia);
											strlwr(tmpMedia);
											
											if(strstr(tmpMedia, "handheld") != NULL)
											{
												hasLoaded = false;
												
												if(htmlPage->cssLocation)
												{
													free(htmlPage->cssLocation); // kill the old memory
													htmlPage->cssLocation = NULL;
												}
											}
											else
											{
												hasLoaded = true;
											}
										}
										
										if(!hasLoaded)
										{
											// we got the URL, lets allocate some dynamic memory and store it for
											// a proper browser to fetch and pass back the css data
											
											htmlPage->cssLocation = (char *)safeMalloc(strlen(tmpTag)+1);
											strcpy(htmlPage->cssLocation, tmpTag);
										}
									}
								}
								
							}
							
							else
							{
								used = false;
							}
							
							if(used)
							{
								goto endOfLoop; // nasty, but skips doing all the if's later
							}
						}
						
						if(strcmp(tag, "p") == 0)
						{
							if(htmlStyle > 0)
							{
								if(isPElement)
								{
									// if they used style but didn't close the <p> tag, we should pop it here
									popItemStackByType(P);
									// pop this too so we don't overflow it
									popColorStack();
								}
								
								pushItemStack(P);
								
								memset(tmpTag, 0, 1025);
								getElementText(element, "class", tmpTag);				
								pushColorStack(tmpColor);
								tmpColor = getCSSElement(tmpTag, "p", tmpColor, htmlPage);
								
								updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
							}
							
							if(tmpDst > 0 && tempOut[tmpDst-1] != '\n')
							{						
								tempOut[tmpDst] = '\n';
								tmpDst++;						
							}
							
							if(!isPElement)
							{
								if(tmpDst > 1 && tempOut[tmpDst-2] != '\n')
								{
									tempOut[tmpDst] = '\n';
									tmpDst++;
								}
							}
							
							isPElement = true;
						}
						
						else if(strcmp(tag, "br") == 0)
						{
							tempOut[tmpDst] = '\n';
							tmpDst++;
						}
						
						else if(strcmp(tag, "img") == 0)
						{	
							memset(tmpTag, 0, 1024);							
							getElementText(element, "width", tmpTag);
							int tWidth = getNumber(tmpTag);
							
							memset(tmpTag, 0, 1024);
							getElementText(element, "height", tmpTag);
							int tHeight = getNumber(tmpTag);
							
							bool rendered = false;
							
							// add check here for images on and off
							if(htmlStyle == 2 && allowURLS && tWidth > 0 && tHeight > 0 && tWidth <= (htmlPage->destWidth - IMAGE_PADDING) && tHeight <= 192) // for where it starts wrapping
							{
								IMAGE_TYPE tmpImage;
								
								tmpImage.graphicsData = NULL;
								
								tmpImage.url.startPos = 0;
								tmpImage.url.endPos = 0;
								
								// url hack to ensure any length url without taking up memory
								char *tmpLoc = cistrstr(element, "src");
								
								if(tmpLoc)
								{
									int placeIn = tmpLoc - element + strlen(tag) + 4; // place into the tag relative to the start of the tag;
									
									placeIn += lastTagStart; // now it points absolutely to the string start where it can be found
									
									getTagLocations(cistrstr(element, "src") + 3, tmpImage.url.startPos, tmpImage.url.endPos);
									
									tmpImage.url.startPos += placeIn;
									tmpImage.url.endPos += placeIn;
									
									for(int tF=0;tF<NUM_CRLFS;tF++)
									{
										if(crlfLoc[tF] != 0 && crlfLoc[tF] <= tmpImage.url.startPos)
										{
											tmpImage.url.startPos++;
											tmpImage.url.endPos++;
										}
									}
								}
								
								if(tmpImage.url.endPos > tmpImage.url.startPos)
								{
									if(!duplicateImages)
									{
										duplicateImageCount = 1;
										duplicateImages = (DUPLICATE_IMAGE_TYPE *)trackMalloc(sizeof(DUPLICATE_IMAGE_TYPE), "duplicate images");
										memset(duplicateImages, 0, sizeof(DUPLICATE_IMAGE_TYPE));
										
										duplicateImages[0].crc = getCRCFromOffset(tempStr, tmpImage.url);
									}
									else
									{
										u32 tmpCRC = getCRCFromOffset(tempStr, tmpImage.url);
										
										for(u32 crcFind=0;crcFind < duplicateImageCount;crcFind++)
										{
											if(tmpCRC == duplicateImages[crcFind].crc)
											{
												// render other image
												
												// just in case there's a url or button tag
												tempOut[tmpDst] = POS_LEFT;
												tmpDst++;												
												tempOut[tmpDst] = POS_RIGHT;
												tmpDst++;
												
												updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, duplicateImages[crcFind].whichFormElement);
												
												tempOut[tmpDst] = FORM_IDENTIFIER;
												tmpDst++;
												
												updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
												
												tempOut[tmpDst] = POS_LEFT;
												tmpDst++;												
												tempOut[tmpDst] = POS_RIGHT;
												tmpDst++;
												
												rendered = true;
												
												break;
											}
										}
										
										if(!rendered) // didn't match
										{
											duplicateImageCount++;
											duplicateImages = (DUPLICATE_IMAGE_TYPE *)trackRealloc(duplicateImages, sizeof(DUPLICATE_IMAGE_TYPE) * duplicateImageCount);
											memset(&duplicateImages[duplicateImageCount - 1], 0, sizeof(DUPLICATE_IMAGE_TYPE));
											
											duplicateImages[duplicateImageCount - 1].crc = getCRCFromOffset(tempStr, tmpImage.url);
										}
									}
									
									if(!rendered)
									{
										// valid image
										rendered = true;
										
										// we will treat images as special types of forms for easier rendering
										htmlPage->maxFormControls++;
										
										if(htmlPage->maxFormControls == 1)
											htmlPage->formCodes = (FORM_ITEM *)trackMalloc(sizeof(FORM_ITEM), "forms data");
										else
											htmlPage->formCodes = (FORM_ITEM *)trackRealloc(htmlPage->formCodes, sizeof(FORM_ITEM) * htmlPage->maxFormControls);
										
										memset(&htmlPage->formCodes[htmlPage->maxFormControls-1], 0, sizeof(FORM_ITEM));
										
										htmlPage->formCodes[htmlPage->maxFormControls-1].formID = DUMMYFORM_IMAGES;
										
										// set up temp image properly
										tmpImage.graphicsData = NULL;
										tmpImage.state = IMAGE_NEEDSDOWNLOAD;
										
										// copy over temporary image
										htmlPage->formCodes[htmlPage->maxFormControls-1].state = trackMalloc(sizeof(IMAGE_TYPE), "image");
										memcpy(htmlPage->formCodes[htmlPage->maxFormControls-1].state, &tmpImage, sizeof(IMAGE_TYPE));
										
										// move in alt text
										getElementText(element, "alt", tmpTag);
										strncpy(htmlPage->formCodes[htmlPage->maxFormControls-1].name, tmpTag, 63);
										
										// set up image attributes
										htmlPage->formCodes[htmlPage->maxFormControls-1].whatType = FORM_IMAGE;
										htmlPage->formCodes[htmlPage->maxFormControls-1].width = tWidth;
										htmlPage->formCodes[htmlPage->maxFormControls-1].height = tHeight;
										
										duplicateImages[duplicateImageCount - 1].whichFormElement = htmlPage->maxFormControls;
										
										// just in case there's a url or button tag
										tempOut[tmpDst] = POS_LEFT;
										tmpDst++;												
										tempOut[tmpDst] = POS_RIGHT;
										tmpDst++;
										
										updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, htmlPage->maxFormControls);
										
										tempOut[tmpDst] = FORM_IDENTIFIER;
										tmpDst++;
										
										updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
										
										tempOut[tmpDst] = POS_LEFT;
										tmpDst++;												
										tempOut[tmpDst] = POS_RIGHT;
										tmpDst++;
									}
								}
							}
							
							if(!rendered)							
							{
								getElementText(element, "alt", tmpTag);
								
								if(strlen(tmpTag) > 0)
								{
									tempOut[tmpDst] = '[';
									tmpDst++;
									
									for(int ti=0;ti<(int)strlen(tmpTag);++ti)
									{
										tempOut[tmpDst] = tmpTag[ti];
										tmpDst++;
									}
									
									tempOut[tmpDst] = ']';
									tmpDst++;
								}
								else
								{
									tempOut[tmpDst] = '[';
									tmpDst++;
									tempOut[tmpDst] = 'I';
									tmpDst++;
									tempOut[tmpDst] = 'M';
									tmpDst++;
									tempOut[tmpDst] = 'G';
									tmpDst++;
									tempOut[tmpDst] = ']';
									tmpDst++;
								}
							}
						}
						
						else if(strcmp(tag, "b") == 0)
						{
							pushItemStack(B);
							
							tmpBold = true;
							
							if(htmlStyle > 0)
							{
								memset(tmpTag, 0, 1025);
								getElementText(element, "class", tmpTag);				
								pushColorStack(tmpColor);
								tmpColor = getCSSElement(tmpTag, "b", tmpColor, htmlPage);
							}
							
							updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
						}
						
						else if(strcmp(tag, "i") == 0)
						{
							pushItemStack(I);
							
							tmpItalics = true;
							
							if(htmlStyle > 0)
							{
								memset(tmpTag, 0, 1025);
								getElementText(element, "class", tmpTag);				
								pushColorStack(tmpColor);
								tmpColor = getCSSElement(tmpTag, "i", tmpColor, htmlPage);
							}
							
							updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
						}
						
						else if(strcmp(tag, "em") == 0)
						{
							pushItemStack(EM);
							
							tmpItalics = true;
							
							if(htmlStyle > 0)
							{
								memset(tmpTag, 0, 1025);
								getElementText(element, "class", tmpTag);				
								pushColorStack(tmpColor);
								tmpColor = getCSSElement(tmpTag, "em", tmpColor, htmlPage);
							}
							
							updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
						}
						
						else if(strcmp(tag, "strong") == 0)
						{
							pushItemStack(STRONG);
							
							tmpBold = true;
							
							if(htmlStyle > 0)
							{
								memset(tmpTag, 0, 1025);
								getElementText(element, "class", tmpTag);				
								pushColorStack(tmpColor);
								tmpColor = getCSSElement(tmpTag, "strong", tmpColor, htmlPage);
							}
							
							updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
						}
						
						else if(strcmp(tag, "cite") == 0)
						{
							pushItemStack(CITE);
							
							tmpItalics = true;
							
							if(htmlStyle > 0)
							{
								memset(tmpTag, 0, 1025);
								getElementText(element, "class", tmpTag);				
								pushColorStack(tmpColor);
								tmpColor = getCSSElement(tmpTag, "cite", tmpColor, htmlPage);
							}
							
							updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
						}
						
						else if(strcmp(tag, "dfn") == 0)
						{
							pushItemStack(DFN);
							
							tmpItalics = true;
							
							if(htmlStyle > 0)
							{
								memset(tmpTag, 0, 1025);
								getElementText(element, "class", tmpTag);				
								pushColorStack(tmpColor);
								tmpColor = getCSSElement(tmpTag, "dfn", tmpColor, htmlPage);
							}
							
							updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
						}
						
						else if(strcmp(tag, "var") == 0)
						{
							pushItemStack(VAR);
							
							tmpItalics = true;
							
							if(htmlStyle > 0)
							{
								memset(tmpTag, 0, 1025);
								getElementText(element, "class", tmpTag);				
								pushColorStack(tmpColor);
								tmpColor = getCSSElement(tmpTag, "var", tmpColor, htmlPage);
							}
							
							updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
						}
						
						else if(strcmp(tag, "u") == 0)
						{
							pushItemStack(U);
							
							tmpUnderline = true;
							
							if(htmlStyle > 0)
							{
								memset(tmpTag, 0, 1025);
								getElementText(element, "class", tmpTag);				
								pushColorStack(tmpColor);
								tmpColor = getCSSElement(tmpTag, "u", tmpColor, htmlPage);
							}
							
							updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
						}
						
						else if(strcmp(tag, "s") == 0 || strcmp(tag, "strike") == 0)
						{
							pushItemStack(STRIKE);
							
							tmpStrikeThrough = true;
							
							if(htmlStyle > 0)
							{
								memset(tmpTag, 0, 1025);
								getElementText(element, "class", tmpTag);				
								pushColorStack(tmpColor);
								tmpColor = getCSSElement(tmpTag, "s", tmpColor, htmlPage);
							}
							
							updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
						}
						
						else if(strcmp(tag, "font") == 0)
						{
							pushItemStack(FONT);
							
							pushColorStack(tmpColor);
							tmpColor = getColorIndex(getFontColor(element, "color", 0x0), htmlPage);
							
							if(htmlStyle > 0)
							{
								memset(tmpTag, 0, 1025);
								getElementText(element, "class", tmpTag);
								tmpColor = getCSSElement(tmpTag, "font", tmpColor, htmlPage);
							}
							
							updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
						}
						
						else if(strcmp(tag, "a") == 0)
						{	
							bool toParse = true;
							
							if(!cistrstr(element, "name"))
							{
								if(!cistrstr(element, "href"))
									toParse = false;
							}
							
							if(toParse)
							{
								if(allowURLS)
								{
									if(!htmlPage->maxURLs)
									{
										htmlPage->urlCodes = (URL_ITEM *)trackMalloc(sizeof(URL_ITEM), "tmp url stack");
										htmlPage->maxURLs = 1;
									}
									else
									{
										++htmlPage->maxURLs;
										htmlPage->urlCodes = (URL_ITEM *)trackRealloc(htmlPage->urlCodes, sizeof(URL_ITEM) * htmlPage->maxURLs);
									}
									
									memset(&htmlPage->urlCodes[htmlPage->maxURLs-1], 0, sizeof(URL_ITEM));
									
									memset(tmpTag, 0, 1025);
									
									getElementText(element, "name", tmpTag);
									strlwr(tmpTag);
									strncpy(htmlPage->urlCodes[htmlPage->maxURLs-1].name, tmpTag, 63);
									
									// url hack to ensure any length url without taking up memory
									char *tmpLoc = cistrstr(element, "href");
									
									if(tmpLoc)
									{
										int placeIn = tmpLoc - element + strlen(tag) + 5; // place into the tag relative to the start of the tag;
										
										placeIn += lastTagStart; // now it points absolutely to the string start where it can be found
										
										getTagLocations(cistrstr(element, "href") + 4, htmlPage->urlCodes[htmlPage->maxURLs-1].url.startPos, htmlPage->urlCodes[htmlPage->maxURLs-1].url.endPos);
										
										htmlPage->urlCodes[htmlPage->maxURLs-1].url.startPos += placeIn;
										htmlPage->urlCodes[htmlPage->maxURLs-1].url.endPos += placeIn;
										
										for(int tF=0;tF<NUM_CRLFS;tF++)
										{
											if(crlfLoc[tF] != 0 && crlfLoc[tF] <= htmlPage->urlCodes[htmlPage->maxURLs-1].url.startPos)
											{
												htmlPage->urlCodes[htmlPage->maxURLs-1].url.startPos++;
												htmlPage->urlCodes[htmlPage->maxURLs-1].url.endPos++;
											}
										}
									}
									
									if(htmlPage->urlCodes[htmlPage->maxURLs-1].url.startPos == htmlPage->urlCodes[htmlPage->maxURLs-1].url.endPos && strlen(htmlPage->urlCodes[htmlPage->maxURLs-1].name) > 0)
									{
										// this is just a placeholder <a> tag in order to have an anchor.
										tmpA = htmlPage->maxURLs;
										wasNameTag = true;
										
										updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
									}
									else
									{
										// normal <a> tag with link, etc
										
										// first, make sure they weren't continuing an old <a> tag
										if(popItemStackByType(A))
										{	
											tmpColor = popColorStack();
											tmpUnderline = oldUnderline;
											tmpA = 0;
											
											if(!wasNameTag)
											{
												updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
												
												if(tmpDst > 4)
												{
													if(tempOut[tmpDst - 1] == 0x20 && tempOut[tmpDst - 4] == FORM_IDENTIFIER)
													{
														// some sites leave a trailing space which looks terrible on DSO
														// lets remove it for aesthetics
														
														moveLastItemBack();
													}
												}
												
												// to ensure that the formatting 'sticks'
												tempOut[tmpDst] = ' ';
												tmpDst++;
											}
										}
										
										pushItemStack(A);
										
										pushColorStack(tmpColor);
										oldUnderline = tmpUnderline;
										tmpUnderline = true;
										wasNameTag = false;
										
										tmpA = htmlPage->maxURLs;
										
										if(htmlStyle > 0)
										{
											memset(tmpTag, 0, 1025);
											getElementText(element, "class", tmpTag);
											
											if(strlen(tmpTag) > 0)
												tmpColor = getCSSElement(tmpTag, "a", C_LINK, htmlPage);
											else
												tmpColor = C_LINK;
										}
										else
										{
											tmpColor = C_LINK;
										}
										
										updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
									}
								}
								else
								{
									pushItemStack(A);
									
									pushColorStack(tmpColor);
									oldUnderline = tmpUnderline;
									tmpUnderline = true;
									tmpA = 0;
									
									if(htmlStyle > 0)
									{
										memset(tmpTag, 0, 1025);
										getElementText(element, "class", tmpTag);
										
										if(strlen(tmpTag) > 0)
											tmpColor = getCSSElement(tmpTag, "a", C_LINK, htmlPage);
										else
											tmpColor = C_LINK;
									}
									else
									{
										tmpColor = C_LINK;
									}
									
									updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
								}
							}
						}
						
						else if(strcmp(tag, "frameset") == 0)
						{
							pushItemStack(FRAMESET);						
							framesCount++;
							inNoFrames = false;
						}
						
						else if(strcmp(tag, "noframes") == 0)
						{
							if(framesCount > 0)
							{
								pushItemStack(NOFRAMES);						
								inNoFrames = true;
							}
						}
						
						else if(strcmp(tag, "script") == 0)
						{
							if(!isSelfTerminating(element))
							{
								pushItemStack(SCRIPT);
								inScript = true;
							}
						}
						
						else if(strcmp(tag, "style") == 0)
						{
							pushItemStack(STYLE);
							inStyle = true;
							
							memset(tmpTag, 0, 1025);
							getElementText(element, "type", tmpTag);
							strlwr(tmpTag);
							
							if(strlen(tmpTag) == 0 || strcmp(tmpTag, "text/css") == 0)
							{
								cssTag = true;
								cssBuf = (char *)trackMalloc(CSS_SIZE, "css temp");
								
								memset(cssBuf, 0, CSS_SIZE);
							}
						}
						
						else if(strcmp(tag, "q") == 0)
						{
							pushItemStack(Q);
							tempOut[tmpDst] = 34;
							tmpDst++;
						}
						
						else if(strcmp(tag, "dt") == 0)
						{
							pushItemStack(DT);
							
							tmpBold = true;
							
							updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
						}
						
						else if(strcmp(tag, "dd") == 0)
						{
							pushItemStack(DD);
							
							tempOut[tmpDst] = '\t';
							tmpDst++;
						}
						
						else if(strcmp(tag, "ul") == 0)
						{
							pushOLStack(olCount);
							pushItemStack(UL);						
							ulDepth++;
							olCount = 0;
							firstTag = true;
							inLI = true;
						}
						
						else if(strcmp(tag, "ol") == 0)
						{
							firstTag = true;
							inLI = true;
							pushOLStack(olCount);
							pushItemStack(OL);						
							ulDepth++;
							olCount = 1;
							
							char tStart[50];
							memset(tStart, 0, 50);
							getElementText(element, "start", tStart);
							
							if(strlen(tStart) > 0)
							{
								int olNum;
								sscanf(tStart, "%d", &olNum);
								
								if(olNum > 0)
									olCount = olNum;
							}
						}
						
						else if(strcmp(tag, "li") == 0)
						{
							if(ulDepth > 0)
							{
								if(!firstTag)
								{
									tempOut[tmpDst] = '\n';
									tmpDst++;
								}
								else if(firstTag && tempOut[tmpDst - 1] != '\n')
								{
									tempOut[tmpDst] = '\n';
									tmpDst++;
								}							
								
								firstTag = false;
								
								for(int t=0;t<ulDepth;t++)
								{							
									tempOut[tmpDst] = '	';
									tmpDst++;
								}
								
								if(olCount == 0)
								{
									tempOut[tmpDst] = 0x95;
									tmpDst++;
								}
								else
								{								
									char tNum[50];
									memset(tNum, 0, 50);
									getElementText(element, "value", tNum);
									
									if(strlen(tNum) > 0)
									{
										int olNum;
										sscanf(tNum, "%d", &olNum);
										
										if(olNum > 0)
											olCount = olNum;
									}
									
									int zz = 0;
									sprintf(tNum, "%d.", olCount++);
									
									while(tNum[zz] != 0)
									{
										tempOut[tmpDst] = tNum[zz];
										tmpDst++;
										zz++;
									}
								}
								
								tempOut[tmpDst] = ' ';
								tmpDst++;
							}							
						}
						
						else if(strcmp(tag, "hr") == 0)
						{
							if(tmpDst > 0 && tempOut[tmpDst-1] != '\n')
							{
								tempOut[tmpDst] = '\n';
								tmpDst++;
							}
							
							updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, true, tmpStrikeThrough, tmpA, 0);
							tempOut[tmpDst] = ' ';
							tmpDst++;
							tempOut[tmpDst] = '\n';
							tmpDst++;						
						}
						
						else if(strcmp(tag, "div") == 0)
						{
							if(tmpDst > 0 && tempOut[tmpDst-1] != '\n')
							{
								tempOut[tmpDst] = '\n';
								tmpDst++;
							}
							
							if(htmlStyle > 0)
							{
								pushItemStack(DIV);
								
								memset(tmpTag, 0, 1025);
								getElementText(element, "class", tmpTag);				
								pushColorStack(tmpColor);
								tmpColor = getCSSElement(tmpTag, "div", tmpColor, htmlPage);
								
								updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
							}
						}
						
						else if(strcmp(tag, "span") == 0)
						{
							if(htmlStyle > 0)
							{
								pushItemStack(SPAN);
								
								memset(tmpTag, 0, 1025);
								getElementText(element, "class", tmpTag);				
								pushColorStack(tmpColor);
								tmpColor = getCSSElement(tmpTag, "span", tmpColor, htmlPage);
								
								updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
							}
						}
						
						if(strcmp(tag, "td") == 0)
						{
							pushItemStack(TD);
							
							int tA = 0;
							
							if(needsSpacing)
								tA = cellSpacing;
							
							if(cellPadding > 0)
							{
								tA += cellPadding;
								
								for(int cp=0;cp<tA;++cp)
								{
									tempOut[tmpDst] = '\a';
									tmpDst++;
								}
							}
							
							needsSpacing = true;
						}
						
						else if(strcmp(tag, "tr") == 0)
						{
							if(checkForBadTR())
							{
								if(htmlStyle == 0)
									popItemStackUntilType(TR, TABLE);
								else
									takeCareOfItem(TR, TABLE, tempOut, tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpStrikeThrough, tmpSize, tmpColor, tmpA, inCode, oldUnderline, framesCount, inNoFrames, inScript, inStyle, htmlPage);
								
								if(tmpDst > 0 && tempOut[tmpDst-1] != '\n')
								{						
									tempOut[tmpDst] = '\n';
									tmpDst++;						
								}
								
								// handle padding and spacing
								int tA = (cellSpacing + cellPadding) - 2;
								
								// special case... libfb automatically adds two, so we need to compensate for that
								if(tA < 0)
								{
									while(tA != 0)
									{		
										tempOut[tmpDst] = POS_UP;
										tmpDst++;	
										
										tA++;
									}
								}
								
								if(tA > 0)
								{
									while(tA != 0)
									{		
										tempOut[tmpDst] = POS_DOWN;
										tmpDst++;	
										
										tA--;
									}
								}
							}
							
							pushItemStack(TR);
							needsSpacing = false;
						}
						
						else if(strcmp(tag, "table") == 0)
						{
							pushItemStack(TABLE);
							
							if(tmpDst > 0 && tempOut[tmpDst-1] != '\n')
							{						
								tempOut[tmpDst] = '\n';
								tmpDst++;						
							}
							
							pushTableStack(cellSpacing, cellPadding);
							
							memset(tmpTag, 0, 1025);							
							getElementText(element, "cellspacing", tmpTag);
							
							if(strlen(tmpTag) > 0)
								cellSpacing = getNumber(tmpTag);
							else
								cellSpacing = DEFAULT_TABLE_SPACING;
							
							memset(tmpTag, 0, 1025);
							getElementText(element, "cellpadding", tmpTag);
							
							if(strlen(tmpTag) > 0)
								cellPadding = getNumber(tmpTag);
							else
								cellPadding = DEFAULT_TABLE_PADDING;
							
							needsSpacing = false;
						}
						
						else if(strcmp(tag, "html") == 0)
							pushItemStack(HTML);
						
						else if(strcmp(tag, "head") == 0)
							pushItemStack(HEAD);
						
						else if(strcmp(tag, "body") == 0)
						{
							pushItemStack(BODY);
							
							inBody = true;
							wasHead = false;
							
							htmlPage->colors[C_BG] = getFontColor(element, "bgcolor", htmlPage->colors[C_BG]);
							htmlPage->colors[C_LINK] = getFontColor(element, "link", htmlPage->colors[C_LINK]);
							htmlPage->colors[C_ALINK] = getFontColor(element, "alink", htmlPage->colors[C_ALINK]);
							htmlPage->colors[C_VLINK] = getFontColor(element, "vlink", htmlPage->colors[C_VLINK]);
							htmlPage->colors[C_DEFAULT] = getFontColor(element, "text", htmlPage->colors[C_DEFAULT]);
						}
						
						else if(strcmp(tag, "title") == 0)
							pushItemStack(TITLE);
					}
					else
					{
						if(inSelect)
						{
							if(strcmp(tag, "/select") == 0)
							{
								inSelect = false;
							}
							
							if(strcmp(tag, "/optgroup") == 0)
							{
								curLevel--;
								
								if(curLevel < 0)
									curLevel = 0;
							}
							
							goto endOfLoop;
						}
						
						if(htmlStyle == 0)
						{
							bool used = true;
							
							if(strcmp(tag, "/h1") == 0)				
							{
								if(popItemStackByType(H1) && !inLI)
								{			
									tempOut[tmpDst] = '\n';
									tmpDst++;
									tempOut[tmpDst] = '\n';
									tmpDst++;
								}
							}
							
							else if(strcmp(tag, "/h2") == 0)				
							{
								if(popItemStackByType(H2) && !inLI)
								{			
									tempOut[tmpDst] = '\n';
									tmpDst++;
									tempOut[tmpDst] = '\n';
									tmpDst++;
								}
							}
							
							else if(strcmp(tag, "/h3") == 0)				
							{
								if(popItemStackByType(H3) && !inLI)
								{
									tempOut[tmpDst] = '\n';
									tmpDst++;
									tempOut[tmpDst] = '\n';
									tmpDst++;
								}
							}
							
							else if(strcmp(tag, "/h4") == 0)				
							{
								if(popItemStackByType(H4) && !inLI)
								{
									tempOut[tmpDst] = '\n';
									tmpDst++;
									tempOut[tmpDst] = '\n';
									tmpDst++;
								}
							}
							
							else if(strcmp(tag, "/h5") == 0)				
							{
								if(popItemStackByType(H5) && !inLI)
								{
									tempOut[tmpDst] = '\n';
									tmpDst++;
									tempOut[tmpDst] = '\n';
									tmpDst++;
								}
							}
							
							else if(strcmp(tag, "/h6") == 0)				
							{
								if(popItemStackByType(H6) && !inLI)
								{
									tempOut[tmpDst] = '\n';
									tmpDst++;
									tempOut[tmpDst] = '\n';
									tmpDst++;
								}
							}
							
							else if(strcmp(tag, "/small") == 0)				
							{
								if(popItemStackByType(SMALL))
								{
								/*	tempOut[tmpDst] = '\n';
									tmpDst++;
									tempOut[tmpDst] = '\n';
									tmpDst++;*/
								}
							}
							
							else if(strcmp(tag, "/big") == 0)				
							{
								if(popItemStackByType(BIG))
								{
									/*tempOut[tmpDst] = '\n';
									tmpDst++;
									tempOut[tmpDst] = '\n';
									tmpDst++;*/
								}
							}
							
							else if(strcmp(tag, "/tt") == 0)				
							{
								if(popItemStackByType(TT))
								{
									/*tempOut[tmpDst] = '\n';
									tmpDst++;
									tempOut[tmpDst] = '\n';
									tmpDst++;*/
								}
							}
							
							else if(strcmp(tag, "/code") == 0)				
							{
								if(popItemStackByType(CODE))
								{
									inCode = false;
									/*tempOut[tmpDst] = '\n';
									tmpDst++;
									tempOut[tmpDst] = '\n';
									tmpDst++;*/
								}
							}
							
							else if(strcmp(tag, "/pre") == 0)				
							{
								if(popItemStackByType(PRE))
									inCode = false;
							}
							
							else
								used = false;
							
							if(used)
								goto endOfLoop; // nasty, but skips doing all the if's later
						}
						else
						{	
							bool used = true;
							
							if(strcmp(tag, "/h1") == 0)				
							{
								if(popItemStackByType(H1))
								{								
									tmpSize = popSizeStack();
									tmpColor = popColorStack();
									
									updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
									
									if(!inLI)
									{
										tempOut[tmpDst] = '\n';
										tmpDst++;
										tempOut[tmpDst] = '\n';
										tmpDst++;
									}
								}
							}
							
							else if(strcmp(tag, "/h2") == 0)				
							{
								if(popItemStackByType(H2))
								{								
									tmpSize = popSizeStack();
									tmpColor = popColorStack();
									
									updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
									
									if(!inLI)
									{
										tempOut[tmpDst] = '\n';
										tmpDst++;
										tempOut[tmpDst] = '\n';
										tmpDst++;
									}
								}
							}
							
							else if(strcmp(tag, "/h3") == 0)				
							{
								if(popItemStackByType(H3))
								{								
									tmpSize = popSizeStack();
									tmpColor = popColorStack();
									
									updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
									
									if(!inLI)
									{
										tempOut[tmpDst] = '\n';
										tmpDst++;
										tempOut[tmpDst] = '\n';
										tmpDst++;
									}
								}
							}
							
							else if(strcmp(tag, "/h4") == 0)				
							{
								if(popItemStackByType(H4))
								{								
									tmpSize = popSizeStack();
									tmpColor = popColorStack();
									
									updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
									
									if(!inLI)
									{
										tempOut[tmpDst] = '\n';
										tmpDst++;
										tempOut[tmpDst] = '\n';
										tmpDst++;
									}
								}
							}
							
							else if(strcmp(tag, "/h5") == 0)				
							{
								if(popItemStackByType(H5))
								{								
									tmpSize = popSizeStack();
									tmpColor = popColorStack();
									
									updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
									
									if(!inLI)
									{
										tempOut[tmpDst] = '\n';
										tmpDst++;
										tempOut[tmpDst] = '\n';
										tmpDst++;
									}
								}
							}
							
							else if(strcmp(tag, "/h6") == 0)				
							{
								if(popItemStackByType(H6))
								{								
									tmpSize = popSizeStack();
									tmpColor = popColorStack();
									
									updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
									
									if(!inLI)
									{
										tempOut[tmpDst] = '\n';
										tmpDst++;
										tempOut[tmpDst] = '\n';
										tmpDst++;
									}
								}
							}
							
							else if(strcmp(tag, "/small") == 0)				
							{
								if(popItemStackByType(SMALL))
								{								
									tmpSize = popSizeStack();
									tmpColor = popColorStack();
									
									updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
									
									/*tempOut[tmpDst] = '\n';
									tmpDst++;
									tempOut[tmpDst] = '\n';
									tmpDst++;*/
								}
							}	
							
							else if(strcmp(tag, "/tt") == 0)				
							{
								if(popItemStackByType(TT))
								{								
									tmpSize = popSizeStack();
									tmpColor = popColorStack();
									
									updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
									
									/*tempOut[tmpDst] = '\n';
									tmpDst++;
									tempOut[tmpDst] = '\n';
									tmpDst++;*/
								}
							}	
							
							else if(strcmp(tag, "/code") == 0)				
							{
								if(popItemStackByType(CODE))
								{								
									tmpSize = popSizeStack();
									tmpColor = popColorStack();
									
									updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
									inCode = false;
									
									/*tempOut[tmpDst] = '\n';
									tmpDst++;
									tempOut[tmpDst] = '\n';
									tmpDst++;*/
								}
							}	
							
							else if(strcmp(tag, "/pre") == 0)				
							{
								if(popItemStackByType(PRE))
								{								
									tmpSize = popSizeStack();
									tmpColor = popColorStack();
									
									updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
									inCode = false;
									
									/*tempOut[tmpDst] = '\n';
									tmpDst++;
									tempOut[tmpDst] = '\n';
									tmpDst++;*/
								}
							}	
							
							else if(strcmp(tag, "/samp") == 0)				
							{
								if(popItemStackByType(SAMP))
								{								
									tmpSize = popSizeStack();
									tmpColor = popColorStack();
									
									updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
									inCode = false;
									
									/*tempOut[tmpDst] = '\n';
									tmpDst++;
									tempOut[tmpDst] = '\n';
									tmpDst++;*/
								}
							}	
							
							else if(strcmp(tag, "/kbd") == 0)				
							{
								if(popItemStackByType(KBD))
								{								
									tmpSize = popSizeStack();
									tmpColor = popColorStack();
									
									updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
									inCode = false;
									
									/*tempOut[tmpDst] = '\n';
									tmpDst++;
									tempOut[tmpDst] = '\n';
									tmpDst++;*/
								}
							}	
							
							else if(strcmp(tag, "/big") == 0)				
							{
								if(popItemStackByType(BIG))
								{								
									tmpSize = popSizeStack();
									tmpColor = popColorStack();
									
									updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
									
									/*tempOut[tmpDst] = '\n';
									tmpDst++;
									tempOut[tmpDst] = '\n';
									tmpDst++;*/
								}
							}
							
							else if(strcmp(tag, "/sub") == 0)				
							{
								if(popItemStackByType(SUB))
								{								
									tmpSize = popSizeStack();
									tmpColor = popColorStack();
									
									updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
									
									switch(tmpSize)
									{
										case 0:
										case 6:
											tempOut[tmpDst] = POS_UP;
											tmpDst++;
											tempOut[tmpDst] = POS_UP;
											tmpDst++;
											tempOut[tmpDst] = POS_UP;
											tmpDst++;
											tempOut[tmpDst] = POS_UP;
											tmpDst++;
											break;
										case 1:
										case 4:
											tempOut[tmpDst] = POS_UP;
											tmpDst++;
											tempOut[tmpDst] = POS_UP;
											tmpDst++;
											tempOut[tmpDst] = POS_UP;
											tmpDst++;
											tempOut[tmpDst] = POS_UP;
											tmpDst++;
											tempOut[tmpDst] = POS_UP;
											tmpDst++;
											tempOut[tmpDst] = POS_UP;
											tmpDst++;
											tempOut[tmpDst] = POS_UP;
											tmpDst++;
											break;
										case 2:
										case 5:
											tempOut[tmpDst] = POS_UP;
											tmpDst++;
											tempOut[tmpDst] = POS_UP;
											tmpDst++;
											tempOut[tmpDst] = POS_UP;
											tmpDst++;
											tempOut[tmpDst] = POS_UP;
											tmpDst++;
											tempOut[tmpDst] = POS_UP;
											tmpDst++;
											tempOut[tmpDst] = POS_UP;
											tmpDst++;
											tempOut[tmpDst] = POS_UP;
											tmpDst++;
											tempOut[tmpDst] = POS_UP;
											tmpDst++;
											tempOut[tmpDst] = POS_UP;
											tmpDst++;
											tempOut[tmpDst] = POS_UP;
											tmpDst++;
											break;
										case 3:
											tempOut[tmpDst] = POS_UP;
											tmpDst++;
											tempOut[tmpDst] = POS_UP;
											tmpDst++;
											tempOut[tmpDst] = POS_UP;
											tmpDst++;
											tempOut[tmpDst] = POS_UP;
											tmpDst++;
											tempOut[tmpDst] = POS_UP;
											tmpDst++;
											tempOut[tmpDst] = POS_UP;
											tmpDst++;
											tempOut[tmpDst] = POS_UP;
											tmpDst++;
											tempOut[tmpDst] = POS_UP;
											tmpDst++;
											tempOut[tmpDst] = POS_UP;
											tmpDst++;
											tempOut[tmpDst] = POS_UP;
											tmpDst++;
											tempOut[tmpDst] = POS_UP;
											tmpDst++;
											tempOut[tmpDst] = POS_UP;
											tmpDst++;
											tempOut[tmpDst] = POS_UP;
											tmpDst++;
											break;
									}
								}
							}
							
							else if(strcmp(tag, "/sup") == 0)				
							{
								if(popItemStackByType(SUPER))
								{								
									tmpSize = popSizeStack();
									tmpColor = popColorStack();
									
									updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
									
									tempOut[tmpDst] = POS_DOWN;
									tmpDst++;
									tempOut[tmpDst] = POS_DOWN;
									tmpDst++;
									
									if(tmpSize == 0 || tmpSize == 6)
									{
										tempOut[tmpDst] = POS_DOWN;
										tmpDst++;
										tempOut[tmpDst] = POS_DOWN;
										tmpDst++;
										tempOut[tmpDst] = POS_DOWN;
										tmpDst++;
									}
								}
							}
							
							else
								used = false;
							
							if(used)
								goto endOfLoop; // nasty, but skips doing all the if's later
						}
						
						if(strcmp(tag, "/p") == 0)				
						{
							if(htmlStyle > 0)
							{
								if(popItemStackByType(P))
								{	
									tmpColor = popColorStack();
									
									updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
								}
							}
							
							if(isPElement)
							{	
								if(tmpDst > 0 && tempOut[tmpDst-1] != '\n')
								{						
									tempOut[tmpDst] = '\n';
									tmpDst++;
								}
								
								if(tmpDst > 1 && tempOut[tmpDst-2] != '\n')
								{
									tempOut[tmpDst] = '\n';
									tmpDst++;
								}
							}
							
							isPElement = false;
						}
						
						else if(strcmp(tag, "/b") == 0)				
						{
							if(popItemStackByType(B))
							{	
								tmpBold = false;
								
								if(htmlStyle > 0)
								{
									tmpColor = popColorStack();
								}
								
								updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
							}
						}
						
						else if(strcmp(tag, "/i") == 0)				
						{
							if(popItemStackByType(I))
							{	
								tmpItalics = false;
								
								if(htmlStyle > 0)
								{
									tmpColor = popColorStack();
								}
								
								updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
							}
						}
						
						else if(strcmp(tag, "/em") == 0)				
						{
							if(popItemStackByType(EM))
							{	
								tmpItalics = false;
								
								if(htmlStyle > 0)
								{
									tmpColor = popColorStack();
								}
								
								updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
							}
						}
						
						else if(strcmp(tag, "/strong") == 0)				
						{
							if(popItemStackByType(STRONG))
							{	
								tmpBold = false;
								
								if(htmlStyle > 0)
								{
									tmpColor = popColorStack();
								}
								
								updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
							}
						}
						
						else if(strcmp(tag, "/cite") == 0)				
						{
							if(popItemStackByType(CITE))
							{	
								tmpItalics = false;
								
								if(htmlStyle > 0)
								{
									tmpColor = popColorStack();
								}
								
								updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
							}
						}
						
						else if(strcmp(tag, "/dfn") == 0)				
						{
							if(popItemStackByType(DFN))
							{	
								tmpItalics = false;
								
								if(htmlStyle > 0)
								{
									tmpColor = popColorStack();
								}
								
								updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
							}
						}
						
						else if(strcmp(tag, "/var") == 0)				
						{
							if(popItemStackByType(VAR))
							{	
								tmpItalics = false;
								
								if(htmlStyle > 0)
								{
									tmpColor = popColorStack();
								}
								
								updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
							}
						}
						
						else if(strcmp(tag, "/u") == 0)				
						{
							if(popItemStackByType(U))
							{	
								tmpUnderline = false;
								
								if(htmlStyle > 0)
								{
									tmpColor = popColorStack();
								}
								
								updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
							}
						}
						
						else if(strcmp(tag, "/s") == 0 || strcmp(tag, "/strike") == 0)				
						{
							if(popItemStackByType(STRIKE))
							{	
								tmpStrikeThrough = false;
								
								if(htmlStyle > 0)
								{
									tmpColor = popColorStack();
								}
								
								updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
							}
						}
						
						else if(strcmp(tag, "/font") == 0)				
						{
							if(popItemStackByType(FONT))
							{	
								tmpColor = popColorStack();
								
								updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
							}
						}
						
						else if(strcmp(tag, "/a") == 0)				
						{
							if(popItemStackByType(A))
							{	
								tmpColor = popColorStack();
								tmpUnderline = oldUnderline;
								tmpA = 0;
								
								if(!wasNameTag)
								{
									updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
									
									if(tmpDst > 4)
									{
										if(tempOut[tmpDst - 1] == 0x20 && tempOut[tmpDst - 4] == FORM_IDENTIFIER)
										{
											// some sites leave a trailing space which looks terrible on DSO
											// lets remove it for aesthetics
											
											moveLastItemBack();
										}
									}
								}
							}
						}
						
						else if(strcmp(tag, "/frameset") == 0)				
						{
							if(popItemStackByType(FRAMESET))
							{	
								framesCount--;
								
								if(framesCount < 0)
									framesCount = 0;
							}
						}
						
						else if(strcmp(tag, "/noframes") == 0)				
						{
							if(framesCount > 0)
							{
								if(popItemStackByType(NOFRAMES))
									inNoFrames = false;
							}
						}
						
						else if(strcmp(tag, "/script") == 0)				
						{
							if(popItemStackByType(SCRIPT))
								inScript = false;
						}
						
						else if(strcmp(tag, "/style") == 0)				
						{
							if(popItemStackByType(STYLE))
							{	
								inStyle = false;
								cssTag = false;
								
								if(cssBuf)
								{	
									if(strlen(cssBuf) > 0)
									{
										parseCSS(cssBuf, htmlPage);
									}
									
									trackFree(cssBuf);
									cssBuf = NULL;
								}
							}
						}
						
						else if(strcmp(tag, "/q") == 0)				
						{
							if(popItemStackByType(Q))
							{	
								tempOut[tmpDst] = 34;
								tmpDst++;
							}
						}
						
						else if(strcmp(tag, "/dt") == 0)				
						{
							if(popItemStackByType(DT))
							{	
								tmpBold = false;
								tempOut[tmpDst] = '\n';
								tmpDst++;
								
								updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
							}
						}
						
						else if(strcmp(tag, "/dd") == 0)				
						{
							if(popItemStackByType(DD))
							{	
								tempOut[tmpDst] = '\n';
								tmpDst++;
							}
						}
						
						else if(strcmp(tag, "/ul") == 0)				
						{
							if(popItemStackByType(UL))
							{	
								olCount = popOLStack();
								ulDepth--;
								if(ulDepth == 0)
								{
									tempOut[tmpDst] = '\n';
									tmpDst++;
									tempOut[tmpDst] = '\n';
									tmpDst++;
									
									inLI = false;
								}
								
								if(ulDepth < 0)
									ulDepth = 0;
							}
						}
						
						else if(strcmp(tag, "/ol") == 0)				
						{
							if(popItemStackByType(OL))
							{	
								olCount = popOLStack();
								ulDepth--;
								if(ulDepth == 0)
								{
									tempOut[tmpDst] = '\n';
									tmpDst++;
									tempOut[tmpDst] = '\n';
									tmpDst++;
									
									inLI = false;
								}
								
								if(ulDepth < 0)
									ulDepth = 0;
							}
						}
						
						else if(strcmp(tag, "/td") == 0) // fixes for lazy site coders like gbadev.htm
						{
							if(htmlStyle == 0)
								popItemStackUntilType(TD, TR);
							else
								takeCareOfItem(TD, TR, tempOut, tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpStrikeThrough, tmpSize, tmpColor, tmpA, inCode, oldUnderline, framesCount, inNoFrames, inScript, inStyle, htmlPage);
							
							if(cellPadding > 0)
							{
								for(int cp=0;cp<(int)cellPadding;++cp)
								{
									tempOut[tmpDst] = '\a';
									tmpDst++;
								}
							}
						}
						
						else if(strcmp(tag, "/tr") == 0)				
						{
							if(htmlStyle == 0)
								popItemStackUntilType(TR,TABLE);
							else
								takeCareOfItem(TR, TABLE, tempOut, tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpStrikeThrough, tmpSize, tmpColor, tmpA, inCode, oldUnderline, framesCount, inNoFrames, inScript, inStyle, htmlPage);
							
							if(tmpDst > 0 && tempOut[tmpDst-1] != '\n')
							{						
								tempOut[tmpDst] = '\n';
								tmpDst++;						
							}
							
							if(cellPadding > 0)
							{
								for(int cp=0;cp<(int)cellPadding;++cp)
								{
									tempOut[tmpDst] = '\f';
									tmpDst++;
								}
							}
							
							needsSpacing = false;
						}
						
						else if(strcmp(tag, "/table") == 0)				
						{
							popItemStackUntilType(TABLE, NOELEMENT);
							popTableStack(cellSpacing, cellPadding);
						}
						
						else if(strcmp(tag, "/div") == 0)
						{
							if(htmlStyle > 0)
							{
								if(popItemStackByType(DIV))
								{	
									tmpColor = popColorStack();
									
									updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
								}
							}
						}
						
						else if(strcmp(tag, "/span") == 0)
						{
							if(htmlStyle > 0)
							{
								if(popItemStackByType(SPAN))
								{	
									tmpColor = popColorStack();
									
									updateItem(tmpDst, tmpBold, tmpItalics, tmpUnderline, tmpSize, tmpColor, false, tmpStrikeThrough, tmpA, 0);
								}
							}
						}
						
						else if(strcmp(tag, "/html") == 0 && peekItemStack() == HTML)
							popItemStackUntilType(HTML, NOELEMENT);
						
						else if(strcmp(tag, "/head") == 0 && peekItemStack() == HEAD)
						{
							wasHead = true;
							popItemStackUntilType(HEAD, HTML);
						}
						
						else if(strcmp(tag, "/body") == 0 && peekItemStack() == BODY)
						{
							inBody = false;
							popItemStackUntilType(BODY, HTML);
						}
						
						else if(strcmp(tag, "/title") == 0 && peekItemStack() == TITLE)
						{
							popItemStackUntilType(TITLE, HEAD);
							noTitles = true;
						}
					}
endOfLoop:
					inTag = false;
				}
				else
				{
					if(tTagLen < 1023)
						tag[tTagLen++] = tempStr[tmpSrc];
					
					if(!inProperty && tempStr[tmpSrc] == '=') // fix for babelfish site
						wasEquals = true;
					
					if(tempStr[tmpSrc] == 34 && (wasEquals || inProperty))
					{
						inProperty = !inProperty;
						wasEquals = false;
					}
				}
			}
		}
		else
		{
			if(inTag)
			{
				for(int tF=0;tF<NUM_CRLFS;tF++)
				{
					if(crlfLoc[tF] == 0)
					{
						crlfLoc[tF] = tmpSrc;
						break;
					}
				}
			}
			else
			{
				if(tmpDst > 0 && !isWhiteSpace(tempOut[tmpDst-1]))
				{
					tempOut[tmpDst] = ' ';
					tmpDst++;
				}
			}
		}
		
		tmpSrc++;
		
		if(htmlPage->cS != 0)
			*(htmlPage->cS) = tmpSrc;
	}
	
	if(cssBuf)
		trackFree(cssBuf);
	
	cssBuf = NULL;
	
	tempOut[tmpDst] = POS_DOWN;
	tmpDst++;
	tempOut[tmpDst] = 0;
	tmpDst++;
	tempOut[tmpDst] = 0;
	tmpDst++;
	tempOut[tmpDst] = 0;
	
	htmlPage->rawText = (char *)trackRealloc(tempOut, tmpDst + 1);
	
	tPage->maxControls = controlPos;
	
	a = 0;	
	while(htmlPage->rawText[a] == '\n' || htmlPage->rawText[a] == '\r' || (htmlPage->rawText[a] == 0x20 && (htmlPage->rawText[a+1] == '\n' || htmlPage->rawText[a+1] == '\r')))
		a++;
	
	for(int tmpLoop=0;tmpLoop<512;tmpLoop++)
		htmlPage->colors[tmpLoop] |= BIT(15);
	
	setDefaultFont(htmlPage);
	setColor(tPage->colors[C_DEFAULT]);
	setBold(false);
	setItalics(false);
	setUnderline(false);
	setStrikeThrough(false);
	
	if(htmlPage->content == TYPE_PLAINTEXT)
	{
		tPage->maxControls = 0;
		
		if(htmlPage->controlCodes)
			trackFree(htmlPage->controlCodes);
		htmlPage->controlCodes = NULL;
	}
	
	if(tPage->maxControls)
	{
		while(tPage->controlCodes[0].position - a < 0) // for instances where formatting is applied and then a space is added
		{
			memmove(&tPage->controlCodes[0], &tPage->controlCodes[1], sizeof(CONTROL_ITEM) * (tPage->maxControls - 1));
			
			tPage->maxControls--;
		}
		
		setCallBack(callbackHTMLWrap);
		setTwoPasEnabled(true);
		
		lastPos = tPage->controlCodes[0].position - a;
		controlPos = 0;
		lastFont = 255;
	}
	
	curBuffer = NULL;
	if(htmlPage->cS != 0)
	{
		*(htmlPage->cS) = 0;
		enableLoading(l_rendering);
	}
	
	tPage->wrapCodes = NULL;
	tPage->maxWraps = 0;
	
	htmlPage->height = dispString(0, -1, htmlPage->rawText + a, 0, true, 3, 3, htmlPage->destWidth, 0x7FFFFFFF) + 20;
	
	disableLoading();
	if(tPage->maxControls)
	{
		setTwoPasEnabled(false);
		clearCallBack();
	}
	
	setFont(font_arial_9);
	setColor(0);
	setBold(false);
	setItalics(false);
	setUnderline(false);
	setStrikeThrough(false);
	
	tPage = NULL;
	
	if(duplicateImages)
		trackFree(duplicateImages);
	
	duplicateImages = NULL;
	
	free(tag);
	free(element);
	free(tmpTag);
}

void loadHTMLFromFile(char *htmlFile, HTML_RENDERED *htmlPage)
{
	htmlPage->height = -1;
	memset(htmlPage->title, 0, 256);
	
	if(DRAGON_FileExists(htmlFile) != FE_FILE)
		return;
	
	DRAGON_FILE *fp = DRAGON_fopen(htmlFile, "r");
	int length = DRAGON_flength(fp);
	
	char *tempStr = (char *)trackMalloc(length + 2048 + 1,"Html data"); // 2 kb render buffer
	
	memset(tempStr + 2048, 0, length + 1);
	memset(tempStr, ' ', 2048); // fill with spaces
	
	DRAGON_fread(tempStr + 2048, 1, length, fp); // buffer for memory loading
	DRAGON_fclose(fp);
	
	loadHTMLFromMemory(tempStr, htmlPage);
	
	// fix url positions
	if(htmlPage->maxURLs)
	{
		for(int i=0;i<htmlPage->maxURLs;i++)
		{
			htmlPage->urlCodes[i].url.startPos -= 2048;
			htmlPage->urlCodes[i].url.endPos -= 2048;
		}
	}
	
	// fix form action positions
	if(htmlPage->maxForms)
	{
		for(int i=0;i<htmlPage->maxForms;i++)
		{
			htmlPage->formData[i].url.startPos -= 2048;
			htmlPage->formData[i].url.endPos -= 2048;
		}
	}
	
	// fix image url positions
	if(htmlPage->maxFormControls)
	{
		for(int i=0;i<htmlPage->maxFormControls;i++)
		{
			if(htmlPage->formCodes[i].formID == DUMMYFORM_IMAGES && htmlPage->formCodes[i].whatType == FORM_IMAGE)
			{
				IMAGE_TYPE *tImg = (IMAGE_TYPE *)htmlPage->formCodes[i].state;
				
				tImg->url.endPos -= 2048;
				tImg->url.startPos -= 2048;
			}
		}
	}
}

u32 getCRCFromOffset(char *inStr, LINK_TYPE inURL)
{
	char *inTag = (char *)safeMalloc(inURL.endPos - inURL.startPos + 1);
	
	for(u32 i = inURL.startPos;i < inURL.endPos; i++)
		inTag[i-inURL.startPos] = inStr[i];
	
	inTag[inURL.endPos - inURL.startPos] = 0;
	
	u32 tCRC = CalcCRC32(inTag);
	
	free(inTag);
	
	return tCRC;	
}

void getURLFromPosition(char *fName, LINK_TYPE inURL, char *outURL)
{
	DRAGON_FILE *df = DRAGON_fopen(fName, "r");
	
	DRAGON_fseek(df, inURL.startPos, SEEK_SET);
	
	char *inTag = (char *)safeMalloc(inURL.endPos - inURL.startPos + 1);
	char *andTag = (char *)safeMalloc(1024);
	char *str = inTag;
	
	DRAGON_fread(inTag, 1, inURL.endPos - inURL.startPos, df);
	DRAGON_fclose(df);
	
	inTag[inURL.endPos - inURL.startPos] = 0;
	
	int i = 0;	
	int inAmpersand = false;
	
	while(*str != 0)
	{
		if(inAmpersand)
		{
			if(*str == ';')
			{
				// end & tag
				
				outURL[i] = convertSpecial(andTag);	
				outURL[i+1] = 0;
				i++;
				
				inAmpersand = false;
			}
			else
			{
				int tStore = strlen(andTag);
				if(tStore < 1023)
					andTag[tStore] = *str;
			}
		}
		else
		{
			// set the default state to unparsed
			bool isParsed = false;
			
			if(*str == '&' && *(str+1) != ' ')
			{
				// search for a ; to prove that it's a proper ending tag
				bool isTag = false;
				
				for(int tS=0;tS<10;tS++)
				{
					if(*(str + tS + 1) == ';')
					{
						// we found the end, go ahead and treat it as a tag
						isTag = true;
						break;
					}
				}
				
				if(isTag)
				{
					// start the tag, and make sure we don't add the &
					memset(andTag, 0, 1024);							
					inAmpersand = true;
					isParsed = true;
				}
			}
			
			// add if we haven't parsed
			if(!isParsed)
			{
				outURL[i] = *str;
				outURL[i+1] = 0;
				i++;
			}
		}
		
		str++;
	}
	
	free(inTag);
	free(andTag);
}

void enableURLS()
{
	allowURLS = true;
}

void disableURLS()
{
	allowURLS = false;
}

void setClickPosition(int px, int py)
{
	clickX = px;
	clickY = py;
	
	focusControl = 0;
}

u32 getLastURL()
{
	return realA;
}

void displayHTML(uint16 *buffer, HTML_RENDERED *htmlPage, int line, int numLines)
{
	if(htmlPage->height == -1)
		return;
	
	if(line > htmlPage->height)
		return;
	
	tPage = htmlPage;
	
	curBuffer = buffer;
	destHeight = numLines;
	
	for(int i=0;i<256*numLines;i++)
		buffer[i] = BIT(15) | htmlPage->colors[C_BG];
	
	a = 0;	
	while(htmlPage->rawText[a] == '\n' || htmlPage->rawText[a] == '\r' || (htmlPage->rawText[a] == 0x20 && (htmlPage->rawText[a+1] == '\n' || htmlPage->rawText[a+1] == '\r')))
		a++;
	
	int offset = -1;
	for(int x=0;x<htmlPage->maxWraps;x++)
	{
		if(line > htmlPage->wrapCodes[x].yPos) // found it
			offset = x;
	}
	
	if(offset == -1)
	{
		setDefaultFont(tPage);
		setColor(tPage->colors[C_DEFAULT]);
		setBold(false);
		setItalics(false);
		setUnderline(false);			
		setStrikeThrough(false);
		
		if(tPage->maxControls)
		{
			if(allowURLS)
				setCallBack(callbackHTMLURLS);	
			else
				setCallBack(callbackHTML);	
			
			setTwoPasEnabled(true);
			controlPos = 0;
			lastPass = 0;
			basePos = 0;
		}
	}
	else
	{
		a = htmlPage->wrapCodes[offset].position; // +=
		basePos = htmlPage->wrapCodes[offset].controlPos;
		line -= htmlPage->wrapCodes[offset].yPos;
		
		if(tPage->maxControls)
		{
			if(allowURLS)
				setCallBack(callbackHTMLURLS);	
			else
				setCallBack(callbackHTML);	
			
			setTwoPasEnabled(true);		
			lastPass = 0;
		}
	}
	
	lastA = 0;
	realA = 0;
	
	dispString(0, (-line)-1, htmlPage->rawText + a, buffer, true, 3, 3, htmlPage->destWidth, numLines);		
	
	if(tPage->maxControls)
	{
		setTwoPasEnabled(false);
		clearCallBack();
	}
	
	setFont(font_arial_9);
	setColor(0);
	setBold(false);
	setItalics(false);
	setUnderline(false);
	setStrikeThrough(false);
	
	tPage = NULL;
}

void freeHTML(HTML_RENDERED *htmlPage)
{	
	freeFakeSprite();
	
	if(htmlPage->controlCodes)
		trackFree(htmlPage->controlCodes);
	htmlPage->controlCodes = NULL;
	
	if(htmlPage->rawText)
		trackFree(htmlPage->rawText);
	htmlPage->rawText = NULL;
	
	if(htmlPage->wrapCodes)
		trackFree(htmlPage->wrapCodes);
	htmlPage->wrapCodes = NULL;
	
	if(htmlPage->urlCodes)
		trackFree(htmlPage->urlCodes);
	htmlPage->urlCodes = NULL;

	if(htmlPage->cssLocation)
		free(htmlPage->cssLocation);
	htmlPage->cssLocation = NULL;
	
	if(htmlPage->cssData)
		trackFree(htmlPage->cssData);
	htmlPage->cssData = NULL;
	
	if(htmlPage->maxFormControls)
	{
		for(int i=0;i<htmlPage->maxFormControls;++i)
		{
			switch(htmlPage->formCodes[i].whatType)
			{
				case FORM_TEXT:
				case FORM_PASSWORD:
				case FORM_TEXTAREA:
				case FORM_SUBMIT:
				case FORM_RESET:
				case FORM_BUTTON:
					if(htmlPage->formCodes[i].resetState)
						trackFree(htmlPage->formCodes[i].resetState);
				case FORM_HIDDEN:
				case FORM_RADIO:
					if(htmlPage->formCodes[i].state)
						trackFree(htmlPage->formCodes[i].state);
					
					break;
				case FORM_SELECT:
				{
					FORM_SELECT_ITEM *fsi = (FORM_SELECT_ITEM *)htmlPage->formCodes[i].state;
					
					if(fsi->items)
						trackFree(fsi->items);
					fsi->items = NULL;
					
					switch(fsi->type)
					{
						case 0:
							trackFree(htmlPage->formCodes[i].state);
							
							break;
						case 1:	
							trackFree(htmlPage->formCodes[i].state);
							
							if(htmlPage->formCodes[i].resetState)
								trackFree(htmlPage->formCodes[i].resetState);
							
							break;
					}
					break;
				}
				case FORM_IMAGE:
				{
					if(htmlPage->formCodes[i].state)
					{
						IMAGE_TYPE *tImg = (IMAGE_TYPE *)htmlPage->formCodes[i].state;
						
						if(tImg->state == IMAGE_READYFORDISPLAY)
						{						
							if(tImg->aniData.maxAni > 0)
								trackFree(tImg->aniData.aniDelays);
							
							if(tImg->graphicsData)
								trackFree(tImg->graphicsData);
						}
						
						trackFree(htmlPage->formCodes[i].state);
					}
					break;
				}
			}
			
			htmlPage->formCodes[i].state = NULL;
			htmlPage->formCodes[i].resetState = NULL;
		}
	}
	
	if(htmlPage->formCodes)
		trackFree(htmlPage->formCodes);
	htmlPage->formCodes = NULL;
	
	if(htmlPage->formData)
		trackFree(htmlPage->formData);
	htmlPage->formData = NULL;
	
	htmlPage->height = -1;
}

void runAnimatedImages(HTML_RENDERED *htmlPage)
{	
	if(htmlPage->maxFormControls)
	{
		u32 curHeartBeat = getAniHeartBeat();
		
		for(int i=0;i<htmlPage->maxFormControls;++i)
		{
			if(htmlPage->formCodes[i].whatType == FORM_IMAGE)
			{
				// check to see if we should handle animation
				
				IMAGE_TYPE *tImg = (IMAGE_TYPE *)htmlPage->formCodes[i].state;
				
				if(tImg->state == IMAGE_READYFORDISPLAY && tImg->aniData.maxAni > 0)
				{
					// it's animated, lets see if we have waited enough time
					
					u32 runTime = curHeartBeat - tImg->aniData.vBlankStart;
					
					if(runTime >= (u32)tImg->aniData.aniDelays[tImg->aniData.curAni])
					{
						// it's passed the minimum time, change it
						
						tImg->aniData.vBlankStart = curHeartBeat;
						tImg->aniData.curAni++;
						
						if(tImg->aniData.curAni >= tImg->aniData.maxAni)
							tImg->aniData.curAni = 0;
					}
				}
			}
		}
	}
}

bool isLoaded(HTML_RENDERED *htmlPage)
{
	if(htmlPage->height == -1)
		return false;
	
	return true;
}

void setHTMLWidth(int width, HTML_RENDERED *htmlPage)
{
	htmlPage->destWidth = width;
}

void setProgressVars(u32 *cSZ, u32 *mSZ, HTML_RENDERED *htmlPage)
{
	htmlPage->cS = cSZ;
	htmlPage->mS = mSZ;
}

int controlFocus()
{
	return focusControl;
}

void clearFocus()
{
	focusControl = 0;
}

void setEncoding(int encoding, HTML_RENDERED *htmlPage)
{
	htmlPage->encoding = encoding;
}

void setContent(int content, HTML_RENDERED *htmlPage)
{
	htmlPage->content = content;
}

uint16 getBGColor(HTML_RENDERED *htmlPage)
{
	return htmlPage->colors[C_BG];	
}


