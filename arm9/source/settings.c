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
 
#include <string.h>
#include <nds.h>
#include <stdio.h>
#include <libfb/libcommon.h>
#include <ini.h>
#include "fonts.h"
#include "fatwrapper.h"
#include "language.h"
#include "general.h"
#include "settings.h"
#include "resources.h"
#include "http.h"
#include "graphics.h"
#include "controls.h"
#include "globals.h"
#include "scribble.h"
#include "irc.h"
#include "configuration.h"
#include "soundplayer.h"
#include "home.h"

extern void FAT_DisableWriting(); // for write setting

char lLanguage[LANG_SIZE];
char cIconSet[ICON_SIZE];
bool milTime = false;
bool firstLast = false;
bool showHidden = true;
u32 htmlStyle = 2;
bool reverseDate = false;
bool disablePatching = false;
extern int locations[12];
extern int homemode[12];
bool disableWrites = false;
char defaultSavePath[256];
char nickName[NICK_LEN];
char altnickName[NICK_LEN];
char ircServer[60];
bool autoConnect = false;
bool autoBullet = false;
int normalBoot = BOOT_CHISHM;
int altBoot = BOOT_MIGHTYMAX;
int wifiMode = WIFI_FIRMWARE;
int startMode = HOME;

static WIFI_PROFILE *dsoProfiles = NULL;
static char proxy[256];
static char dldiFile[256];
static char homePage[256];
static char downloadLocation[256];
static int mode = HOME;
static int lastMode;
static int saveFormat = SAVE_BMP;
static bool swapAB = false;
static bool textFixedWidth = false;
static bool ircFixedWidth = false;
static bool secondClickOpens = true;
static bool autoHide = true;
static bool imageLandscape = true;
static bool keyClick = false;


uint16 widgetBorderColor = 0x0000;
uint16 widgetFillColor = 0xFFFF;
uint16 widgetTextColor = 0x0000;
uint16 widgetHighlightColor = 0x7E00;
uint16 scrollFillColor = 0xFFFF;
uint16 scrollHighlightColor = 0x7E00;
uint16 scrollNormalColor = 0x0000;
uint16 genericHighlightColor = 0x6776;
uint16 genericTextColor = 0x0000;
uint16 genericFillColor = 0xFFFF;
uint16 genericProgressColor = 0x7F11; //0x6F7B
uint16 listBorderColor = 0x7BDE;
uint16 listFillColor = 0x7BDE;
uint16 listTextColor = 0x0000;
uint16 textEntryTextColor = 0x294A;
uint16 textEntryFillColor = 0xFFFF;
uint16 textEntryHighlightColor = 0x7E00;
uint16 textEntryProgressColor = 0x7F11; //0x6F7B
uint16 textAreaTextColor = 0x0000;
uint16 textAreaFillColor = 0xFFFF;
uint16 cursorNormalColor = 0x0000;
uint16 cursorOverwriteColor = 0x001F;
uint16 calendarWeekendColor = 0x7E00;
uint16 calendarNormalColor = 0x0000;
uint16 calendarHasDayPlannerColor = 0x4010;
uint16 calendarCurrentDateColor = 0x001F;
uint16 calencarHasReminderColor = 0x221F;
uint16 calendarDayUnderlineColor = 0x2529;
uint16 dayPlannerBorderColor = 0x0000;
uint16 browserHiddenColor = 0x3DEF;
uint16 configurationArrowColor = 0x0000;
uint16 keyboardBorderColor = 0x0000;
uint16 keyboardFillColor = 0xFFFF;
uint16 keyboardSpecialColor = 0x5EF7;
uint16 keyboardABCSwapColor = 0x6FBA;
uint16 keyboardHighlightColor = 0x7E00;
uint16 keyboardSpecialHighlightColor = 0x001F;
uint16 keyboardTextColor = 0x0000;
uint16 keyboardMEDSTextColor = 0x294A;
uint16 homeDateTextColor = 0x0000;
uint16 homeDateFillColor = 0x6739;
uint16 homeDateBorderColor = 0x0000;
uint16 homeDateBorderFillColor = 0x34C0;
uint16 homeHighlightColor = 0x0200;
uint16 pictureSelectBorderColor = 0x001F;
uint16 soundSeekFillColor = 0xFFFF;
uint16 soundSeekForeColor = 0x7E00;
uint16 soundSeekDisabledColor = 0x5294;
uint16 soundMetaTextColor = 0x294A;
uint16 soundHoldColor = 0x5294;
uint16 soundEQMedianColor = 0x001F;
uint16 soundEQDataColor = 0x8200;
uint16 soundArrowColor = 0x0000;
uint16 calculatorScreenFillColor = 0xFFFF;
uint16 calculatorScreenTextColor = 0x0000;
uint16 calculatorScreenBorderColor = 0x0000;
uint16 calculatorScreenFadeTextColor = 0x5294;
uint16 calculatorButtonTextColor = 0x0000;
uint16 calculatorButtonBorderColor = 0x0000;
uint16 calculatorNormalFillColor = 0xFFFF;
uint16 calculatorMemoryFillColor = 0x5F7F;
uint16 calculatorSpecialFillColor = 0x739C;
uint16 calculatorUnaryFillColor = 0x6BBB;
uint16 calculatorBinaryFillColor = 0x7F39;
uint16 calculatorButtonHighlightColor = 0x7E00;
uint16 texteditorComment = 0x5A08;
uint16 texteditorProperty = 0x0000;
uint16 texteditorSection = 0x4010;
uint16 texteditorSetting = 0x7C00;
uint16 viewerBookmarkColor = 0x001F;
uint16 loadingBorderColor = 0x0000;
uint16 loadingFillColor = 0xFFFF;
uint16 loadingBarColor = 0x7E00;
uint16 ircTabFillColor = 0xFFFF;
uint16 ircTabBorderColor = 0x0000;
uint16 ircTabTextColor = 0x0000;
uint16 ircTabHighlightColor = 0x7E00;
uint16 ircTabMoreTabsColor = 0x77BD;
uint16 ircTabImprtantTextColor = 0x001F;
uint16 ircTabUnimportantTextColor = 0x7C00;
uint16 ircScreenFillColor = 0xFFFF;
uint16 ircScreenBorderColor = 0x000;
uint16 ircScreenTextColor = 0x0000;
char ircColorNOTICE = 7;
char ircColorACTION = 13;
char ircColorCTCP = 4;
char ircColorNORMAL = 0;
char ircColorSERVER = 12;
char ircColorDSO = 10;
char ircColorNICK = 3;
char ircColorTOPIC = 3;
char ircColorMODE = 12;
char ircColorHIGHLIGHT = 9;

