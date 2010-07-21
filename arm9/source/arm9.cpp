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
 
#include <stdio.h>
#include <stdlib.h>
#include <nds.h>
#include <string.h>
#include <exceptions.h>
#include <libfb/libcommon.h>
#include <libdt/libdt.h>
#include <libpicture.h>
#include <nds/arm9/sound.h>
#include "fatwrapper.h"
#include "keyboard.h"
#include "fonts.h"
#include "settings.h"
#include "graphics.h"
#include "filerout.h"
#include "globals.h"
#include "settings.h"
#include "language.h"
#include "general.h"
#include "sound.h"
#include "wifi.h"
#include "colors.h"
#include "lcdfunctions.h"
#include "controls.h"
#include "clipboard.h"
#include "resources.h"
#include "splash.h"
#include "html.h"
#include "home.h"
#include "calendar.h"
#include "cyg-profile.h"
#include "help.h"
#include "todo.h"
#include "soundplayer.h"
#include "webbrowser.h"

// internals
static bool hasTouched = false;

// key inputs
void touchDown(int px, int py)
{	
	if(!hasTouched)
	{
		queueControls(px, py, CE_DOWN);
		hasTouched = true;
	}
}

void touchUp()
{
	if(hasTouched)
	{
		queueControls(0, 0, CE_UP);
		hasTouched = false;
	}
}

bool checkSleepState()
{
	if(getLCDState() == LCD_OFF)
	{
		if(!(keysHeld() & KEY_LID))
		{		
			setLCDState(LCD_ON);
			return true;
		}
	}
	else
	{
		if(keysHeld() & KEY_LID)
		{
			setLCDState(LCD_OFF);
			return true;
		}
	}
	
	return false;
}

void checkKeys()
{
	// Only for inside sound loops
	scanKeys();
	
	touchPosition touch = touchReadXYNew();
	
	if((keysHeld() & KEY_TOUCH) && hasTouched)
	{
		// Do the move event
		queueControls(touch.px, touch.py, CE_MOVE);
		return;
	}
	
	queueHotKeys(keysDown(), keysHeld());
}

//-------------
// main program
//-------------

