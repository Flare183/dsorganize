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
 
#ifndef _HTML_INCLUDED
#define _HTML_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <nds.h>
#include <stdio.h>
#include "globals.h"

#define F_BOLD			(BIT(16))
#define F_ITALICS		(BIT(17))
#define F_UNDERLINE		(BIT(18))
#define F_STRIKETHROUGH	(BIT(19))
#define F_HR			(BIT(20))

#define F_GETSIZE(x)	(((x) & 0xF0000000) >> 28)
#define F_SETSIZE(x)	(((x) & 0x0000000F) << 28)
#define F_GETFORM(x)	(((x) & 0x0F000000) >> 24)
#define F_SETFORM(x)	(((x) & 0x0000000F) << 24)

#define M_GET 0
#define M_POST 1

#define MAX_WRAPS 100 //50

// crlf's to track inside tags
#define NUM_CRLFS 10

#define C_DEFAULT 0
#define C_LINK 1
#define C_ALINK 2
#define C_VLINK 3
#define C_BG 4
#define F_BORDER 5
#define F_TEXT 6
#define F_BG 7
#define C_START 8

#define DUMMYFORM_IMAGES 0xFFFFFFFF
#define FORM_IDENTIFIER 30

#define FORM_NOTHING 0
#define FORM_TEXT 1
#define FORM_PASSWORD 2
#define FORM_CHECK 3
#define FORM_HIDDEN 4
#define FORM_RADIO 5
#define FORM_SUBMIT 6
#define FORM_RESET 7
#define FORM_BUTTON 8
#define FORM_TEXTAREA 9
#define FORM_SELECT 10
#define FORM_IMAGE 11

// Encoding
#define CHARSET_ASCII 0
#define CHARSET_UTF8 1
#define CHARSET_8859 2

// Content
#define TYPE_HTML 0
#define TYPE_PLAINTEXT 1
#define TYPE_IMAGE 2

// Image states
#define IMAGE_NEEDSDOWNLOAD 0
#define IMAGE_READYFORDISPLAY 1
#define IMAGE_CANTLOAD 2

#define IMAGE_PADDING 7
#define DEFAULT_TABLE_SPACING 2
#define DEFAULT_TABLE_PADDING 0

#define CSS_SIZE 20480

typedef struct
{
	char tag[10];
	char outChar;
	char lf;
} TAG_TYPE;

typedef struct
{
	u32 startPos;
	u32 endPos;
} LINK_TYPE;

typedef struct
{
	u32 crc;
	u32 whichFormElement;
} DUPLICATE_IMAGE_TYPE;

typedef struct
{
	u32 curAni;
	u32 maxAni;
	u32 vBlankStart;
	int *aniDelays;
} ANIMATION_TYPE;

typedef struct
{
	LINK_TYPE url;
	uint16 *graphicsData;
	u32 state;
	ANIMATION_TYPE aniData;
} IMAGE_TYPE;

typedef struct
{
	int position;
	u32 whichA;
	u32 whichForm;
	u32 fontOptions;
} CONTROL_ITEM;

typedef struct
{
	u32 yPos; // for anchors
	LINK_TYPE url;
	char name[64];
} URL_ITEM;

typedef struct
{
	int yPos;
	int position;
	CONTROL_ITEM *lastItem;
	int controlPos;
} WRAP_ITEM;

typedef struct
{
	LINK_TYPE url;
	u32 method;	
} FORM_SUBMIT_ITEM;

typedef struct
{
	u32 formID;
	int xPos;
	int yPos;
	int width;
	int height;
	
	int whatType;
	int maxLength;
	char name[64];
	void *state;
	void *resetState;
} FORM_ITEM;

typedef struct
{
	int depth; // for optgroup, 0 = normal, 1+ = intended, -1, -2, etc = unselectable optgroup label
	int highlighted;
	
	char caption[128];
	char value[64];
} SELECT_ITEM;

typedef struct
{
	u32 type;
	u32 focused;
	int cursor;
	
	SELECT_ITEM *items;
} FORM_SELECT_ITEM;

typedef struct
{
	int colorIndex;
	char cssElement[31];
	char cssClass[31];
	int relativeIndex;
	bool hasFilled;
} CSS_ITEM;

typedef struct
{
	char title[256];
	char *rawText;
	
	int destWidth;
	int height;
	int encoding;
	int content;
	
	uint16 colors[512];	
	u32 maxColors;
	
	CONTROL_ITEM *controlCodes;
	int maxControls;
	
	WRAP_ITEM *wrapCodes;
	int maxWraps;
	
	URL_ITEM *urlCodes;
	int maxURLs;
	
	FORM_ITEM *formCodes;
	int maxFormControls;
	
	FORM_SUBMIT_ITEM *formData;
	int maxForms;
	
	CSS_ITEM *cssData;
	int maxCSSElements;
	
	char *cssLocation;
	
	u32 *cS;
	u32 *mS;
} HTML_RENDERED;

void loadHTMLFromFile(char *htmlFile, HTML_RENDERED *htmlPage);
void loadHTMLFromMemory(char *tempStr, HTML_RENDERED *htmlPage);
void displayHTML(uint16 *buffer, HTML_RENDERED *htmlPage, int line, int numLines);
void freeHTML(HTML_RENDERED *htmlPage);
bool isLoaded(HTML_RENDERED *htmlPage);
void setHTMLWidth(int width, HTML_RENDERED *htmlPage);
void setProgressVars(u32 *cSZ, u32 *mSZ, HTML_RENDERED *htmlPage);
void runAnimatedImages(HTML_RENDERED *htmlPage);
void parseCSS(char *cssSrc, HTML_RENDERED *htmlPage);
void fixBadColor(HTML_RENDERED *htmlPage);

// for url clicking
void enableURLS();
void disableURLS();
u32 getLastURL();
void setClickPosition(int px, int py);
void setClicking(u32 tmpURL, uint16 action, HTML_RENDERED *htmlPage);
int controlFocus();
void clearFocus();
void setEncoding(int encoding, HTML_RENDERED *htmlPage);
void setContent(int content, HTML_RENDERED *htmlPage);
void getURLFromPosition(char *fName, LINK_TYPE inURL, char *outURL);
uint16 getBGColor(HTML_RENDERED *htmlPage);

// initializing data
void fixAndTags();

#ifdef __cplusplus
}
#endif

#endif