// accessor functions
void setStartMode(int m)
{
	startMode = m;
}

char *getProxy()
{
	return proxy;
}

char *getDLDI()
{
	return dldiFile;
}

char *getHomePage()
{
	return homePage;
}

char *getDownloadDir()
{
	return downloadLocation;
}

int getMode()
{
	return mode;	
}

void setMode(int newMode)
{
	mode = newMode;
}

int getLastMode()
{
	return lastMode;
}

void setLastMode(int oldMode)
{
	lastMode = oldMode;
}

void setSaveFormat(int sf)
{
	saveFormat = sf;
}

int getSaveFormat()
{
	return saveFormat;
}

bool isABSwapped()
{
	return swapAB;
}

void toggleABSwap()
{
	swapAB = !swapAB;
}

bool keyClickEnabled()
{
	return keyClick;
}

void toggleKeyClick()
{
	keyClick = !keyClick;
}

void toggleImageLandscape()
{
	imageLandscape = !imageLandscape;
}

bool isImageLandscape()
{
	return imageLandscape;
}

bool isAutoHide()
{
	return autoHide;
}

void toggleAutoHide()
{
	autoHide = !autoHide;
}

bool secondClickAction()
{
	return secondClickOpens;
}

void toggleSecondClickOption()
{
	secondClickOpens = !secondClickOpens;
}

bool isReverseDate()
{
	return reverseDate;
}

bool isTextEditorFixed()
{
	return textFixedWidth;
}

bool isIRCFixed()
{
	return ircFixedWidth;
}

uint16 **setTextEditorFont()
{
	if(textFixedWidth)
	{
		return font_profont_9;
	}
	else
	{
		return font_arial_9;
	}
}

uint16 **setIRCFont()
{
	if(ircFixedWidth)
	{
		return font_profont_9;
	}
	else
	{
		return font_gautami_10;
	}
}

void toggleTextFixed()
{
	textFixedWidth = !textFixedWidth;
}

void toggleIRCFixed()
{
	ircFixedWidth = !ircFixedWidth;
}

void setLocations(char *str)
{
	int x = 0;
	int item = 0;
	
	char tmpStr[20];
	memset(tmpStr, 0, 20);
	
	while(str[x] != 0)
	{
		if(str[x] == ',')
		{
			// check here for what item to set for
			
			strlwr(tmpStr);
			
			if(strcmp(tmpStr,"calendar") == 0)
				locations[item] = 0; //CALENDAR
			if(strcmp(tmpStr,"dayview") == 0)
				locations[item] = 1; //DAYVIEW
			if(strcmp(tmpStr,"address") == 0)
				locations[item] = 2; //ADDRESS;
			if(strcmp(tmpStr,"todo") == 0)
				locations[item] = 3; //TODOLIST;
			if(strcmp(tmpStr,"scribble") == 0)
				locations[item] = 4; //SCRIBBLEPAD;
			if(strcmp(tmpStr,"browser") == 0)
				locations[item] = 5; //BROWSER;
			if(strcmp(tmpStr,"calculator") == 0)
				locations[item] = 6; //CALCULATOR;
			if(strcmp(tmpStr,"irc") == 0)
				locations[item] = 7; //IRC;
			if(strcmp(tmpStr,"webbrowser") == 0)
				locations[item] = 8; //WEBBROWSER;
			if(strcmp(tmpStr,"hbdb") == 0)
				locations[item] = 9; //HBDB;
			
			memset(tmpStr, 0, 20);
			item++;
		}
		else
			tmpStr[strlen(tmpStr)] = str[x];
		
		x++;
	}
}

void loadExternalDLDI()
{
	char tmpFile[256];
	int fType;
	
	memset(dldiFile, 0, 256);
	
	DRAGON_chdir(d_res);
	fType = DRAGON_FindFirstFile(tmpFile);
	
	while(fType != FE_NONE)
	{
		if(fType == FE_FILE)
		{
			char ext[256];
			
			separateExtension(tmpFile, ext);
			strlwr(ext);
			
			if(strcmp(ext, ".dldi") == 0)
			{
				// dldi file
				
				strcpy(dldiFile, tmpFile);
				strcat(dldiFile, ext);
				
				break;
			}
		}
		
		fType = DRAGON_FindNextFile(tmpFile);		
	}
	
	DRAGON_closeFind();
	DRAGON_chdir("/");
}