void initProgram()
{
	//------------------------
	// start initializing crap
	//------------------------
	defaultExceptionHandler(); // set up the exception handler
	
    powerON(POWER_ALL_2D); // turn on everything
	
	#ifndef DEBUG_MODE
	fb_init(); // initialize top screen video
	#else
	debugInit();
	#endif
	bg_init(); // initialize bottom screen video
	
	setNewOrientation(ORIENTATION_0);
	
	// set up extra vram banks to be scratch memory	
	vramSetBankE(VRAM_E_LCD);
	vramSetBankF(VRAM_F_LCD);
	vramSetBankG(VRAM_G_LCD);
	vramSetBankH(VRAM_H_LCD);
	vramSetBankI(VRAM_I_LCD);

	fb_setBGColor(30653);
	bg_setBGColor(0);
	drawStartSplash();
	
	lcdMainOnTop(); // set fb to top screen
	fb_swapBuffers();
	bg_swapBuffers();
	
	// out of order for competition
	irqInit(); // initialize irqs
	
    irqSet(IRQ_VBLANK, startvBlank);
	irqEnable(IRQ_VBLANK);
	
	setMode(INITFAT);
	setSoundInterrupt(); // initialize fifo irq
	
	setGenericSound(11025, 127, 64, 1);
	waitForInit(); // wait until arm7 has loaded and gone to sleep
	initComplexSound(); // initialize sound variables
	initWifi();
	fixGautami();
	
	setCursorProperties(0, 2, 0, 0);
	initCapture();
	initClipboard();
	
	// set defaults to english in case we can't load the langauge file for some reason
	// also takes care of partial translations.	
	initLanguage();
	initRandomList();
	fixAndTags();
	
	resetKeyboard();
	setDate();
	
	if(!DRAGON_InitFiles())
	{
		// oops, no cf card!
		setMode(DISPLAYCOW);
		
		setFont(font_arial_11);
		setColor(0xFFFF);
		
		bg_dispSprite(96, 5, errmsg, 0);
		bg_setClipping(5,25,250,181);
		bg_dispString(0,0,l_nofat);
		bg_swapBuffers();
		
		while(1)
		{
			// wee, la la la!
		}
	}
	
	//--------------------------------------------------------------------
	//finished init, now check to make sure the DSOrganize dir is there...
	//--------------------------------------------------------------------
	
	findDataDirectory();
	makeDirectories();
	
	if(DRAGON_FileExists("DSOrganize") != FE_DIR)
	{
		setMode(DISPLAYCOW);
		
		// oops, not there, we must create!
		DRAGON_mkdir("DSOrganize");
		DRAGON_chdir("DSOrganize");
		DRAGON_mkdir("DAY");
		DRAGON_mkdir("HELP");
		DRAGON_mkdir("LANG");
		DRAGON_mkdir("RESOURCES");
		DRAGON_mkdir("REMINDER");
		DRAGON_mkdir("SCRIBBLE");
		DRAGON_mkdir("TODO");
		DRAGON_mkdir("VCARD");
		DRAGON_mkdir("ICONS");
		DRAGON_mkdir("CACHE");
		DRAGON_mkdir("COOKIES");
		DRAGON_mkdir("HOME");
		DRAGON_chdir("/");
		
		makeDefaultSettings();
		
		setFont(font_arial_11);
		setColor(0xFFFF);
		
		bg_dispSprite(96, 5, errmsg, 0);
		bg_setClipping(5,25,250,181);
		bg_dispString(0,0, l_createdir);
		bg_swapBuffers();
		
		while(!keysDown())
		{
			scanKeys();	
		}
	}
	
	setMode(INITPLUGIN);
	
	//-------------------------------------------------------------------
	//finished creating dirs, now check to make sure if they extracted it
	//did their extracting program actually get all the dirs?
	//-------------------------------------------------------------------	
	
	DRAGON_chdir(d_base);
	
	if(DRAGON_FileExists("Day") != FE_DIR)
	{
		DRAGON_mkdir("DAY");
	}
	if(DRAGON_FileExists("Help") != FE_DIR)
	{
		DRAGON_mkdir("HELP");
	}
	if(DRAGON_FileExists("Lang") != FE_DIR)
	{
		DRAGON_mkdir("LANG");
	}
	if(DRAGON_FileExists("Reminder") != FE_DIR)
	{
		DRAGON_mkdir("REMINDER");
	}
	if(DRAGON_FileExists("Scribble") != FE_DIR)
	{
		DRAGON_mkdir("SCRIBBLE");
	}
	if(DRAGON_FileExists("Todo") != FE_DIR)
	{
		DRAGON_mkdir("TODO");
	}
	if(DRAGON_FileExists("VCard") != FE_DIR)
	{
		DRAGON_mkdir("VCARD");
	}
	if(DRAGON_FileExists("Icons") != FE_DIR)
	{
		DRAGON_mkdir("ICONS");
	}
	if(DRAGON_FileExists("Cache") != FE_DIR)
	{
		DRAGON_mkdir("CACHE");
	}
	if(DRAGON_FileExists("Cookies") != FE_DIR)
	{
		DRAGON_mkdir("COOKIES");
	}
	if(DRAGON_FileExists("Home") != FE_DIR)
	{
		DRAGON_mkdir("HOME");
	}
	
	DRAGON_chdir("/");
	
	//-------------------------------------------
	//how about we load the settings for them eh?
	//-------------------------------------------
	loadSettings();
	
	DRAGON_chdir(d_base);
	
	if(DRAGON_FileExists("startup.wav") == FE_FILE)
	{
		char tStr[256];
		
		sprintf(tStr, "%sstartup.wav", d_base);
		loadWavToMemory();
		loadSound(tStr);
	}
	
	DRAGON_chdir("/");
	initStartScreen();
	
    irqSet(IRQ_VBLANK, vBlank);	
	fb_setBGColor(genericFillColor);
	bg_setBGColor(genericFillColor);
}

#ifdef PROFILING
unsigned int hblanks = 0;

void hblank_handler(void)
{
	hblanks++;
}
#endif

int main()
{	
	initProgram();
	
	#ifdef PROFILING
	int counter = 0;
	
	irqSet(IRQ_HBLANK, hblank_handler);
	irqEnable(IRQ_HBLANK);
	
	cygprofile_begin();
	cygprofile_enable();
	#endif
	
	while(1) 
	{
		updateStreamLoop();
		if(!checkHelp())
		{	
			if(getLCDState() == LCD_ON)
			{
				updateStreamLoop();
				clearHelpScreen();
			}
			
			updateStreamLoop();
			drawControls(getLCDState() != LCD_ON);
			
			updateStreamLoop();
			checkKeys();			
			executeQueuedControls();
			
			// Split here because the state can change in checkKeys
			if(getLCDState() == LCD_ON)
			{
				#ifdef SCREENSHOT_MODE
				takeScreenshot();			
				#endif
				
				updateStreamLoop();
				drawToScreen();
			}
			else
			{	
				updateStreamLoop();
				checkEndSound();
			}
			
			updateStreamLoop();
			checkSleepState();
		}
		
		#ifdef PROFILING
		counter++;
		
		if(counter == 700)
		{
			cygprofile_disable();
			cygprofile_end();
		}
		#endif
	}
}
