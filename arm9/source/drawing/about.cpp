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
#include <libfb/libcommon.h>
#include "about.h"
#include "home.h"
#include "general.h"
#include "html.h"
#include "controls.h"
#include "help.h"

// About source
#include "about_html.h"

static HTML_RENDERED aboutData;
static char *abScreen = NULL;

void drawAboutScreen()
{
	#ifndef DEBUG_MODE
	displayHTML(fb_backBuffer(), &aboutData, 0, 192);
	#endif
	displayHTML(bg_backBuffer(), &aboutData, 192, 192);
}

void freeAbout()
{
	freeHTML(&aboutData);
}

void exitAbout(int x, int y)
{
	returnHome();
}

void loadBrowser(int x, int y)
{
	/*setMode(WEBBROWSER);
	initWebBrowser();
	
	setTemporaryHome("http://www.dragonminded.com/ndsdev/");*/
}

void initAbout()
{
	setEncoding(CHARSET_ASCII, &aboutData);
	setHTMLWidth(ABOUT_HELP_WIDTH, &aboutData);
	setContent(TYPE_HTML, &aboutData);
	
	abScreen = (char *)trackMalloc(strlen((char *)about_html)+1, "temp about");
	memcpy(abScreen, about_html, strlen((char *)about_html)+1);
	
	loadHTMLFromMemory(abScreen, &aboutData);
	
	clearControls();
	
	registerScreenUpdate(drawAboutScreen, freeAbout);
	registerHelpScreen(NULL, true);
	
	newControl(0xFF, 32, 70, 244, 79, CT_HITBOX, NULL);
	setControlCallbacks(0xFF, loadBrowser, NULL, NULL);
	addHotKey(0xF0, KEY_START, exitAbout, 0, NULL);
}