void loadSettings()
{
	char sStr[256];
	
	defaultLocations();
	
	sprintf(sStr, "%sconfig.ini", d_base);	
	
	if(DRAGON_FileExists(sStr) != FE_FILE)
		makeDefaultSettings();
		
	setIniFile(sStr);
	
	if(getSetting("HomeScreen", "Order", sStr))
	{
		strcat(sStr,",");
		setLocations(sStr);
	}
	
	if(getSetting("HomeScreen", "IconSet", sStr))
		strcpy(cIconSet, sStr);
	else
		strcpy(cIconSet, "Default");
	
	if(getSetting("Regional", "Language", sStr))
	{
		strlwr(sStr);
		
		strcpy(lLanguage, sStr);
		loadLanguage(sStr);
	}
	else
	{
		strcpy(lLanguage, "english");
		loadLanguage(lLanguage);
	}
	
	if(getSetting("General", "HTMLStyle", sStr))
	{
		htmlStyle = sStr[0] - '0';
		
		if(htmlStyle > 2)
			htmlStyle = 2;
	}
	else
	{
		htmlStyle = 2;
	}
	
	milTime = getBoolSetting("Regional", "Display24h");
	firstLast = getBoolSetting("Regional", "DisplayFirstLast");
	reverseDate = getBoolSetting("Regional", "ReverseDate");	
	secondClickOpens = getBoolSetting("General", "SecondClickAction");
	swapAB = getBoolSetting("General", "swapAB");
	disableWrites = getBoolSetting("General", "DisableWrites");
	keyClick = getBoolSetting("General", "KeyboardNoise");
	autoBullet = getBoolSetting("Todo", "AutoBullet");
	showHidden = getBoolSetting("Browser", "ShowHidden");
	disablePatching = getBoolSetting("Browser", "DisablePatching");
	
	normalBoot = BOOT_CHISHM;
	altBoot = BOOT_MIGHTYMAX;

	if(getSetting("General", "NormalLoader", sStr))
	{
		strlwr(sStr);	
		
		if(strcmp(sStr, "mightymax") == 0)
			normalBoot = BOOT_MIGHTYMAX;
		if(strcmp(sStr, "chishm") == 0)
			normalBoot = BOOT_CHISHM;
	}	
	
	if(getSetting("General", "AlternateLoader", sStr))
	{
		strlwr(sStr);	
		
		if(strcmp(sStr, "mightymax") == 0)
			altBoot = BOOT_MIGHTYMAX;
		if(strcmp(sStr, "chishm") == 0)
			altBoot = BOOT_CHISHM;
	}
	
	wifiMode = WIFI_FIRMWARE;
	
	if(getSetting("General", "WifiMethod", sStr))
	{
		strlwr(sStr);	
		
		if(strcmp(sStr, "firmware") == 0)
			wifiMode = WIFI_FIRMWARE;
		if(strcmp(sStr, "dsorganize") == 0)
			wifiMode = WIFI_DSORGANIZE;
	}	
	
	if(getSetting("General", "Hand", sStr))
	{
		strlwr(sStr);	
		
		if(strcmp(sStr, "left") == 0)
			setHand(HAND_LEFT);
		if(strcmp(sStr, "right") == 0)
			setHand(HAND_RIGHT);
	}
	
	saveFormat = SAVE_BMP;
	if(getSetting("ScribblePad", "SaveFormat", sStr))
	{
		strlwr(sStr);	
		
		if(strcmp(sStr,"png") == 0)
			saveFormat = SAVE_PNG;	
		if(strcmp(sStr,"bmp") == 0)
			saveFormat = SAVE_BMP;
	}
	
	if(getSetting("ScribblePad", "CustomColors", sStr))
	{
		bool doneLoading = false;
		char strColor[64];
		char *tStr = sStr;
		int curLoc = 18;
		
		memset(strColor, 0, 64);
		
		while(!doneLoading && curLoc < 27)
		{
			if(*tStr == 0 || *tStr == ',')
			{
				getCustomColors()[curLoc] = getNumber(strColor);
				memset(strColor, 0, 64);
				
				curLoc++;
				
				if(*tStr == 0)
					doneLoading = true;
			}
			else
			{
				strColor[strlen(strColor)] = *tStr;
			}
			
			tStr++;
		}
	}
	
	if(getSetting("Database", "DefaultPath", defaultSavePath))
	{
		if(strlen(defaultSavePath) == 0)
			strcpy(defaultSavePath, "/");
		else
		{
			int x = strlen(defaultSavePath)-1;
			if(defaultSavePath[x] != '/')
				strcat(defaultSavePath, "/");
		}
	}
	else
		strcpy(defaultSavePath, "/");
	
	if(getSetting("Database", "Proxy", proxy))
	{
		if(strlen(proxy) == 0)
			setProxy(false, NULL);
		else
			setProxy(true, proxy);
	}
	else
	{
		setProxy(false, NULL);
		strcpy(proxy, "");
	}
	
	if(!getSetting("IRC", "Nick", nickName))
		strcpy(nickName,"");
	if(!getSetting("IRC", "AltNick", altnickName))
		strcpy(altnickName,"");
	
	if(!getSetting("IRC", "Server", ircServer))
		strcpy(ircServer,"irc.rizon.net:6667");
	
	autoConnect = getBoolSetting("IRC", "AutoConnect");
	
	if(getSetting("IRC", "Font", sStr))
	{
		strlwr(sStr);	
		
		if(strcmp(sStr, "fixed") == 0)
			ircFixedWidth = true;
		if(strcmp(sStr, "variable") == 0)
			ircFixedWidth = false;
	}
	
	if(getSetting("Editor", "Font", sStr))
	{
		strlwr(sStr);	
		
		if(strcmp(sStr, "fixed") == 0)
			textFixedWidth = true;
		if(strcmp(sStr, "variable") == 0)
			textFixedWidth = false;
	}
	
	if(!getSetting("WebBrowser", "Homepage", homePage))
		strcpy(homePage,"about:blank");
	
	if(getSetting("WebBrowser", "DownloadPath", downloadLocation))
	{
		if(strlen(downloadLocation) == 0)
			strcpy(downloadLocation, "/");
		else
		{
			int x = strlen(downloadLocation)-1;
			if(downloadLocation[x] != '/')
				strcat(downloadLocation, "/");
		}
	}
	else
		strcpy(downloadLocation, "/");
	
	imageLandscape = true;
	if(getSetting("ImageViewer", "Orientation", sStr))
	{
		strlwr(sStr);	
		
		if(strcmp(sStr, "letter") == 0)
			imageLandscape = false;
	}
	
	autoHide = getBoolSetting("WebBrowser", "AutoHide");
	
	// grab the startup screen	
	if(getSetting("Startup", "DefaultScreen", sStr))
	{
		strlwr(sStr);
		
		if(strcmp(sStr,"calendar") == 0)
			startMode = CALENDAR;
		else if(strcmp(sStr,"dayview") == 0)
			startMode = DAYVIEW;
		else if(strcmp(sStr,"address") == 0)
			startMode = ADDRESS;
		else if(strcmp(sStr,"todo") == 0)
			startMode = TODOLIST;
		else if(strcmp(sStr,"scribble") == 0)
			startMode = SCRIBBLEPAD;
		else if(strcmp(sStr,"browser") == 0)
			startMode = BROWSER;
		else if(strcmp(sStr,"calculator") == 0)
			startMode = CALCULATOR;
		else if(strcmp(sStr,"irc") == 0)
			startMode = IRC;
		else if(strcmp(sStr,"webbrowser") == 0)
			startMode = WEBBROWSER;
		else
			startMode = HOME;
	}
	
	// load the rest of the settings
	loadColors();
	loadIconSet();
	loadIRCConfig();
	loadSoundMode();
	loadExternalDLDI();
	loadWifi();
	
	if(disableWrites)
	{
		FAT_DisableWriting();
	}
}

