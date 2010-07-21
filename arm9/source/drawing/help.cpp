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
#include "help.h"
#include "settings.h"
#include "html.h"
#include "general.h"
#include "globals.h"

extern char curLang[30];
extern bool allowSwapBuffer;
extern u32 htmlStyle;

static char *helpLoc = NULL;
static bool helpSwap = false;
static bool helpLoaded = false;
static HTML_RENDERED htmlPage;

void registerHelpScreen(char *fName, bool hSwap)
{	
	if(helpLoc)
	{
		free(helpLoc);
	}
	
	clearHelpScreen();
	
	if(fName == NULL)
	{
		helpLoc = NULL;
		return;
	}
	
	helpLoc = (char *)safeMalloc(strlen(fName) + 1);
	
	strcpy(helpLoc, fName);
	helpSwap = hSwap;
}

bool isHelpPage()
{
	return helpLoc != NULL;
}

void drawHelpScreen()
{
	if(!helpLoaded)
	{
		if(helpLoc == NULL)
		{
			return;
		}
		
		if(strlen(helpLoc) == 0)
		{
			return;
		}
		
		u32 oldText = htmlStyle;
		htmlStyle = 1;
		
		char str[256];
		sprintf(str, "%s%s/%s", d_help, curLang, helpLoc);
		
		setHTMLWidth(250, &htmlPage);
		setEncoding(0, &htmlPage);
		setContent(TYPE_HTML, &htmlPage);
		loadHTMLFromFile(str, &htmlPage);
		
		if(!isLoaded(&htmlPage))
		{
			sprintf(str, "%s%s/%s", d_help, "english", helpLoc);			
			loadHTMLFromFile(str, &htmlPage); // try the english one
			
			if(!isLoaded(&htmlPage))
			{
				char *t = "<html><head><title>Error Loading</title></head><body>There was an error loading the respective help file for this page.</body></html>";
				char *tSpace = (char *)trackMalloc(strlen(t)+1, "tmp render");
				memcpy(tSpace, t, strlen(t)+1);
				
				loadHTMLFromMemory(tSpace, &htmlPage);
			}
		}
		
		htmlStyle = oldText;
		helpLoaded = true;
	}
	
	#ifndef DEBUG_MODE
	if(!helpSwap)
	{
		allowSwapBuffer = false;
		disableBackgroundDraw();
		displayHTML(bg_backBuffer(), &htmlPage, 0, 192);
	}
	else
	{
		allowSwapBuffer = true;		
		disableBackgroundDraw();
		displayHTML(fb_backBuffer(), &htmlPage, 0, 192);
		displayHTML(bg_backBuffer(), &htmlPage, 192, 192);
	}
	#endif
}

void clearHelpScreen()
{
	if(helpLoaded)
	{
		helpLoaded = false;
		freeHTML(&htmlPage);
		memset(&htmlPage, 0, sizeof(HTML_RENDERED));
		
		fb_setClipping(0,0,256,192);
		bg_setClipping(0,0,256,192);
		
		if(getMode() == SOUNDPLAYER)
		{
			fb_swapBuffers();
		}
	}
}