void initStartScreen()
{
	switch(startMode)
	{
		case CALENDAR:
			gotoApplication(0,false);
			break;
		case DAYVIEW:
			gotoApplication(1,false);
			break;
		case ADDRESS:
			gotoApplication(2,false);
			break;
		case TODOLIST:
			gotoApplication(3,false);
			break;
		case SCRIBBLEPAD:
			gotoApplication(4,false);
			break;
		case BROWSER:
			gotoApplication(5,false);
			break;
		case CALCULATOR:
			gotoApplication(6,false);
			break;
		case IRC:
			gotoApplication(7,false);
			break;
		case WEBBROWSER:
			gotoApplication(8,false);
			break;
		default:
			returnHome();
			break;
	}
}

void loadWifi()
{
	DRAGON_chdir("/");
	
	char sStr[256];
	sprintf(sStr, "%swifi.dat", d_base);
	
	dsoProfiles = (WIFI_PROFILE *)safeMalloc(3 * sizeof(WIFI_PROFILE));
	
	if(DRAGON_FileExists(sStr) != FE_FILE)
	{
		memset(dsoProfiles, 0, sizeof(WIFI_PROFILE) * 3);
		return;
	}
	
	DRAGON_FILE *df = DRAGON_fopen(sStr, "r");
	DRAGON_fread(dsoProfiles, 1, sizeof(WIFI_PROFILE) * 3, df);
	DRAGON_fclose(df);
	
	cloneIfNeeded(dsoProfiles);
}

void saveWifi()
{
	DRAGON_chdir("/");
	
	char sStr[256];
	sprintf(sStr, "%swifi.dat", d_base);	
	
	DRAGON_FILE *df = DRAGON_fopen(sStr, "w");
	DRAGON_fwrite(dsoProfiles, 1, sizeof(WIFI_PROFILE) * 3, df);
	DRAGON_fclose(df);
}

WIFI_PROFILE *readWifi()
{
	return &dsoProfiles[0];
}

void makeDefaultSettings()
{	
	DRAGON_chdir(d_base);
	DRAGON_FILE *fFile = DRAGON_fopen("config.ini", "w");
	
	DRAGON_fputs("; Edit this as you please to customize DSOrganize", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; to your liking.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("[Startup]", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs(";DefaultScreen can be one of the following:", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs(";calendar, dayview, home, address, calculator, todo, scribble, irc, webbrowser or browser", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("DefaultScreen=home", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("[Regional]", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; Change this to the filename of the language file you need, sans", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; the extension.  English is built in.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("Language=English", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; Set this to true to display things in 24 hour mode.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("Display24h=false", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; Set this to true to show names in First Last format instead of", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; Last, First format.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("DisplayFirstLast=false", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; Set this to true to display dates with day first.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("ReverseDate=false", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("[Browser]", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; This setting will hide hidden and system files in the browser.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("ShowHidden=true", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; This setting will disable autopatching in the browser.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("DisablePatching=false", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("[ScribblePad]", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; This setting allows choice in scribble pad saving formats.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; Current support allows for bmp or png.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("SaveFormat=png", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("[Todo]", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; Set this to true to have the todo add automatic bullets.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("AutoBullet=false", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("[HomeScreen]", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; This setting allows you to rearrange the home pages.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("Order=calendar,dayview,address,todo,scribble,browser,calculator,irc,webbrowser,hbdb", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; This setting allows you to specify a custom iconlist for the homescreen.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("IconSet=Default", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("[General]", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; This setting enables clicking again to launch in some screens.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("SecondClickAction=true", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; This setting changes the HTML render style.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("HTMLStyle=2", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; Set this to true to reverse the A/B buttons.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("swapAB=false", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; These two settings control the loader assigned to A and Y in the browser.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; Choices currently include: internal, chishm, mightymax", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("NormalLoader=chishm", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("AlternateLoader=mightymax", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; This changes the location of the scrollbars.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("Hand=right", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; This changes where DSOrganize looks for your wifi profile.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("WifiMethod=firmware", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; This enables or disables the click noise on the keyboard.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("KeyboardNoise=false", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("[Database]", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; This setting changes where homebrew is downloaded to.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("DefaultPath=/", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs(";Proxy=127.0.0.1:8080", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("[IRC]", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; These settings change your irc options.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("Nick=", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("AltNick=", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("Server=irc.rizon.net:6667", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("AutoConnect=true", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("Font=variable", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("[WebBrowser]", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; These settings change your web browser options.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("Homepage=about:blank", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; This setting changes where things are downloaded to from the web browser.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("DownloadPath=/", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; This setting changes whether or not the keyboard is hidden after navigating to a page.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("AutoHide=true", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("[ImageViewer]", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; This setting changes the screen orientation of the image viewer.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("Orientation=landscape", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("[Editor]", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; These settings change your text editor options.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("Font=variable", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	
	DRAGON_fclose(fFile);
	
	fFile = DRAGON_fopen("autoperform.txt", "w");	
	
	DRAGON_fputs("/j #dsorganize", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	
	DRAGON_fclose(fFile);
	
	strcpy(lLanguage, "english");
	loadLanguage(lLanguage);
}

void saveSettings()
{
	int x = strlen(defaultSavePath)-1;
	if(defaultSavePath[x] != '/')
		strcat(defaultSavePath, "/");
	
	DRAGON_chdir(d_base);
	DRAGON_FILE *fFile = DRAGON_fopen("config.ini", "w");
	
	DRAGON_fputs("; Edit this as you please to customize DSOrganize", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; to your liking.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("[Startup]", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs(";DefaultScreen can be one of the following:", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs(";calendar, dayview, home, address, calculator, todo, scribble, irc, webbrowser or browser", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("DefaultScreen=", fFile);
	
	switch(startMode)
	{
		case HOME:
			DRAGON_fputs("home", fFile);
			break;
		case CALENDAR:
			DRAGON_fputs("calendar", fFile);
			break;
		case DAYVIEW:
			DRAGON_fputs("dayview", fFile);
			break;
		case ADDRESS:
			DRAGON_fputs("address", fFile);
			break;
		case BROWSER:
			DRAGON_fputs("browser", fFile);
			break;
		case CALCULATOR:
			DRAGON_fputs("calculator", fFile);
			break;
		case TODOLIST:
			DRAGON_fputs("todo", fFile);
			break;
		case SCRIBBLEPAD:
			DRAGON_fputs("scribble", fFile);
			break;
		case IRC:
			DRAGON_fputs("irc", fFile);
			break;
		case WEBBROWSER:
			DRAGON_fputs("webbrowser", fFile);
			break;
	}
	
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("[Regional]", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; Change this to the filename of the language file you need, sans", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; the extension.  English is built in.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("Language=", fFile);
	
	DRAGON_fputs(lLanguage, fFile);
	
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; Set this to true to display things in 24 hour mode.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("Display24h=", fFile);
	
	if(milTime)
		DRAGON_fputs("true", fFile);
	else
		DRAGON_fputs("false", fFile);		
	
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; Set this to true to show names in First Last format instead of", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; Last, First format.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("DisplayFirstLast=", fFile);
	
	if(firstLast)
		DRAGON_fputs("true", fFile);
	else
		DRAGON_fputs("false", fFile);
	
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; Set this to true to display dates with day first.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("ReverseDate=", fFile);
	
	if(reverseDate)
		DRAGON_fputs("true", fFile);
	else
		DRAGON_fputs("false", fFile);	
	
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("[Browser]", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; This setting will hide hidden and system files in the browser.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("ShowHidden=", fFile);
	
	if(showHidden)
		DRAGON_fputs("true", fFile);
	else
		DRAGON_fputs("false", fFile);
	
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; This setting will disable autopatching in the browser.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("DisablePatching=", fFile);
	
	if(disablePatching)
		DRAGON_fputs("true", fFile);
	else
		DRAGON_fputs("false", fFile);
	
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("[ScribblePad]", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; This setting allows choice in scribble pad saving formats.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; Current support allows for bmp or png.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("SaveFormat=", fFile);
	
	switch(saveFormat)
	{
		case SAVE_PNG:
			DRAGON_fputs("png", fFile);
			break;
		case SAVE_BMP:
			DRAGON_fputs("bmp", fFile);
			break;
	}
	
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; This setting saves your color picker options.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("CustomColors=", fFile);
	
	int i;	
	for(i=18;i<27;i++)
	{
		if(i > 18)
			DRAGON_fputc(',', fFile);
		
		char strColor[64];
		
		sprintf(strColor, "%d", getCustomColors()[i]);
		
		DRAGON_fputs(strColor, fFile);
	}
	
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("[Todo]", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; Set this to true to have the todo add automatic bullets.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("AutoBullet=", fFile);
	
	if(autoBullet)
		DRAGON_fputs("true", fFile);
	else
		DRAGON_fputs("false", fFile);	
	
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);	
	DRAGON_fputs("[HomeScreen]", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; This setting allows you to rearrange the home pages.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("Order=", fFile);
	
	// change this later for more items	
	for(x=0;x<10;x++)
	{
		switch(locations[x])
		{
			case 0:
				DRAGON_fputs("calendar", fFile);
				break;
			case 1:
				DRAGON_fputs("dayview", fFile);
				break;
			case 2:
				DRAGON_fputs("address", fFile);
				break;
			case 3:
				DRAGON_fputs("todo", fFile);
				break;
			case 4:
				DRAGON_fputs("scribble", fFile);
				break;
			case 5:
				DRAGON_fputs("browser", fFile);
				break;
			case 6:
				DRAGON_fputs("calculator", fFile);
				break;
			case 7:
				DRAGON_fputs("irc", fFile);
				break;
			case 8:
				DRAGON_fputs("webbrowser", fFile);
				break;
			case 9:				
				DRAGON_fputs("hbdb", fFile);
				break;
		}
		
		if(x < 9) // always one less than the loop
			DRAGON_fputc(',', fFile);
	}
	
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; This setting allows you to specify a custom iconlist for the homescreen.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("IconSet=", fFile);
	
	if(strlen(cIconSet) == 0)
		DRAGON_fputs("Default", fFile);
	else
		DRAGON_fputs(cIconSet, fFile);
	
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("[General]", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; This setting enables clicking again to launch in some screens.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("SecondClickAction=", fFile);
	
	if(secondClickOpens)
		DRAGON_fputs("true", fFile);
	else
		DRAGON_fputs("false", fFile);	
	
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; This setting changes the HTML render style.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("HTMLStyle=", fFile);
	
	DRAGON_fputc('0' + htmlStyle, fFile);
		
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; Set this to true to reverse the A/B buttons.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("swapAB=", fFile);
	
	if(swapAB)
		DRAGON_fputs("true", fFile);
	else
		DRAGON_fputs("false", fFile);	
	
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; These two settings control the loader assigned to A and Y in the browser.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; Choices currently include: internal, chishm, mightymax", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("NormalLoader=", fFile);
	
	switch(normalBoot)
	{
		case BOOT_MIGHTYMAX:
			DRAGON_fputs("mightymax", fFile);
			break;
		case BOOT_CHISHM:
			DRAGON_fputs("chishm", fFile);
			break;
	}
	
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("AlternateLoader=", fFile);
	
	switch(altBoot)
	{
		case BOOT_MIGHTYMAX:
			DRAGON_fputs("mightymax", fFile);
			break;
		case BOOT_CHISHM:
			DRAGON_fputs("chishm", fFile);
			break;
	}
	
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; This setting forces DSOrganize to not write to the card.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("DisableWrites=", fFile);
	
	if(disableWrites)
		DRAGON_fputs("true", fFile);
	else
		DRAGON_fputs("false", fFile);	
	
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; This changes the location of the scrollbars.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("Hand=", fFile);
	
	if(getHand() == HAND_RIGHT)
		DRAGON_fputs("right", fFile);
	else
		DRAGON_fputs("left", fFile);
	
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; This changes where DSOrganize looks for your wifi profile.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("WifiMethod=", fFile);
	
	if(wifiMode == WIFI_FIRMWARE)
		DRAGON_fputs("firmware", fFile);
	else
		DRAGON_fputs("dsorganize", fFile);
	
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);	
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; This enables or disables the click noise on the keyboard.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("KeyboardNoise=", fFile);
	
	if(keyClick)
		DRAGON_fputs("true", fFile);
	else
		DRAGON_fputs("false", fFile);
	
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);	
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("[Database]", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; This setting changes where homebrew is downloaded to.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("DefaultPath=", fFile);
	
	DRAGON_fputs(defaultSavePath, fFile);
	
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	
	if(strlen(proxy) == 0)
		DRAGON_fputs(";Proxy=127.0.0.1:8080", fFile);
	else
	{
		DRAGON_fputs("Proxy=", fFile);
		DRAGON_fputs(proxy, fFile);
	}
	
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("[IRC]", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; These settings change your irc options.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("Nick=", fFile);
	
	DRAGON_fputs(nickName, fFile);	
	
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);	
	DRAGON_fputs("AltNick=", fFile);
	
	DRAGON_fputs(altnickName, fFile);	
	
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("Server=", fFile);
	
	DRAGON_fputs(ircServer, fFile);	
	
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("AutoConnect=", fFile);
	
	if(autoConnect)
		DRAGON_fputs("true", fFile);
	else
		DRAGON_fputs("false", fFile);	
		
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("Font=", fFile);
	
	if(ircFixedWidth)
		DRAGON_fputs("fixed", fFile);
	else
		DRAGON_fputs("variable", fFile);	
	
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("[WebBrowser]", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; These settings change your web browser options.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("Homepage=", fFile);
	
	DRAGON_fputs(homePage, fFile);
	
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; This setting changes where things are downloaded to from the web browser.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("DownloadPath=", fFile);
	
	DRAGON_fputs(downloadLocation, fFile);
	
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; This setting changes whether or not the keyboard is hidden after navigating to a page.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("AutoHide=", fFile);
	
	if(autoHide)
		DRAGON_fputs("true", fFile);
	else
		DRAGON_fputs("false", fFile);	
	
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("[ImageViewer]", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("; This setting changes the screen orientation of the image viewer.", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("Orientation=", fFile);
	
	if(imageLandscape)
		DRAGON_fputs("landscape", fFile);
	else
		DRAGON_fputs("letter", fFile);
	
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("[Editor]", fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);
	DRAGON_fputs("Font=", fFile);
	
	if(textFixedWidth)
		DRAGON_fputs("fixed", fFile);
	else
		DRAGON_fputs("variable", fFile);	
	
	DRAGON_fputc(0x0D, fFile);
	DRAGON_fputc(0x0A, fFile);

	DRAGON_fclose(fFile);
}

void loadColors()
{
	char sStr[256];
	uint16 tColor = 0;
	int tValue = -1;
	
	// try colors.ini inside iconset first
	DRAGON_chdir("/");
	sprintf(sStr, "%s%s/colors.ini", d_icons, cIconSet);
	
	if(DRAGON_FileExists(sStr) != FE_FILE)
	{
		sprintf(sStr, "%scolors.ini", d_base);
		if(DRAGON_FileExists(sStr) != FE_FILE)
		{	
			return;
		}
	}
	
	setIniFile(sStr);
	
	// general
	tColor = getColorSetting("Generic", "Highlight");
	if(tColor)
		genericHighlightColor = tColor;
	tColor = getColorSetting("Generic", "Fill");
	if(tColor)
		genericFillColor = tColor;
	tColor = getColorSetting("Generic", "Text");
	if(tColor)
		genericTextColor = tColor;
	tColor = getColorSetting("Generic", "Progress");
	if(tColor)
		genericProgressColor = tColor;
	
	// widgets
	tColor = getColorSetting("Widget", "Border");
	if(tColor)
		widgetBorderColor = tColor;
	tColor = getColorSetting("Widget", "Fill");
	if(tColor)
		widgetFillColor = tColor;
	tColor = getColorSetting("Widget", "Text");
	if(tColor)
		widgetTextColor = tColor;
	tColor = getColorSetting("Widget", "Highlight");
	if(tColor)
		widgetHighlightColor = tColor;
	
	// scrollbar
	tColor = getColorSetting("Scroll", "Fill");
	if(tColor)
		scrollFillColor = tColor;
	tColor = getColorSetting("Scroll", "Highlight");
	if(tColor)
		scrollHighlightColor = tColor;
	tColor = getColorSetting("Scroll", "Normal");
	if(tColor)
		scrollNormalColor = tColor;
	
	// lists
	tColor = getColorSetting("List", "Border");
	if(tColor)
		listBorderColor = tColor;
	tColor = getColorSetting("List", "Fill");
	if(tColor)
		listFillColor = tColor;
	tColor = getColorSetting("List", "Text");
	if(tColor)
		listTextColor = tColor;
	
	// textboxes
	tColor = getColorSetting("TextBox", "Text");
	if(tColor)
		textEntryTextColor = tColor;
	tColor = getColorSetting("TextBox", "Fill");
	if(tColor)
		textEntryFillColor = tColor;
	tColor = getColorSetting("TextBox", "Highlight");
	if(tColor)
		textEntryHighlightColor = tColor;
	tColor = getColorSetting("TextBox", "Progress");
	if(tColor)
		textEntryProgressColor = tColor;
	
	// text area
	tColor = getColorSetting("TextArea", "Fill");
	if(tColor)
		textAreaFillColor = tColor;
	tColor = getColorSetting("TextArea", "Text");
	if(tColor)
		textAreaTextColor = tColor;
	
	// calendar
	tColor = getColorSetting("Calendar", "Weekend");
	if(tColor)
		calendarWeekendColor = tColor;
	tColor = getColorSetting("Calendar", "Normal");
	if(tColor)
		calendarNormalColor = tColor;
	tColor = getColorSetting("Calendar", "DayPlanner");
	if(tColor)
		calendarHasDayPlannerColor = tColor;
	tColor = getColorSetting("Calendar", "CurrentDate");
	if(tColor)
		calendarCurrentDateColor = tColor;
	tColor = getColorSetting("Calendar", "Reminder");
	if(tColor)
		calencarHasReminderColor = tColor;
	tColor = getColorSetting("Calendar", "DayUnderline");
	if(tColor)
		calendarDayUnderlineColor = tColor;
	
	// day planner
	tColor = getColorSetting("DayPlanner", "Dots");
	if(tColor)
		dayPlannerBorderColor = tColor;
	
	// config
	tColor = getColorSetting("Configuration", "Arrows");
	if(tColor)
		configurationArrowColor = tColor;
		
	// keyboard
	tColor = getColorSetting("Keyboard", "Text");
	if(tColor)
		keyboardTextColor = tColor;
	tColor = getColorSetting("Keyboard", "MEDS");
	if(tColor)
		keyboardMEDSTextColor = tColor;
	tColor = getColorSetting("Keyboard", "Highlight");
	if(tColor)
		keyboardHighlightColor = tColor;
	tColor = getColorSetting("Keyboard", "SpecialHighlight");
	if(tColor)
		keyboardSpecialHighlightColor = tColor;
	tColor = getColorSetting("Keyboard", "Border");
	if(tColor)
		keyboardBorderColor = tColor;
	tColor = getColorSetting("Keyboard", "Fill");
	if(tColor)
		keyboardFillColor = tColor;
	tColor = getColorSetting("Keyboard", "Special");
	if(tColor)
		keyboardSpecialColor = tColor;
	tColor = getColorSetting("Keyboard", "ABCSwap");
	if(tColor)
		keyboardABCSwapColor = tColor;
	
	// picture viewer
	tColor = getColorSetting("Picture", "Select");
	if(tColor)
		pictureSelectBorderColor = tColor;
	
	// browser
	tColor = getColorSetting("Browser", "Hidden");
	if(tColor)
		browserHiddenColor = tColor;
	
	// cursor
	tColor = getColorSetting("Cursor", "Insert");
	if(tColor)
		cursorNormalColor = tColor;
	tColor = getColorSetting("Cursor", "Overwrite");
	if(tColor)
		cursorOverwriteColor = tColor;
	
	// home screen date section
	tColor = getColorSetting("Date", "Text");
	if(tColor)
		homeDateTextColor = tColor;
	tColor = getColorSetting("Date", "Fill");
	if(tColor)
		homeDateFillColor = tColor;
	tColor = getColorSetting("Date", "Border");
	if(tColor)
		homeDateBorderColor = tColor;
	tColor = getColorSetting("Date", "BorderFill");
	if(tColor)
		homeDateBorderFillColor = tColor;
	
	// syntax highlighting
	tColor = getColorSetting("Editor", "Comment");
	if(tColor)
		texteditorComment = tColor;
	tColor = getColorSetting("Editor", "Property");
	if(tColor)
		texteditorProperty = tColor;
	tColor = getColorSetting("Editor", "Section");
	if(tColor)
		texteditorSection = tColor;
	tColor = getColorSetting("Editor", "Setting");
	if(tColor)
		texteditorSetting = tColor;
	
	// home screen
	tColor = getColorSetting("Home", "Highlight");
	if(tColor)
		homeHighlightColor = tColor;
	
	// viewer screen
	tColor = getColorSetting("Viewer", "Bookmark");
	if(tColor)
		viewerBookmarkColor = tColor;
		
	// progress screens
	tColor = getColorSetting("ProgressBar", "Border");
	if(tColor)
		loadingBorderColor = tColor;
	tColor = getColorSetting("ProgressBar", "Fill");
	if(tColor)
		loadingFillColor = tColor;
	tColor = getColorSetting("ProgressBar", "Bar");
	if(tColor)
		loadingBarColor = tColor;
	
	// sound
	tColor = getColorSetting("Sound", "SeekFill");
	if(tColor)
		soundSeekFillColor = tColor;
	tColor = getColorSetting("Sound", "SeekNormal");
	if(tColor)
		soundSeekForeColor = tColor;
	tColor = getColorSetting("Sound", "SeekDisabled");
	if(tColor)
		soundSeekDisabledColor = tColor;
	tColor = getColorSetting("Sound", "MetaText");
	if(tColor)
		soundMetaTextColor = tColor;
	tColor = getColorSetting("Sound", "Hold");
	if(tColor)
		soundHoldColor = tColor;
	tColor = getColorSetting("Sound", "EQMedian");
	if(tColor)
		soundEQMedianColor = tColor;
	tColor = getColorSetting("Sound", "EQData");
	if(tColor)
		soundEQDataColor = tColor;
	tColor = getColorSetting("Sound", "Arrows");
	if(tColor)
		soundArrowColor = tColor;
	
	// calculator
	tColor = getColorSetting("Calculator", "ScreenFill");
	if(tColor)
		calculatorScreenFillColor = tColor;
	tColor = getColorSetting("Calculator", "ScreenText");
	if(tColor)
		calculatorScreenTextColor = tColor;
	tColor = getColorSetting("Calculator", "ScreenBorder");
	if(tColor)
		calculatorScreenBorderColor = tColor;
	tColor = getColorSetting("Calculator", "ButtonText");
	if(tColor)
		calculatorButtonTextColor = tColor;
	tColor = getColorSetting("Calculator", "ButtonBorder");
	if(tColor)
		calculatorButtonBorderColor = tColor;
	tColor = getColorSetting("Calculator", "FillNormal");
	if(tColor)
		calculatorNormalFillColor = tColor;
	tColor = getColorSetting("Calculator", "FillMemory");
	if(tColor)
		calculatorMemoryFillColor = tColor;
	tColor = getColorSetting("Calculator", "FillSpecial");
	if(tColor)
		calculatorSpecialFillColor = tColor;
	tColor = getColorSetting("Calculator", "FillUnary");
	if(tColor)
		calculatorUnaryFillColor = tColor;
	tColor = getColorSetting("Calculator", "FillBinary");
	if(tColor)
		calculatorBinaryFillColor = tColor;
	tColor = getColorSetting("Calculator", "ScreenFadeText");
	if(tColor)
		calculatorScreenFadeTextColor = tColor;
	tColor = getColorSetting("Calculator", "ButtonHighlight");
	if(tColor)
		calculatorButtonHighlightColor = tColor;
	
	// irc
	tColor = getColorSetting("IRC", "TabFill");
	if(tColor)
		ircTabFillColor = tColor;
	tColor = getColorSetting("IRC", "TabBorder");
	if(tColor)
		ircTabBorderColor = tColor;
	tColor = getColorSetting("IRC", "TabText");
	if(tColor)
		ircTabTextColor = tColor;
	tColor = getColorSetting("IRC", "TabHighlight");
	if(tColor)
		ircTabHighlightColor = tColor;
	tColor = getColorSetting("IRC", "TabMore");
	if(tColor)
		ircTabMoreTabsColor = tColor;
	tColor = getColorSetting("IRC", "TabImportant");
	if(tColor)
		ircTabImprtantTextColor = tColor;
	tColor = getColorSetting("IRC", "TabUnimportant");
	if(tColor)
		ircTabUnimportantTextColor = tColor;
	tColor = getColorSetting("IRC", "ScreenFill");
	if(tColor)
		ircScreenFillColor = tColor;
	tColor = getColorSetting("IRC", "ScreenBorder");
	if(tColor)
		ircScreenBorderColor = tColor;
	tColor = getColorSetting("IRC", "ScreenText");
	if(tColor)
		ircScreenTextColor = tColor;
	
	// irc message customizations
	tValue = getNumberSetting("IRC", "Notice");
	if(tValue != -1)
		ircColorNOTICE = tColor;
	tValue = getNumberSetting("IRC", "Action");
	if(tValue != -1)
		ircColorACTION = tColor;
	tValue = getNumberSetting("IRC", "CTCP");
	if(tValue != -1)
		ircColorCTCP = tColor;
	tValue = getNumberSetting("IRC", "Normal");
	if(tValue != -1)
		ircColorNORMAL = tColor;
	tValue = getNumberSetting("IRC", "Server");
	if(tValue != -1)
		ircColorSERVER = tColor;
	tValue = getNumberSetting("IRC", "Internal");
	if(tValue != -1)
		ircColorDSO = tColor;
	tValue = getNumberSetting("IRC", "Nick");
	if(tValue != -1)
		ircColorNICK = tColor;
	tValue = getNumberSetting("IRC", "Topic");
	if(tValue != -1)
		ircColorTOPIC = tColor;
	tValue = getNumberSetting("IRC", "Mode");
	if(tValue != -1)
		ircColorMODE = tColor;
	tValue = getNumberSetting("IRC", "Highlight");
	if(tValue != -1)
		ircColorHIGHLIGHT = tColor;
	
	// patch sprites on the fly to skin properly
	k_caps[5] = keyboardTextColor;
	k_shift[5] = keyboardTextColor;
	k_del[4] = keyboardTextColor;
	k_space[5] = keyboardTextColor;
	k_bsp[5] = keyboardTextColor;
	k_ret[5] = keyboardTextColor;
	k_spl[5] = keyboardTextColor;
}

