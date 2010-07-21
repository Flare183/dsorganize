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
#include "configuration.h"
#include "home.h"
#include "fatwrapper.h"
#include "colors.h"
#include "keyboard.h"
#include "settings.h"
#include "fonts.h"
#include "general.h"
#include "graphics.h"
#include "globals.h"
#include "resources.h"
#include "language.h"
#include "http.h"
#include "settings.h"
#include "controls.h"
#include "tabs.h"
#include "wifi.h"
#include "wifi_shared.h"
#include "sound.h"
#include "soundipc.h"
#include "scribble.h"
#include "irc.h"
#include "help.h"

extern volatile Wifi_MainStruct * WifiData;

static LANG_LIST *aLanguages = NULL;
static ICON_LIST *cIcons = NULL;
static char *descText = NULL;
static int lState = 0;
static int iState = 0;
static int mLanguages = 0;
static int mIcons = 0;
static bool cLoadedPage1 = false;
static bool isCFlipped = false;
static bool isDragging = false;
static int dragItem = -1;
static int lastX = 0;
static int lastY = 0;
static int offsetX = 0;
static int offsetY = 0;
static int lastLoaded = -1;
static int lastLoadedWifi = -1;
static uint16 *sprites[MAXDRAG];
static int configCursor;
static int sState = 0;
static WIFI_PROFILE *wifiInfo = NULL;
static WIFI_PROFILE *wifiSave = NULL;
static int wifiState = WIFI_NORMAL;
static int wifiCursor = 0;
static int wifiEditProfile = -1;
static char wifiInput[64];

extern bool milTime;
extern bool firstLast;
extern bool showHidden;
extern int startMode;
extern u32 htmlStyle;
extern bool reverseDate;
extern char lLanguage[LANG_SIZE];
extern char cIconSet[ICON_SIZE];
extern char defaultSavePath[256];
extern int locations[12];
extern char nickName[NICK_LEN];
extern char altnickName[NICK_LEN];
extern char ircServer[60];
extern bool autoConnect;
extern bool autoBullet;
extern char curLang[60];
extern int normalBoot;
extern int altBoot;
extern int wifiMode;

// prototypes
void createConfiguration();
void drawConfiguration();
void drawTopConfiguration();
void configurationSwitchPages(int x, int y);
void configDecCursor(int x, int y);
void configIncCursor(int x, int y);
void configurationForward(int x, int y);
void configurationBack(int x, int y);
void configurationDrag(int px, int py);
void configurationDrop();
void configureAction(int px, int py);
void configToggleKeyboard(int x, int y);
void leftAction();
void rightAction();
bool shouldBeFlipped();
void createConfigButtons();
void configListCallback(int pos, int x, int y);
void editConfigAction(char c);
void resetScreenFlip();
bool isTextEntryScreen();

void setSprites()
{
	for(int x=0;x<12;x++)
	{	
		switch(locations[x])
		{
			case 0:		
				sprites[x] = reminderfilelarge;
				break;
			case 1:
				sprites[x] = dpfilelarge;
				break;
			case 2:
				sprites[x] = vcfilelarge;
				break;
			case 3:
				sprites[x] = todofilelarge;
				break;
			case 4:
				sprites[x] = scribbledrag;
				break;
			case 5:
				sprites[x] = browserdrag;
				break;
			case 6:
				sprites[x] = calcdrag;
				break;
			case 7:
				sprites[x] = ircdrag;
				break;
			case 8:
				sprites[x] = webbrowserdrag;
				break;
			case 9:
				sprites[x] = hbdbdrag;
			case 10:
			case 11:
				break;
		}
	}
}

void clearHelpMemory()
{
	if(descText)
		trackFree(descText);
	
	descText = NULL;
}

void configurationSetLanguages()
{
	strcpy(lLanguage,aLanguages[lState].langURL);
	strcpy(cIconSet,cIcons[iState].iconURL);
}

void destroyWifiInfo()
{
	if(wifiInfo != NULL)
	{
		free(wifiInfo);
	}
	
	wifiInfo = NULL;
}

void loadWifiInfo()
{	
	// populate firmware settings
	for(int x=0;x<3;x++)
	{
		if(WifiData->wfc_enable[x] != 0)
		{	
			// set up the enabled and wep flags
			wifiInfo[x].enabled = true;
			wifiInfo[x].wepMode = (WifiData->wfc_enable[x] & 0xF);
			
			// grab the ssid
			strncpy(wifiInfo[x].ssid, (const char *)WifiData->wfc_ap[x].ssid, WifiData->wfc_ap[x].ssid_len);
			
			// grab the wepkey
			memcpy(wifiInfo[x].wepKey, (const void *)WifiData->wfc_wepkey[x], 16);
			
			// set up the fixed ip settings
			wifiInfo[x].ip = WifiData->wfc_config[x][0];
			wifiInfo[x].gateway = WifiData->wfc_config[x][1];
			wifiInfo[x].subnet = WifiData->wfc_config[x][2];
			
			// set up dns settings
			wifiInfo[x].primarydns = WifiData->wfc_config[x][3];
			wifiInfo[x].secondarydns = WifiData->wfc_config[x][4];
			
			// now figure out if they have dhcp
			u32 bitsSet = wifiInfo[x].ip | wifiInfo[x].gateway;
			if(bitsSet != 0)
			{
				wifiInfo[x].dhcp = false;				
			}
			else
			{
				wifiInfo[x].dhcp = true;
			}
			
			bitsSet = wifiInfo[x].primarydns | wifiInfo[x].secondarydns;
			if(bitsSet != 0)
			{
				wifiInfo[x].autodns = false;				
			}
			else
			{
				wifiInfo[x].autodns = true;
			}
		}
		else
		{
			// ensure we don't use this profile
			wifiInfo[x].enabled = false;
			wifiInfo[x].dhcp = true;
			wifiInfo[x].autodns = true;
		}
	}
	
	// populate normal settings	
	for(int x=0;x<3;x++)
	{
		memcpy(wifiInfo + 3 + x, readWifi() + x, sizeof(WIFI_PROFILE));
	}
}

void saveWifiInfo(WIFI_PROFILE *tProfile)
{
	int c = 0;
	
	// save back to wifi structs for use
	for(int x=0;x<3;x++)
	{
		if(tProfile[x].enabled)
		{
			// ganked from wifi_arm7 and changed to suit current needs
			WifiData->wfc_enable[c] = 0x80 | (tProfile[x].wepMode & 0x0F);
			WifiData->wfc_ap[c].channel = 0;
			
			int n;
			
			for(n=0;n<6;n++) WifiData->wfc_ap[c].bssid[n] = 0;
			for(n=0;n<16;n++) WifiData->wfc_wepkey[c][n] = tProfile[x].wepKey[n];
			for(n=0;n<32;n++) WifiData->wfc_ap[c].ssid[n] = tProfile[x].ssid[n];
			
			WifiData->wfc_ap[c].ssid_len = strlen(tProfile[x].ssid);
			
			if(tProfile[x].dhcp)
			{
				WifiData->wfc_config[c][0] = 0;
				WifiData->wfc_config[c][1] = 0;
				WifiData->wfc_config[c][2] = 0;
			}
			else
			{
				WifiData->wfc_config[c][0] = tProfile[x].ip;
				WifiData->wfc_config[c][1] = tProfile[x].gateway;
				WifiData->wfc_config[c][2] = tProfile[x].subnet;
			}
			
			if(tProfile[x].autodns)
			{
				WifiData->wfc_config[c][3] = 0;
				WifiData->wfc_config[c][4] = 0;
			}
			else
			{
				WifiData->wfc_config[c][3] = tProfile[x].primarydns;
				WifiData->wfc_config[c][4] = tProfile[x].secondarydns;
			}
			
			c++;
		}
	}
	
	if(c < 3)
	{
		for(int x=c;x<3;x++)
		{
			WifiData->wfc_enable[x] = 0;
		}
	}
}

void cloneIfNeeded(void *wTmp)
{
	// copy into cached firmware the desired settings
	if(wifiMode == WIFI_DSORGANIZE)
	{
		saveWifiInfo((WIFI_PROFILE *)wTmp);
	}
}

void saveToSettings()
{
	int c = 0;
	
	// save back dso structs
	for(int x=0;x<3;x++)
	{
		if(wifiInfo[x + 3].enabled)
		{
			// make sure dhcp/autodns are set right
			if(wifiInfo[3 + x].dhcp)
			{
				wifiInfo[3 + x].ip = 0;
				wifiInfo[3 + x].gateway = 0;
				wifiInfo[3 + x].subnet = 0;
			}
			
			if(wifiInfo[3 + x].autodns)
			{
				wifiInfo[3 + x].primarydns = 0;
				wifiInfo[3 + x].secondarydns = 0;
			}
			
			memcpy(readWifi() + c, wifiInfo + 3 + x, sizeof(WIFI_PROFILE));			
			c++;
		}
	}
	
	if(c < 3)
	{
		for(int x=c;x<3;x++)
		{
			memset(readWifi() + x, 0, sizeof(WIFI_PROFILE));
		}
	}
}

void initWifiInfo()
{
	destroyWifiInfo();
	
	wifiInfo = (WIFI_PROFILE *)safeMalloc(6 * sizeof(WIFI_PROFILE));
	
	// load original firmware settings
	soundIPC->channels = 0;
	SendArm7Command(ARM7COMMAND_LOAD_WIFI, 0);
	
	while(soundIPC->channels != 1)
	{
		vblankWait();
	}
	
	loadWifiInfo();
}

void checkKeyboard()
{
	if(shouldBeFlipped())
	{
		if(!isCFlipped)
		{
			isCFlipped = true;
			lcdSwap();
			
			resetKBCursor();
			addKeyboard(0, 0, true, false, SCREEN_FB, editConfigAction, NULL);
			
			setControlVisible(CONTROL_L, false);
			setControlVisible(CONTROL_R, false);
			
			setControlVisible(CONTROL_BACKWARD, false);
			setControlVisible(CONTROL_FORWARD, false);
			
			char str[128];
			
			sprintf(str,"%c\a\a%s", BUTTON_X, l_swap);			
			int tLen = getStringWidth(str, font_gautami_10) / 2;
			
			newControl(0xF0, 128 - tLen, 0, 128 + tLen, TOPAREA - 1, CT_HITBOX, NULL);
			setControlCallbacks(0xF0, configToggleKeyboard, NULL, NULL);
			setControlHotkey(0xF0, KEY_X, 0);
			
			setControlRepeat(2, 15, 4);
			setControlRepeat(3, 15, 4);
			
			setControlEnabled(CONTROL_HOME, false);
			addHotKey(0xF1, KEY_START, configurationSwitchPages, 0, NULL);
		}
	}
	else
	{
		if(isCFlipped)
		{	
			isCFlipped = false;
			lcdSwap();
			
			deleteControl(CONTROL_KEYBOARD);
			deleteControl(0xF0);
			deleteControl(0xF1);
			
			createConfigButtons();
			
			setControlVisible(CONTROL_L, true);
			setControlVisible(CONTROL_R, true);
			
			setControlVisible(CONTROL_BACKWARD, true);
			setControlVisible(CONTROL_FORWARD, true);
			
			setControlAttribute(2, ATTR_REPEAT, false);
			setControlAttribute(3, ATTR_REPEAT, false);
			
			setControlEnabled(CONTROL_HOME, true);
		}
	}
}

void initConfiguration()
{
	initTabs();
	
	addTab(PAGE1, c_global);
	addTab(PAGE2, c_global);
	addTab(PAGE3, c_home);
	addTab(PAGE4, c_scribble);
	addTab(PAGE5, c_todo);
	addTab(PAGE6, c_editor);
	addTab(PAGE7, c_image);
	addTab(PAGE8, c_browser);
	addTab(PAGE9, c_irc);
	addTab(PAGE10, c_web);
	addTab(PAGE11, c_wifi);
	
	initWifiInfo();
	
	wifiState = WIFI_NORMAL;
	
	lastLoaded = -1;
	configCursor = 0;
	
	cLoadedPage1 = false;
	isCFlipped = false;
	
	createConfiguration();
}

void destroyConfiguration()
{
	resetScreenFlip();
	destroyTabs();
	
	destroyWifiInfo();
	clearHelpMemory();
}

void configMoveCursor(int x, int y)
{
	switch(getControlID())
	{
		case 2: // left button
			leftAction();
			break;
		case 3: // right button
			rightAction();
			break;
		case 4: // up list
			if(getCursor() > 0)
			{
				moveCursorRelative(-1);
			}
			break;
		case 5: // down list
			if(getCursor() < (Wifi_GetNumAP() - 1))
			{
				moveCursorRelative(1);
			}
			break;
		case 6: // left list
			moveCursorRelative(-8);
			break;
		case 7: // right list
			moveCursorRelative(8);
			
			if(getCursor() >= Wifi_GetNumAP())
			{
				moveCursorAbsolute(Wifi_GetNumAP() - 1);
			}
			break;
	}
}

void configToggleKeyboard(int x, int y)
{
	if(isTextEntryScreen())
	{
		toggleKeyboard(x,y);
	}
}

void drawConfigurationScreen()
{
	drawTopConfiguration();
	drawConfiguration();
}

void createConfigButtons()
{
	deleteControl(CONTROL_L);
	deleteControl(CONTROL_R);
	
	deleteControl(CONTROL_FORWARD);
	deleteControl(CONTROL_BACKWARD);
	
	deleteControl(CONTROL_HOME);
	
	if(!isCFlipped)
	{	
		if(wifiState != WIFI_EDITSETTING)
		{
			addDefaultLR(l_prevfield, configDecCursor, l_nextfield, configIncCursor);
		}
		
		if(configCursor >= PAGE10 && wifiState != WIFI_NORMAL)
		{
			addDefaultButtons(l_done, configurationForward, l_cancel, configurationBack);
		}
		else
		{
			addDefaultButtons(l_save, configurationForward, l_back, configurationBack);
		}
	}
	
	if(configCursor >= PAGE10)
	{
		if(wifiState == WIFI_EDITPROFILE)
		{
			setControlEnabled(0xFF, true);
			addHome(l_edit, configurationSwitchPages);
			
			deleteControl(CONTROL_KEYBOARD);
			deleteControl(CONTROL_LIST);
			deleteControl(0xF0);
			
			deleteControl(4);
			deleteControl(5);
			deleteControl(6);
			deleteControl(7);
			
			setControlEnabled(2, true);
			setControlEnabled(3, true);
			
			setControlAttribute(2, ATTR_REPEAT, false);
			setControlAttribute(3, ATTR_REPEAT, false);
			
			return;
		}
		if(wifiState == WIFI_EDITSETTING)
		{
			setControlEnabled(0xFF, false);
			
			if(wifiCursor == -2)
			{
				addListBox(LIST_LEFT, LIST_TOP, LIST_RIGHT, LIST_BOTTOM, Wifi_GetNumAP(), l_nowifi, configListCallback, NULL);
				setControlCallbacks(CONTROL_LIST, configurationForward, NULL, NULL);
				
				setControlEnabled(2, false);
				setControlEnabled(3, false);
				
				// for dpad
				addHotKey(4, KEY_UP, configMoveCursor, KEY_UP, NULL);
				addHotKey(5, KEY_DOWN, configMoveCursor, KEY_DOWN, NULL);
				addHotKey(6, KEY_LEFT, configMoveCursor, KEY_LEFT, NULL);
				addHotKey(7, KEY_RIGHT, configMoveCursor, KEY_RIGHT, NULL);
				
				setControlRepeat(4, 15, 4);
				setControlRepeat(5, 15, 4);
				setControlRepeat(6, 15, 4);
				setControlRepeat(7, 15, 4);
			}
			else
			{
				resetKBCursor();
				addKeyboard(0, 0, true, false, SCREEN_BG, editConfigAction, NULL);
				
				char str[128];
				
				sprintf(str,"%c\a\a%s", BUTTON_X, l_swap);			
				int tLen = getStringWidth(str, font_gautami_10) / 2;
				
				newControl(0xF0, 128 - tLen, 0, 128 + tLen, TOPAREA - 1, CT_HITBOX, NULL);
				setControlCallbacks(0xF0, configToggleKeyboard, NULL, NULL);
				setControlHotkey(0xF0, KEY_X, 0);
				
				setControlRepeat(2, 15, 4);
				setControlRepeat(3, 15, 4);
			}
			
			return;
		}
	}
	
	addHome(l_more, configurationSwitchPages);
}

void createConfiguration()
{
	clearControls();
	
	registerScreenUpdate(drawConfigurationScreen, destroyConfiguration);
	registerHelpScreen("configuration.html", true);
	
	createConfigButtons();
	
	// eaiest way to convert this page is to just pass on the hook to the original touch controls	
	newControl(0xFF, 0, TOPAREA, 255, 160, CT_HITBOX, NULL);
	setControlCallbacks(0xFF, configureAction, configurationDrag, configurationDrop);
	setControlAbsoluteCoords(0xFF, true);
	
	// for dpad
	addHotKey(2, KEY_LEFT, configMoveCursor, KEY_LEFT, NULL);
	addHotKey(3, KEY_RIGHT, configMoveCursor, KEY_RIGHT, NULL);
}

void configurationSetStartMode()
{	
	switch(sState)
	{
		case 0:
			setStartMode(HOME);
			break;
		case 1:
			setStartMode(CALENDAR);
			break;
		case 2:
			setStartMode(DAYVIEW);
			break;
		case 3:
			setStartMode(ADDRESS);
			break;
		case 4:
			setStartMode(TODOLIST);
			break;
		case 5:
			setStartMode(SCRIBBLEPAD);
			break;
		case 6:
			setStartMode(BROWSER);
			break;
		case 7:
			setStartMode(CALCULATOR);
			break;
		case 8:
			setStartMode(IRC);
			break;
		case 9:
			setStartMode(WEBBROWSER);
			break;
	}
}

bool shouldBeFlipped()
{
	if(configCursor == DEFAULTSAVE || configCursor == PROXY || configCursor == NICKNAME || configCursor == ALTNICK || configCursor == IRCSERVER || configCursor == HOMEPAGE || configCursor == DOWNDIR)
		return true;
	
	return false;
}

bool isTextEntryScreen()
{
	if(shouldBeFlipped())
		return true;
	
	if(configCursor == WIFISWITCH && wifiState == WIFI_EDITSETTING && wifiCursor >= 0)
		return true;
	
	return false;
}

void cursorLeaveEvent(int oldCursor)
{
	switch(oldCursor)
	{
		case DEFAULTSAVE:
			if(defaultSavePath[strlen(defaultSavePath)-1] != '/')
			{
				// Ensure trailing slash
				strcat(defaultSavePath, "/");
			}
			
			break;
		case DOWNDIR:
			char *tDown = getDownloadDir();
			
			if(tDown[strlen(tDown)-1] != '/')
			{
				// Ensure trailing slash
				strcat(tDown, "/");
			}
			
			break;
	}
}

void getKeyFromMode(WIFI_PROFILE tWifi, char *tWEP)
{
	memset(tWEP, 0, 33);
	
	switch(tWifi.wepMode)
	{
		case 1: // 5 byte hex WEP
		{
			char tHex[3];
			
			for(int i=0;i<5;i++)
			{
				sprintf(tHex, "%02X", tWifi.wepKey[i]);
				strcat(tWEP, tHex);
			}
			
			break;
		}
		case 2: // 13 byte hex WEP
		{
			char tHex[3];
			
			for(int i=0;i<13;i++)
			{
				sprintf(tHex, "%02X", tWifi.wepKey[i]);
				strcat(tWEP, tHex);
			}
			
			break;
		}
		case 3: // 16 byte hex WEP
		{
			char tHex[3];
			
			for(int i=0;i<16;i++)
			{
				sprintf(tHex, "%02X", tWifi.wepKey[i]);
				strcat(tWEP, tHex);
			}
			
			break;
		}
		case 5: // 5 byte ascii WEP
			memcpy(tWEP, tWifi.wepKey, 5);
			break;
		case 6: // 13 byte ascii WEP
			memcpy(tWEP, tWifi.wepKey, 13);
			break;
		case 7: // 16 byte ascii WEP
			memcpy(tWEP, tWifi.wepKey, 16);
			break;
	}
}

void drawTopConfiguration()
{	
	if(!isCFlipped)
	{
		const char *headerStrings[NUMOPTIONS] = { l_globalsettings, l_globalsettings, l_globalsettings, l_globalsettings, l_globalsettings, l_globalsettings, l_globalsettings, l_globalsettings, l_globalsettings, l_globalsettings, l_homescreensettings, l_scribblesettings, l_todosettings, l_editorsettings, l_imagesettings, l_browsersettings, l_browsersettings, l_browsersettings, l_browsersettings, l_ircsettings, l_ircsettings, l_ircsettings, l_ircsettings, l_ircsettings, l_websettings, l_websettings, l_websettings, l_websettings, l_websettings, l_wifisettings };
		const char *textStrings[NUMOPTIONS] = { "language.txt", "start.txt", "time.txt", "name.txt", "date.txt", "iconset.txt", "secondclick.txt", "swapab.txt", "lefthanded.txt", "keysound.txt", "homescreen.txt", "save.txt", "bullets.txt", "fixededitor.txt", "imageorientation.txt", "showhidden.txt", "", "normalboot.txt", "altboot.txt", "autoconnect.txt", "", "", "", "ircfixed.txt", "htmlmode.txt", "", "", "", "autohide.txt", "wifi.txt" };
		
		bool wifiUpdate = false;
		
		if(configCursor == WIFISWITCH && (wifiState == WIFI_EDITSETTING || wifiState == WIFI_EDITPROFILE))
		{
			int index = wifiCursor + 2;
			
			if(index != lastLoadedWifi)
			{
				wifiUpdate = true;
				lastLoadedWifi = index;
			}
			
			const char *wifiStrings[] = { "ssidsearch.txt", "", "ssid.txt", "wep.txt", "dhcp.txt", "fixedip.txt", "subnet.txt", "gateway.txt", "dns.txt", "primarydns.txt", "secondarydns.txt" };
			textStrings[WIFISWITCH] = wifiStrings[index];
		}
		
		if(!descText)
			descText = (char *)trackMalloc(2048, "temporary help desc");
		
		if(lastLoaded != configCursor || wifiUpdate)		
		{
			if(strlen(textStrings[configCursor]) != 0)
			{
				char tmpLocation[256];
				char tmpLocation2[256];
				
				sprintf(tmpLocation, "%s%s/config/%s", d_help, curLang, textStrings[configCursor]);
				sprintf(tmpLocation2, "%s%s/config/%s", d_help, "english", textStrings[configCursor]);
				
				if(DRAGON_FileExists(tmpLocation) == FE_FILE)
				{
					DRAGON_FILE *fp = DRAGON_fopen(tmpLocation, "r");
					descText[DRAGON_fread(descText, 1, 2048, fp)] = 0;
					DRAGON_fclose(fp);
				}
				else if(DRAGON_FileExists(tmpLocation2) == FE_FILE)
				{
					DRAGON_FILE *fp = DRAGON_fopen(tmpLocation2, "r");
					descText[DRAGON_fread(descText, 1, 2048, fp)] = 0;
					DRAGON_fclose(fp);
				}				
				else
				{
					strcpy(descText, "");
				}
			}
			
			lastLoaded = configCursor;
		}
		
		fb_setDefaultClipping();
		setFont(font_gautami_10);		
		setColor(genericTextColor);
		fb_dispString(3, 3, l_settingdesc);
		
		setColor(textEntryTextColor);		
		fb_drawFilledRect(3,3,252,17,ircScreenBorderColor,ircScreenFillColor);	
		fb_setClipping(7, 7, 250, 16);
		fb_dispString(0,0, headerStrings[configCursor]);
		
		fb_drawFilledRect(3,19,252,188,ircScreenBorderColor,ircScreenFillColor);
		fb_setClipping(7, 23, 250, 188);
		fb_dispString(0,0, descText);
	}
	else
	{
		char str[128];
		
		if(configCursor == DEFAULTSAVE)
			strcpy(str, defaultSavePath);
		if(configCursor == PROXY)
			strcpy(str, getProxy());
		if(configCursor == NICKNAME)
			strcpy(str, nickName);
		if(configCursor == ALTNICK)
			strcpy(str, altnickName);
		if(configCursor == IRCSERVER)
			strcpy(str, ircServer);
		if(configCursor == HOMEPAGE)
			strcpy(str, getHomePage());
		if(configCursor == DOWNDIR)
			strcpy(str, getDownloadDir());
		
		setColor(textEntryTextColor);
		setFont(font_gautami_10);
		fb_drawFilledRect(13, 19, 241, 33, widgetBorderColor, textEntryFillColor);
		fb_setClipping(13, 19, 239, 33);
		
		bool noBlink = false;
		
		if(keysHeld() & KEY_TOUCH)
		{
			touchPosition t = touchReadXYNew();
			
			if(t.px > 13 & t.py > 19 & t.px < 241 & t.py < 33)
			{
				int tCur = getTouchCursor();
				
				if(tCur == -2)
					moveKBCursorAbsolute(strlen(str));
				else if(tCur == -1)
					resetKBCursor();
				else
					moveKBCursorAbsolute(tCur);
				
				noBlink = true;
			}
			
			setCallBack(cursorPosCall);
		}
		
		if(blinkOn() || noBlink)
		{
			if(isInsert())
				setCursorProperties(cursorNormalColor, -2, -3, -1);
			else
				setCursorProperties(cursorOverwriteColor, -2, -3, -1);
				
			showCursor();
			setCursorPos(getKBCursor());		
		}
		
		fb_disableClipping();		
		setFakeHighlight();
		
		fb_dispString(3, 4, str);
		clearCallBack();
		
		fb_enableClipping();
		
		hideCursor();
		clearHighlight();	
		
		fb_drawFilledRect(10, 162, 126, 182, widgetBorderColor, widgetFillColor);	
		fb_drawFilledRect(130, 162, 244, 182, widgetBorderColor, widgetFillColor);
		
		setFont(font_arial_9);
		setColor(widgetTextColor);
		
		fb_setDefaultClipping();
		
		char str1[128];
		char str2[128];
		
		char aButton = BUTTON_A;
		char bButton = BUTTON_B;
		
		if(isABSwapped())
		{
			aButton = BUTTON_B;
			bButton = BUTTON_A;
		}
		
		sprintf(str1, "%c\a%s", aButton, l_save);
		sprintf(str2, "%c\a%s", bButton, l_back);
		
		fb_dispString(centerOnPt(getLLocation(), str1, font_arial_9),165,str1);	
		fb_dispString(centerOnPt(getRLocation(), str2, font_arial_9),165,str2);	
		
		setFont(font_gautami_10);
		setColor(genericTextColor);
		
		sprintf(str,"%c\a\a%s", BUTTON_L, l_prevfield);	
		fb_dispString(3, 3, str);
		sprintf(str,"%s\a\a%c", l_nextfield, BUTTON_R);
		fb_dispString(253-getStringWidth(str, font_gautami_10),3, str);
		
		sprintf(str,"%c\a\a%s", BUTTON_X, l_swap);
		fb_dispString(centerOnPt(getScreenCenter(),str,font_gautami_10), 3, str);
	}
}

void configListCallback(int pos, int x, int y)
{
	char str[33];
	Wifi_AccessPoint ap;
	
	if(Wifi_GetAPData(pos,&ap)==WIFI_RETURN_OK) 
	{
		strcpy(str, ap.ssid);
	}
	else
	{
		strcpy(str, "???");
	}
	
	abbreviateString(str, LIST_WIDTH - 42, font_arial_9);		
	
	bg_dispString(15, 0, str);
	
	if((ap.flags & WFLAG_APDATA_WEP) || (ap.flags & WFLAG_APDATA_WPA))
	{
		bg_dispSprite(x, y + 1, c_locked, 31775);
	}
	else
	{
		bg_dispSprite(x, y + 1, c_unlocked, 31775);
	}
	
	// From 1 to 100, i have no idea where sgstair got the divisor of 0xD0, or why 133 works better than 100...
	// I just compared numbers from the wifi config to my laptop and figured out a scale that worked better than 100,
	// and 133 was the average of a bunch of numbers that I compared.  This should be good enough for most end users...
	int ssidStrength = (ap.rssi*133)/0xD0;
	
	// draw strength
	int sX = ((getHand() == HAND_RIGHT) ? (LIST_RIGHT - (DEFAULT_SCROLL_WIDTH + 2)) : LIST_RIGHT) - 26;
	int sY = y + 11;
	
	for(int i=0;i<5;i++)
	{
		uint16 bgColor = listFillColor;
		
		if(ssidStrength > (i * 20) + 5)
		{
			bgColor = RGB15(0, 0, 31);
		}
		
		bg_drawFilledRect(sX + (i * 5), sY - (2 + (2 * i)), sX + (i * 5) + 3, sY, widgetTextColor, bgColor);
	}
}

void drawSetting(int settingPosition, const char *settingTitle, const char *settingValue, bool highlighted, bool hasArrows, bool italics)
{
	setFont(font_gautami_10);
	setColor(genericTextColor);
	
	bg_dispString(13, settingPosition, settingTitle);
	drawTextBox(13, settingPosition + 8, 242, settingPosition + 23, settingValue, true, highlighted, italics);

	if(hasArrows)
	{
		bg_dispCustomSprite(5, settingPosition + 3 + 8, spr_left, 31775, configurationArrowColor);
		bg_dispCustomSprite(245, settingPosition + 3 + 8, spr_right, 31775, configurationArrowColor);
	}
}

void drawConfiguration()
{	
	char str[256];
	
	if(!(configCursor >= PAGE10 && (wifiState == WIFI_EDITPROFILE || wifiState == WIFI_EDITSETTING)))
	{
		displayTabs();
	}
	
	if(configCursor < PAGE2) // page 1
	{
		char modes[MAX_MODES][60];
		
		strcpy(modes[0], l_home);
		strcpy(modes[1], l_calendar);
		strcpy(modes[2], l_dayview);
		strcpy(modes[3], l_addressbook);
		strcpy(modes[4], l_todo);
		strcpy(modes[5], l_scribble);
		strcpy(modes[6], l_browser);
		strcpy(modes[7], l_calculator);
		strcpy(modes[8], l_irc);
		strcpy(modes[9], l_webbrowser);
		
		if(!cLoadedPage1)
		{
			if(!aLanguages)
				aLanguages = (LANG_LIST *)trackMalloc(LANG_SIZE * 100, "language list");
			
			mLanguages = listLanguages(aLanguages);
			
			lState = 0;
			sState = 0;
			
			for(uint16 i=0;i<mLanguages;i++)
			{
				char str1[60];
				char str2[60];
				
				strcpy(str1, aLanguages[i].langURL);
				strcpy(str2, lLanguage);
				
				strlwr(str1);
				strlwr(str2);
				
				if(strcmp(str1,str2) == 0) // we haev the right language
					lState = i;
			}
			
			if(startMode == HOME)
				sState = 0;
			if(startMode == CALENDAR)
				sState = 1;
			if(startMode == DAYVIEW)
				sState = 2;
			if(startMode == ADDRESS)
				sState = 3;
			if(startMode == TODOLIST)
				sState = 4;			
			if(startMode == SCRIBBLEPAD)
				sState = 5;
			if(startMode == BROWSER)
				sState = 6;
			if(startMode == CALCULATOR)
				sState = 7;
			if(startMode == IRC)
				sState = 8;
			if(startMode == WEBBROWSER)
				sState = 9;
			if(!cIcons)
				cIcons = (ICON_LIST *)trackMalloc(ICON_SIZE * 100, "icon list");
			
			mIcons = listIconSets(cIcons);
			
			for(uint16 i=0;i<mIcons;i++)
			{
				char str1[60];
				char str2[60];
				
				strcpy(str1, cIcons[i].iconURL);
				strcpy(str2, cIconSet);
				
				strlwr(str1);
				strlwr(str2);
				
				if(strcmp(str1,str2) == 0) // we haev the right language
					iState = i;
			}
			
			cLoadedPage1 = true;
		}
		
		// draw the settings
		drawSetting(Y_FIRST, l_language, aLanguages[lState].langURL, configCursor == LANGUAGE, true, false);
		drawSetting(Y_SECOND, l_startscreen, modes[sState], configCursor == HOMESCREEN, true, false);
		drawSetting(Y_FOURTH, l_dateformat, reverseDate ? "DD/MM/YYYY" : "MM/DD/YYYY", configCursor == DATEFORMAT, true, false);
		drawSetting(Y_FIFTH, l_iconset, cIcons[iState].iconURL, configCursor == ICONSET, true, false);
		
		// manually draw these settings because they are different
		setFont(font_gautami_10);
		setColor(genericTextColor);
		
		bg_dispString(13, Y_THIRD, l_timeformat);
		bg_dispString(141, Y_THIRD, l_nameformat);
		
		drawTextBox(13, Y_THIRD + 8, 115, Y_THIRD + 23, milTime ? "24h" : "12h", true, configCursor == TIMEFORMAT, false);
		
		if(firstLast)
		{	
			strcpy(str, l_first);
			strcat(str, " ");
			strcat(str, l_last);
		}
		else
		{
			strcpy(str, l_last);
			strcat(str, ", ");
			strcat(str, l_first);	
		}
		
		drawTextBox(141, Y_THIRD + 8, 242, Y_THIRD + 23, str, true, configCursor == NAMEFORMAT, false);
		
		bg_dispCustomSprite(5, Y_THIRD + 3 + 8, spr_left, 31775, configurationArrowColor);
		bg_dispCustomSprite(118, Y_THIRD + 3 + 8, spr_right, 31775, configurationArrowColor);	
		bg_dispCustomSprite(133, Y_THIRD + 3 + 8, spr_left, 31775, configurationArrowColor);
		bg_dispCustomSprite(245, Y_THIRD + 3 + 8, spr_right, 31775, configurationArrowColor);
	}
	else if(configCursor >= PAGE2 && configCursor < PAGE3) // page 2
	{		
		drawSetting(Y_FIRST, l_secondclick, secondClickAction() ? l_yes : l_no, configCursor == SECONDCLICK, true, false);
		drawSetting(Y_SECOND, l_swapab, isABSwapped() ? l_yes : l_no, configCursor == SWAPAB, true, false);
		drawSetting(Y_THIRD, l_handposition, getHand() == HAND_RIGHT ? l_righthand : l_lefthand, configCursor == LEFTHANDED, true, false);
		drawSetting(Y_FOURTH, l_keyclick, keyClickEnabled() ? l_click : l_silent, configCursor == KEYCLICK, true, false);
	}
	else if(configCursor >= PAGE3 && configCursor < PAGE4) // page 3
	{		
		setSprites(); // set up sprite order
		
		int x = 16;
		int y = Y_OFFSET + DRAG_CLIP_Y + 2;
		
		for(int z=0;z<MAXDRAG;z++)
		{
			if(!(isDragging && dragItem == z))
				bg_dispSprite(x, y, sprites[z], 31775);
			
			x += X_JUMP;
			if(x >= 224)
			{
				x = 16;
				y += Y_JUMP;
			}
		}
		
		if(isDragging && dragItem != -1)
		{
			int tx = lastX - offsetX;
			int ty = lastY - offsetY;
			
			if(tx < 0)
				tx = 0;
			if(ty < 0)
				ty = 0;
			if(tx + 32 > 255)
				tx = 255 - 32;
			if(ty + 32 > 191)
				ty = 191 - 32;
			
			bg_dispSprite(tx, ty, sprites[dragItem], 31775);
		}
		
		setFont(font_arial_9);
		setColor(genericTextColor);
		bg_setClipping(16, Y_OFFSET, 240, Y_OFFSET + DRAG_CLIP_Y);
		bg_dispString(0, 0, l_drag);
	}
	else if(configCursor >= PAGE4 && configCursor < PAGE5) // page 4
	{
		switch(getSaveFormat())
		{
			case SAVE_BMP:
				drawSetting(Y_FIRST, l_saveformat, "bmp", configCursor == SAVEFORMAT, true, false);
				break;
			case SAVE_PNG:
				drawSetting(Y_FIRST, l_saveformat, "png", configCursor == SAVEFORMAT, true, false);
				break;
		}
	}
	else if(configCursor >= PAGE5 && configCursor < PAGE6) // page 5
	{		
		drawSetting(Y_FIRST, l_autobullet, autoBullet ? l_yes : l_no, configCursor == AUTOBULLET, true, false);
	}
	else if(configCursor >= PAGE6 && configCursor < PAGE7) // page 6
	{
		drawSetting(Y_FIRST, l_editorfont, isTextEditorFixed() ? l_fixed : l_variable, configCursor == VIEWERFIXED, true, false);
	}
	else if(configCursor >= PAGE7 && configCursor < PAGE8) // page 7
	{
		drawSetting(Y_FIRST, l_imageorientation, isImageLandscape() ? l_landscape : l_letter, configCursor == IMAGESIDEWAYS, true, false);
	}
	else if(configCursor >= PAGE8 && configCursor < PAGE9) // page 8
	{
		drawSetting(Y_FIRST, l_showhidden, showHidden ? l_yes : l_no, configCursor == SHOWHIDDEN, true, false);
		drawSetting(Y_SECOND, l_defaulthomebrew, defaultSavePath, configCursor == DEFAULTSAVE, false, false);
		
		char *tSetting = NULL;
		switch(normalBoot)
		{
			case BOOT_MIGHTYMAX:
				tSetting = "Mighty Max";
				break;
			case BOOT_CHISHM:
				tSetting = "Chishm";
				break;
		}
		
		drawSetting(Y_THIRD, l_normalboot, tSetting, configCursor == NORMALBOOT, true, false);
		
		switch(altBoot)
		{
			case BOOT_MIGHTYMAX:
				tSetting = "Mighty Max";
				break;
			case BOOT_CHISHM:
				tSetting = "Chishm";
				break;
		}
		
		drawSetting(Y_FOURTH, l_altboot, tSetting, configCursor == ALTBOOT, true, false);
	}
	else if(configCursor >= PAGE9 && configCursor < PAGE10) // page 9
	{
		drawSetting(Y_FIRST, l_autoconnect, autoConnect ? l_yes : l_no, configCursor == AUTOCONNECT, true, false);
		drawSetting(Y_SECOND, l_server, ircServer, configCursor == IRCSERVER, false, false);
		drawSetting(Y_THIRD, l_ircnickname, strlen(nickName) == 0 ? l_none : nickName, configCursor == NICKNAME, false, strlen(nickName) == 0);
		drawSetting(Y_FOURTH, l_altnickname, strlen(altnickName) == 0 ? l_none : altnickName, configCursor == ALTNICK, false, strlen(altnickName) == 0);
		drawSetting(Y_FIFTH, l_ircfont, isIRCFixed() ? l_fixed : l_variable, configCursor == IRCFIXED, true, false);
	}
	else if(configCursor >= PAGE10 && configCursor < PAGE11) // page 10
	{
		char *tSetting = NULL;
		switch(htmlStyle)
		{
			case 0:
				tSetting = l_textonly;
				break;
			case 1:
				tSetting = l_extended;
				break;
			case 2:
				tSetting = l_extendedimages;
				break;
		}
		
		drawSetting(Y_FIRST, l_htmlmode, tSetting, configCursor == HTMLMODE, true, false);
		drawSetting(Y_SECOND, l_proxy, strlen(getProxy()) == 0 ? l_none : getProxy(), configCursor == PROXY, false, strlen(getProxy()) == 0);
		drawSetting(Y_THIRD, l_homepage, getHomePage(), configCursor == HOMEPAGE, false, false);
		
		drawSetting(Y_FOURTH, l_downloaddir, getDownloadDir(), configCursor == DOWNDIR, false, false);
		drawSetting(Y_FIFTH, l_autohide, isAutoHide() ? l_yes : l_no, configCursor == AUTOHIDE, true, false);
	}
	else if(configCursor >= PAGE11) // page 11
	{
		if(wifiState == WIFI_NORMAL)
		{
			drawSetting(Y_FIRST, l_wifimode, wifiMode == WIFI_FIRMWARE ? l_firmware : "DSOrganize", configCursor == WIFISWITCH, true, false);
			
			setColor(genericTextColor);
			
			// draw complex options here
			for(int x=0;x<3;x++)
			{
				int curProfile = x + (3 * wifiMode);
				
				if(wifiInfo[curProfile].enabled)
				{
					char tmpStr[256];
					
					sprintf(tmpStr, "%s\n", wifiInfo[curProfile].ssid);
					
					if(wifiInfo[curProfile].dhcp)
					{
						strcat(tmpStr, "DHCP\n");
					}
					else
					{
						strcat(tmpStr, l_fixedip);
						strcat(tmpStr, "\n");
					}
					
					if(wifiInfo[curProfile].autodns)
					{
						strcat(tmpStr, l_autodns);
					}
					else
					{
						strcat(tmpStr, l_manualdns);
					}
					
					drawRoundedButton(13 + (x * (ROUNDBUTTON_WIDTH + INBETWEEN_WIDTH + 1)), Y_SECOND, 13 + (x * (ROUNDBUTTON_WIDTH + INBETWEEN_WIDTH + 1)) + ROUNDBUTTON_WIDTH, Y_FIFTH, tmpStr, false, false);
					drawRoundedButton(13 + (x * (ROUNDBUTTON_WIDTH + INBETWEEN_WIDTH + 1)), Y_FIFTH + 4, 13 + (x * (ROUNDBUTTON_WIDTH + INBETWEEN_WIDTH + 1)) + ROUNDBUTTON_WIDTH, Y_FIFTH + 23, l_delete, false, false);
				}
				else
				{
					setItalics(true);
					drawRoundedButton(13 + (x * (ROUNDBUTTON_WIDTH + INBETWEEN_WIDTH + 1)), Y_SECOND, 13 + (x * (ROUNDBUTTON_WIDTH + INBETWEEN_WIDTH + 1)) + ROUNDBUTTON_WIDTH, Y_FIFTH + 23, l_none, false, false);
					setItalics(false);
				}
			}
		}
		else if(wifiState == WIFI_EDITPROFILE)
		{
			// draw headings
			setFont(font_gautami_10);
			setColor(genericTextColor);
			
			bg_dispString(13, Y_FIRST - Y_SUBTRACT, "SSID");
			bg_dispString(13, Y_SECOND - Y_SUBTRACT, l_wepkey);
			
			// draw options buttons
			drawRoundedButton(202, Y_FIRST - Y_SUBTRACT + 8, 242, Y_FIRST - Y_SUBTRACT + 23, l_scan, false, false);
			drawRoundedButton(13, Y_THIRD - Y_SUBTRACT + 8, 60, Y_THIRD - Y_SUBTRACT + 23, "DHCP", wifiCursor == 2, wifiInfo[wifiEditProfile].dhcp);
			drawRoundedButton(62, Y_THIRD - Y_SUBTRACT + 8, 120, Y_THIRD - Y_SUBTRACT + 23, l_fixedip, wifiCursor == 2, !wifiInfo[wifiEditProfile].dhcp);
			drawRoundedButton(13, Y_FIFTH - Y_SUBTRACT, 90, Y_FIFTH - Y_SUBTRACT + 15, l_autodns, wifiCursor == 6, wifiInfo[wifiEditProfile].autodns);
			drawRoundedButton(92, Y_FIFTH - Y_SUBTRACT, 195, Y_FIFTH - Y_SUBTRACT + 15, l_manualdns, wifiCursor == 6, !wifiInfo[wifiEditProfile].autodns);
			
			// draw textbox settings
			drawTextBox(13, Y_FIRST - Y_SUBTRACT + 8, 200, Y_FIRST - Y_SUBTRACT + 23, wifiInfo[wifiEditProfile].ssid, true, wifiCursor == 0, false);
			
			if(wifiInfo[wifiEditProfile].wepMode == 0)
			{
				drawTextBox(13, Y_SECOND - Y_SUBTRACT + 8, 242, Y_SECOND - Y_SUBTRACT + 23, l_none, true, wifiCursor == 1, true);
			}
			else
			{
				// format wep key for display
				char tWEP[33];
				getKeyFromMode(wifiInfo[wifiEditProfile], tWEP);
				
				drawTextBox(13, Y_SECOND - Y_SUBTRACT + 8, 242, Y_SECOND - Y_SUBTRACT + 23, tWEP, true, wifiCursor == 1, false);
			}
			
			if(!wifiInfo[wifiEditProfile].dhcp)
			{
				// draw headings
				bg_dispString(129, Y_THIRD - Y_SUBTRACT, l_ip);
				bg_dispString(13, Y_FOURTH - Y_SUBTRACT, l_subnet);
				bg_dispString(129, Y_FOURTH - Y_SUBTRACT, l_gateway);
				
				// display textboxes
				char tIP[16];
				
				longToString(wifiInfo[wifiEditProfile].ip, tIP);
				drawTextBox(129, Y_THIRD - Y_SUBTRACT + 8, 242, Y_THIRD - Y_SUBTRACT + 23, tIP, true, wifiCursor == 3, false);
				
				longToString(wifiInfo[wifiEditProfile].subnet, tIP);
				drawTextBox(13, Y_FOURTH - Y_SUBTRACT + 8, 127, Y_FOURTH - Y_SUBTRACT + 23, tIP, true, wifiCursor == 4, false);
				
				longToString(wifiInfo[wifiEditProfile].gateway, tIP);
				drawTextBox(129, Y_FOURTH - Y_SUBTRACT + 8, 242, Y_FOURTH - Y_SUBTRACT + 23, tIP, true, wifiCursor == 5, false);
			}
			
			if(!wifiInfo[wifiEditProfile].autodns)
			{
				// draw headers
				bg_dispString(13, Y_FIFTH - Y_SUBTRACT + 17, l_primarydns);
				bg_dispString(129, Y_FIFTH - Y_SUBTRACT + 17, l_secondarydns);
				
				// display textboxes
				char tIP[16];
				
				longToString(wifiInfo[wifiEditProfile].primarydns, tIP);
				drawTextBox(13, Y_FIFTH - Y_SUBTRACT + 8 + 17, 127, Y_FIFTH - Y_SUBTRACT + 8 + 32, tIP, true, wifiCursor == 7, false);
				
				longToString(wifiInfo[wifiEditProfile].secondarydns, tIP);
				drawTextBox(129, Y_FIFTH - Y_SUBTRACT + 8 + 17, 242, Y_FIFTH - Y_SUBTRACT + 8 + 32, tIP, true, wifiCursor == 8, false);
			}
		}
		else if(wifiState == WIFI_EDITSETTING)
		{
			if(wifiCursor == -2)
			{
				// scan for ap
				setControlState(CONTROL_LIST, Wifi_GetNumAP());				
			}
			else
			{	
				sprintf(str,"%c\a\a%s", BUTTON_X, l_swap);
				bg_dispString(centerOnPt(getScreenCenter(),str,font_gautami_10), 3, str);
				
				bool noBlink = false;
				
				if(keysHeld() & KEY_TOUCH)
				{
					touchPosition t = touchReadXYNew();
					
					if(t.px > 13 & t.py > 19 & t.px < 241 & t.py < 33)
					{
						int tCur = getTouchCursor();
						
						if(tCur == -2)
							moveKBCursorAbsolute(strlen(wifiInput));
						else if(tCur == -1)
							resetKBCursor();
						else
							moveKBCursorAbsolute(tCur);
						
						noBlink = true;
					}
					
					setCallBack(cursorPosCall);
				}
				
				if(blinkOn() || noBlink)
				{
					if(isInsert())
						setCursorProperties(cursorNormalColor, -2, -3, -1);
					else
						setCursorProperties(cursorOverwriteColor, -2, -3, -1);
						
					showCursor();
					setCursorPos(getKBCursor());		
				}
				
				bg_disableClipping();		
				setFakeHighlight();
				
				drawTextBox(13, 19, 241, 33, wifiInput, false, false, false);
				
				clearCallBack();				
				bg_enableClipping();				
				hideCursor();
				clearHighlight();
			}
		}
	}
	
	setFont(font_gautami_10);
	bg_setDefaultClipping();
	setColor(genericTextColor);	
}

void leftAction()
{
	if(configCursor >= PAGE10 && wifiState != WIFI_NORMAL)
	{
		if(wifiState == WIFI_EDITPROFILE)
		{
			switch(wifiCursor)
			{
				case 2:
					wifiInfo[wifiEditProfile].dhcp = !wifiInfo[wifiEditProfile].dhcp;
					
					if(!wifiInfo[wifiEditProfile].dhcp)
					{
						// if dhcp is off, autodns needs to be off too
						wifiInfo[wifiEditProfile].autodns = false;
					}
					
					return;
				case 6:
					wifiInfo[wifiEditProfile].autodns = !wifiInfo[wifiEditProfile].autodns;
					
					if(!wifiInfo[wifiEditProfile].dhcp)
					{
						// if dhcp is disabled, auto dns needs to be disabled too
						wifiInfo[wifiEditProfile].autodns = false;
					}
					return;
			}
		}
		else if(wifiState == WIFI_EDITSETTING)
		{
			moveKBCursorRelative(CURSOR_BACKWARD);
		}
		
		return;
	}
	
	if(configCursor == LANGUAGE)
	{
		if(lState > 0)
		{
			lState--;
			initLanguage();
			loadLanguage(aLanguages[lState].langURL);
		}
	}
	else if(configCursor == ICONSET)
	{
		if(iState > 0)
			iState--;
	}
	else if(configCursor == HOMESCREEN)
	{
		if(sState > 0)
			sState--;					
	}
	else if(configCursor == TIMEFORMAT)
		milTime = !milTime;
	else if(configCursor == NAMEFORMAT)
		firstLast = !firstLast;
	else if(configCursor == SHOWHIDDEN)
		showHidden = !showHidden;
	else if(configCursor == SAVEFORMAT)
	{
		if(getSaveFormat() == SAVE_PNG)
			setSaveFormat(SAVE_BMP);
		else if(getSaveFormat() == SAVE_BMP)
			setSaveFormat(SAVE_PNG);
	}
	else if(configCursor == HTMLMODE)
	{
		if(htmlStyle > 0)
			htmlStyle--;
	}
	else if(configCursor == SECONDCLICK)
		toggleSecondClickOption();
	else if(configCursor == DATEFORMAT)
		reverseDate = !reverseDate;
	else if(configCursor == DEFAULTSAVE || configCursor == PROXY || configCursor == NICKNAME || configCursor == ALTNICK || configCursor == IRCSERVER || configCursor == DOWNDIR)
	{
		moveKBCursorRelative(CURSOR_BACKWARD);
	}
	else if(configCursor == AUTOCONNECT)
		autoConnect = !autoConnect;
	else if(configCursor == SWAPAB)
		toggleABSwap();
	else if(configCursor == NORMALBOOT)
	{
		normalBoot--;
		if(normalBoot < 1)
			normalBoot = 2;
	}
	else if(configCursor == ALTBOOT)
	{
		altBoot--;
		if(altBoot < 1)
			altBoot = 2;
	}
	else if(configCursor == AUTOBULLET)
		autoBullet = !autoBullet;
	else if(configCursor == LEFTHANDED)
		swapHands();
	else if(configCursor == VIEWERFIXED)
		toggleTextFixed();
	else if(configCursor == IRCFIXED)
		toggleIRCFixed();
	else if(configCursor == WIFISWITCH)
		wifiMode = 1 - wifiMode;
	else if(configCursor == AUTOHIDE)
		toggleAutoHide();
	else if(configCursor == IMAGESIDEWAYS)
		toggleImageLandscape();
	else if(configCursor == KEYCLICK)
		toggleKeyClick();
}

void rightAction()
{
	if(configCursor >= PAGE10 && wifiState != WIFI_NORMAL)
	{
		if(wifiState == WIFI_EDITPROFILE)
		{
			switch(wifiCursor)
			{
				case 2:
					wifiInfo[wifiEditProfile].dhcp = !wifiInfo[wifiEditProfile].dhcp;
					
					if(!wifiInfo[wifiEditProfile].dhcp)
					{
						// if dhcp is off, autodns needs to be off too
						wifiInfo[wifiEditProfile].autodns = false;
					}
					
					return;
				case 6:
					wifiInfo[wifiEditProfile].autodns = !wifiInfo[wifiEditProfile].autodns;
					
					if(!wifiInfo[wifiEditProfile].dhcp)
					{
						// if dhcp is disabled, auto dns needs to be disabled too
						wifiInfo[wifiEditProfile].autodns = false;
					}
					return;
			}
		}
		else if(wifiState == WIFI_EDITSETTING)
		{			
			if(getKBCursor() < (int)strlen(wifiInput))
				moveKBCursorRelative(CURSOR_FORWARD);
		}
		
		return;
	}
	
	if(configCursor == LANGUAGE)
	{
		if(lState < mLanguages-1)
		{
			lState++;
			initLanguage();
			loadLanguage(aLanguages[lState].langURL);
		}
	}
	else if(configCursor == ICONSET)
	{
		if(iState < mIcons-1)
			iState++;
	}
	else if(configCursor == HOMESCREEN)
	{
		if(sState < MAX_STARTUP_SELECTION - 1)
			sState++;					
	}
	else if(configCursor == TIMEFORMAT)
		milTime = !milTime;
	else if(configCursor == NAMEFORMAT)
		firstLast = !firstLast;
	else if(configCursor == SHOWHIDDEN)
		showHidden = !showHidden;
	else if(configCursor == SAVEFORMAT)
	{
		if(getSaveFormat() == SAVE_PNG)
			setSaveFormat(SAVE_BMP);
		else if(getSaveFormat() == SAVE_BMP)
			setSaveFormat(SAVE_PNG);
	}
	else if(configCursor == HTMLMODE)
	{
		if(htmlStyle < 2)
			htmlStyle++;
	}
	else if(configCursor == SECONDCLICK)
		toggleSecondClickOption();
	else if(configCursor == DATEFORMAT)
		reverseDate = !reverseDate;
	else if(configCursor == DEFAULTSAVE)
	{
		if(getKBCursor() < (int)strlen(defaultSavePath))
			moveKBCursorRelative(CURSOR_FORWARD);
	}
	else if(configCursor == PROXY)
	{
		if(getKBCursor() < (int)strlen(getProxy()))
			moveKBCursorRelative(CURSOR_FORWARD);	
	}
	else if(configCursor == AUTOCONNECT)
		autoConnect = !autoConnect;
	else if(configCursor == NICKNAME)
	{
		if(getKBCursor() < (int)strlen(nickName))
			moveKBCursorRelative(CURSOR_FORWARD);
	}	
	else if(configCursor == ALTNICK)
	{
		if(getKBCursor() < (int)strlen(altnickName))
			moveKBCursorRelative(CURSOR_FORWARD);
	}
	else if(configCursor == IRCSERVER)
	{
		if(getKBCursor() < (int)strlen(ircServer))
			moveKBCursorRelative(CURSOR_FORWARD);	
	}
	else if(configCursor == SWAPAB)
		toggleABSwap();
	else if(configCursor == NORMALBOOT)
	{
		normalBoot++;
		if(normalBoot > 2)
			normalBoot = 1;
	}
	else if(configCursor == ALTBOOT)
	{
		altBoot++;
		if(altBoot > 2)
			altBoot = 1;
	}
	else if(configCursor == AUTOBULLET)
		autoBullet = !autoBullet;
	else if(configCursor == LEFTHANDED)
		swapHands();
	else if(configCursor == VIEWERFIXED)
		toggleTextFixed();
	else if(configCursor == IRCFIXED)
		toggleIRCFixed();
	else if(configCursor == WIFISWITCH)
		wifiMode = 1 - wifiMode;
	else if(configCursor == AUTOHIDE)
		toggleAutoHide();
	else if(configCursor == IMAGESIDEWAYS)
		toggleImageLandscape();
	else if(configCursor == KEYCLICK)
		toggleKeyClick();
	else if(configCursor == DOWNDIR)
	{
		if(getKBCursor() < (int)strlen(getDownloadDir()))
			moveKBCursorRelative(CURSOR_FORWARD);
	}
}

void insertCurrentSetting()
{
	memset(wifiInput, 0, 64);	
	resetKBCursor();
	
	switch(wifiCursor)
	{
		case 0: // ssid
			strcpy(wifiInput, wifiInfo[wifiEditProfile].ssid);
			break;
		case 1: // wep key
			getKeyFromMode(wifiInfo[wifiEditProfile], wifiInput);
			break;
		case 3: // fixed ip
			if(wifiInfo[wifiEditProfile].ip == 0)
			{
				return;
			}
			
			longToString(wifiInfo[wifiEditProfile].ip, wifiInput);
			break;
		case 4: // subnet mask
			if(wifiInfo[wifiEditProfile].subnet == 0)
			{
				return;
			}
			
			longToString(wifiInfo[wifiEditProfile].subnet, wifiInput);
			break;
		case 5: // gateway
			if(wifiInfo[wifiEditProfile].gateway == 0)
			{
				return;
			}
			
			longToString(wifiInfo[wifiEditProfile].gateway, wifiInput);
			break;
		case 7: // primary dns
			if(wifiInfo[wifiEditProfile].primarydns == 0)
			{
				return;
			}
			
			longToString(wifiInfo[wifiEditProfile].primarydns, wifiInput);
			break;
		case 8: // secondary dns
			if(wifiInfo[wifiEditProfile].secondarydns == 0)
			{
				return;
			}
			
			longToString(wifiInfo[wifiEditProfile].secondarydns, wifiInput);
			break;
	}
}

char digitFromHex(char hex)
{	
	if(hex >= '0' && hex <= '9')
		return (hex - '0') & 0xF;
	
	if(hex >= 'a' && hex <= 'f')
		return (hex - 'a' + 10) & 0xF;
	
	if(hex >= 'A' && hex <= 'F')
		return (hex - 'A' + 10) & 0xF;
	
	return 0;
}

bool validateSetting()
{
	switch(wifiCursor)
	{
		case 0: // ssid
			memset(wifiInfo[wifiEditProfile].ssid, 0, 33);
			strcpy(wifiInfo[wifiEditProfile].ssid, wifiInput);
			
			// always succeeds
			return true;
		case 1: // wep key
			switch(strlen(wifiInput))
			{
				case 0: // no wep key
					wifiInfo[wifiEditProfile].wepMode = 0;
					
					return true;
				case 5: // 5 byte ascii WEP
					memset(wifiInfo[wifiEditProfile].wepKey, 0, 16);
					memcpy(wifiInfo[wifiEditProfile].wepKey, wifiInput, 5);
					
					wifiInfo[wifiEditProfile].wepMode = 5;
					
					return true;
				case 13: // 13 byte ascii WEP
					memset(wifiInfo[wifiEditProfile].wepKey, 0, 16);
					memcpy(wifiInfo[wifiEditProfile].wepKey, wifiInput, 13);
					
					wifiInfo[wifiEditProfile].wepMode = 6;
					
					return true;
				case 16: // 13 byte ascii WEP
					memset(wifiInfo[wifiEditProfile].wepKey, 0, 16);
					memcpy(wifiInfo[wifiEditProfile].wepKey, wifiInput, 16);
					
					wifiInfo[wifiEditProfile].wepMode = 7;
					
					return true;
				case 10: // 5 byte hex WEP
					memset(wifiInfo[wifiEditProfile].wepKey, 0, 16);
					
					for(int x = 0; x < 5; x++)
					{
						char tChar;
						
						tChar = digitFromHex(wifiInput[x * 2]) << 4;
						tChar |= digitFromHex(wifiInput[(x * 2) + 1]);
						
						wifiInfo[wifiEditProfile].wepKey[x] = tChar;
					}
					
					wifiInfo[wifiEditProfile].wepMode = 1;
					
					return true;
				case 26: // 13 byte hex WEP
					memset(wifiInfo[wifiEditProfile].wepKey, 0, 16);
					
					for(int x = 0; x < 13; x++)
					{
						char tChar;
						
						tChar = digitFromHex(wifiInput[x * 2]) << 4;
						tChar |= digitFromHex(wifiInput[(x * 2) + 1]);
						
						wifiInfo[wifiEditProfile].wepKey[x] = tChar;
					}
					
					wifiInfo[wifiEditProfile].wepMode = 2;
					
					return true;
				case 32: // 13 byte hex WEP
					memset(wifiInfo[wifiEditProfile].wepKey, 0, 16);
					
					for(int x = 0; x < 16; x++)
					{
						char tChar;
						
						tChar = digitFromHex(wifiInput[x * 2]) << 4;
						tChar |= digitFromHex(wifiInput[(x * 2) + 1]);
						
						wifiInfo[wifiEditProfile].wepKey[x] = tChar;
					}
					
					wifiInfo[wifiEditProfile].wepMode = 3;
					
					return true;
			}
			
			// default to failed wep key
			return false;
		case 3: // fixed ip
		{
			if(strlen(wifiInput) == 0)
			{
				wifiInfo[wifiEditProfile].ip = 0;
				return true;
			}
			
			u32 tIP = stringToLong(wifiInput);
			
			if(tIP == 0)
			{
				// invalid ip
				return false;
			}
			
			wifiInfo[wifiEditProfile].ip = tIP;
			
			return true;
		}
		case 4: // subnet mask
		{
			if(strlen(wifiInput) == 0)
			{
				wifiInfo[wifiEditProfile].subnet = 0;
				return true;
			}
			
			u32 tIP = stringToLong(wifiInput);
			
			if(tIP == 0)
			{
				// invalid ip
				return false;
			}
			
			wifiInfo[wifiEditProfile].subnet = tIP;
			
			return true;
		}
		case 5: // gateway
		{
			if(strlen(wifiInput) == 0)
			{
				wifiInfo[wifiEditProfile].gateway = 0;
				return true;
			}
			
			u32 tIP = stringToLong(wifiInput);
			
			if(tIP == 0)
			{
				// invalid ip
				return false;
			}
			
			wifiInfo[wifiEditProfile].gateway = tIP;
			
			return true;
		}
		case 7: // primary dns
		{
			if(strlen(wifiInput) == 0)
			{
				wifiInfo[wifiEditProfile].primarydns = 0;
				return true;
			}
			
			u32 tIP = stringToLong(wifiInput);
			
			if(tIP == 0)
			{
				// invalid ip
				return false;
			}
			
			wifiInfo[wifiEditProfile].primarydns = tIP;
			
			return true;
		}
		case 8: // secondary dns
		{
			if(strlen(wifiInput) == 0)
			{
				wifiInfo[wifiEditProfile].secondarydns = 0;
				return true;
			}
			
			u32 tIP = stringToLong(wifiInput);
			
			if(tIP == 0)
			{
				// invalid ip
				return false;
			}
			
			wifiInfo[wifiEditProfile].secondarydns = tIP;
			
			return true;
		}
	}
	
	return false;
}

void handleWifiClick(int px, int py)
{
	if(wifiState == WIFI_NORMAL)
	{
		// calculate complex options here
		for(int x=0;x<3;x++)
		{
			int curProfile = x + (3 * wifiMode);
			
			if(wifiInfo[curProfile].enabled)
			{
				if((px >= 13 + (x * (ROUNDBUTTON_WIDTH + INBETWEEN_WIDTH + 1))) && (py >= Y_SECOND) && (px <= 13 + (x * (ROUNDBUTTON_WIDTH + INBETWEEN_WIDTH + 1)) + ROUNDBUTTON_WIDTH) && (py <= Y_FIFTH))
				{
					// edit profile
					wifiSave = (WIFI_PROFILE *)safeMalloc(sizeof(WIFI_PROFILE));
					memcpy(wifiSave, &wifiInfo[curProfile], sizeof(WIFI_PROFILE));
					
					wifiEditProfile = curProfile;
					
					wifiState = WIFI_EDITPROFILE;
					wifiCursor = 0;
					
					createConfigButtons();					
					
					return;
				}
				else if((px >= 13 + (x * (ROUNDBUTTON_WIDTH + INBETWEEN_WIDTH + 1))) && (py >= Y_FIFTH + 4) && (px <= 13 + (x * (ROUNDBUTTON_WIDTH + INBETWEEN_WIDTH + 1)) + ROUNDBUTTON_WIDTH) && (py <= Y_FIFTH + 23))
				{
					// delete profile
					
					memset(&wifiInfo[curProfile], 0, sizeof(WIFI_PROFILE));
					wifiInfo[curProfile].enabled = false;
					
					return;
				}
			}
			else
			{
				if((px >= 13 + (x * (ROUNDBUTTON_WIDTH + INBETWEEN_WIDTH + 1))) && (py >= Y_SECOND) && (px <= 13 + (x * (ROUNDBUTTON_WIDTH + INBETWEEN_WIDTH + 1)) + ROUNDBUTTON_WIDTH) && (py <= Y_FIFTH + 23))
				{
					//edit new profile
					wifiSave = (WIFI_PROFILE *)safeMalloc(sizeof(WIFI_PROFILE));
					memcpy(wifiSave, &wifiInfo[curProfile], sizeof(WIFI_PROFILE));
					
					wifiInfo[curProfile].enabled = true;
					wifiInfo[curProfile].dhcp = true;
					wifiInfo[curProfile].autodns = true;
					wifiEditProfile = curProfile;
					
					wifiState = WIFI_EDITPROFILE;
					wifiCursor = 0;
					
					createConfigButtons();
					
					return;
				}
			}
		}
	}
	else if(wifiState == WIFI_EDITPROFILE)
	{
		// wifi profile settings
		int oldCursor = wifiCursor;
		wifiCursor = -1;
		
		if((px >= 13) && (py >= Y_FIRST - Y_SUBTRACT + 8) && (px <= 200) && (py <= Y_FIRST - Y_SUBTRACT + 23))
		{
			wifiCursor = 0;
		}
		else if((px >= 202) && (py >= Y_FIRST - Y_SUBTRACT + 8) && (px <= 242) && (py <= Y_FIRST - Y_SUBTRACT + 23))
		{
			// scan for ap mode
			wifiCursor = -2;
			wifiState = WIFI_EDITSETTING;
			
			resetCursor();
			Wifi_ScanMode();
			createConfigButtons();
			return;
		}
		else if((px >= 13) && (py >= Y_SECOND - Y_SUBTRACT + 8) && (px <= 242) && (py <= Y_SECOND - Y_SUBTRACT + 23))
		{
			wifiCursor = 1;
		}
		else if((px >= 13) && (py >= Y_THIRD - Y_SUBTRACT + 8) && (px <= 60) && (py <= Y_THIRD - Y_SUBTRACT + 23))
		{
			wifiCursor = 2;
			wifiInfo[wifiEditProfile].dhcp = true;
			return;
		}
		else if((px >= 62) && (py >= Y_THIRD - Y_SUBTRACT + 8) && (px <= 120) && (py <= Y_THIRD - Y_SUBTRACT + 23))
		{
			wifiCursor = 2;
			
			// if dhcp is off, autodns needs to be off too
			wifiInfo[wifiEditProfile].dhcp = false;
			wifiInfo[wifiEditProfile].autodns = false;
			return;
		}
		
		if(!wifiInfo[wifiEditProfile].dhcp)
		{
			if((px >= 129) && (py >= Y_THIRD - Y_SUBTRACT + 8) && (px <= 242) && (py <= Y_THIRD - Y_SUBTRACT + 23))
			{
				wifiCursor = 3;
			}
			else if((px >= 13) && (py >= Y_FOURTH - Y_SUBTRACT + 8) && (px <= 127) && (py <= Y_FOURTH - Y_SUBTRACT + 23))
			{
				wifiCursor = 4;
			}
			else if((px >= 129) && (py >= Y_FOURTH - Y_SUBTRACT + 8) && (px <= 242) && (py <= Y_FOURTH - Y_SUBTRACT + 23))
			{
				wifiCursor = 5;
			}
		}
		
		if((px >= 13) && (py >= Y_FIFTH - Y_SUBTRACT) && (px <= 90) && (py <= Y_FIFTH - Y_SUBTRACT + 15))
		{
			wifiCursor = 6;
			
			// we can only do auto-dns if dhcp is enabled
			if(wifiInfo[wifiEditProfile].dhcp == true)
			{
				wifiInfo[wifiEditProfile].autodns = true;
			}
			
			return;
		}
		else if((px >= 92) && (py >= Y_FIFTH - Y_SUBTRACT) && (px <= 195) && (py <= Y_FIFTH - Y_SUBTRACT + 15))
		{
			wifiCursor = 6;
			wifiInfo[wifiEditProfile].autodns = false;
			return;
		}
		
		if(!wifiInfo[wifiEditProfile].autodns)
		{
			if((px >= 13) && (py >= Y_FIFTH - Y_SUBTRACT + 8 + 17) && (px <= 127) && (py <= Y_FIFTH - Y_SUBTRACT + 8 + 32))
			{
				wifiCursor = 7;
			}
			else if((px >= 129) && (py >= Y_FIFTH - Y_SUBTRACT + 8 + 17) && (px <= 242) && (py <= Y_FIFTH - Y_SUBTRACT + 8 + 32))
			{
				wifiCursor = 8;
			}
		}
		
		if(oldCursor == wifiCursor)
		{
			// go into edit mode
			insertCurrentSetting();
			wifiState = WIFI_EDITSETTING;
			createConfigButtons();
		}
		else if(wifiCursor == -1)
		{
			wifiCursor = oldCursor;
		}
	}
}

static bool shouldCheck = false;

bool firstItem(int py)
{
	bool tTest = py >= (Y_FIRST + 8) && py <= (Y_FIRST + 23);
	
	shouldCheck |= tTest;
	
	return tTest;
}

bool secondItem(int py)
{
	bool tTest = py >= (Y_SECOND + 8) && py <= (Y_SECOND + 23);

	shouldCheck |= tTest;
	
	return tTest;
}

bool thirdItem(int py)
{
	bool tTest = py >= (Y_THIRD + 8) && py <= (Y_THIRD + 23);

	shouldCheck |= tTest;
	
	return tTest;
}

bool fourthItem(int py)
{
	bool tTest = py >= (Y_FOURTH + 8) && py <= (Y_FOURTH + 23);

	shouldCheck |= tTest;
	
	return tTest;
}

bool fifthItem(int py)
{
	bool tTest = py >= (Y_FIFTH + 8) && py <= (Y_FIFTH + 23);

	shouldCheck |= tTest;
	
	return tTest;
}

void checkForLR(int px)
{
	if(!shouldCheck)
	{
		return;
	}
	
	if(px < 13)
	{
		leftAction();	
	}
	else if(px > 242)
	{
		rightAction();	
	}
	
	shouldCheck = false;
}

void fireIfChanged(int oldCursor)
{
	if(oldCursor != configCursor)
	{
		cursorLeaveEvent(oldCursor);
		checkKeyboard();
	}
}

void configureAction(int px, int py)
{
	if(isCFlipped)
		return;
	
	if(configCursor >= PAGE10 && wifiState != WIFI_NORMAL)
	{
		handleWifiClick(px, py);
		return;
	}
	
	if(clickTab(px, py))
	{
		cursorLeaveEvent(configCursor);
		configCursor = getActiveTab();
		
		checkKeyboard();
	}
	else
	{
		int oldCursor = configCursor;
		
		if(configCursor < PAGE2) // page 1
		{
			if(firstItem(py))
			{
				configCursor = LANGUAGE;
			}
			else if(secondItem(py))
			{
				configCursor = HOMESCREEN;				
			}
			else if(thirdItem(py))
			{
				if(px < 13)
				{
					configCursor = TIMEFORMAT;
					leftAction();
				}
				if(px >= 13 && px <= 115)
					configCursor = TIMEFORMAT;
				if(px > 115 && px < 128)
				{
					configCursor = TIMEFORMAT;
					rightAction();
				}
				if(px > 128 && px < 141)
				{
					configCursor = NAMEFORMAT;
					leftAction();								
				}
				if(px >= 141 && px <= 242)
					configCursor = NAMEFORMAT;
				if(px > 242)
				{
					configCursor = NAMEFORMAT;
					rightAction();								
				}
				
				fireIfChanged(oldCursor);			
				return;
			}
			else if(fourthItem(py))
			{
				configCursor = DATEFORMAT;
			}
			else if(fifthItem(py))
			{
				configCursor = ICONSET;
			}
		}
		else if(configCursor >= PAGE2 && configCursor < PAGE3) // page 2
		{
			if(firstItem(py))
			{
				configCursor = SECONDCLICK;
			}
			else if(secondItem(py))
			{
				configCursor = SWAPAB;
			}
			else if(thirdItem(py))
			{
				configCursor = LEFTHANDED;	
			}
			else if(fourthItem(py))
			{
				configCursor = KEYCLICK;	
			}	
		}
		else if(configCursor >= PAGE3 && configCursor < PAGE4) // page 3
		{
			int x = 16;
			int y = Y_OFFSET + DRAG_CLIP_Y + 2;
			
			for(int z=0;z<MAXDRAG;z++)
			{
				if(px >= x && px <= x + 32 && py >= y && py <= y + 32)
				{
					dragItem = z;
					isDragging = true;
					lastX = px;
					lastY = py;
					
					offsetX = px - x;
					offsetY = py - y;
					
					return;
				}
				
				x += X_JUMP;
				if(x >= 224)
				{
					x = 16;
					y += Y_JUMP;
				}
			}
			
			return;
		}
		else if(configCursor >= PAGE4 && configCursor < PAGE5) // page 4
		{
			if(firstItem(py))
			{
				configCursor = SAVEFORMAT;
			}
		}
		else if(configCursor >= PAGE5 && configCursor < PAGE6) // page 5
		{
			if(firstItem(py))
			{
				configCursor = AUTOBULLET;
			}	
		}
		else if(configCursor >= PAGE6 && configCursor < PAGE7) // page 6
		{
			if(firstItem(py))
			{
				configCursor = VIEWERFIXED;
			}	
		}
		else if(configCursor >= PAGE7 && configCursor < PAGE8) // page 7
		{
			if(firstItem(py))
			{
				configCursor = IMAGESIDEWAYS;
			}	
		}
		else if(configCursor >= PAGE8 && configCursor < PAGE9) // page 8
		{
			if(firstItem(py))
			{
				configCursor = SHOWHIDDEN;					
			}				
			else if(secondItem(py))
			{
				configCursor = DEFAULTSAVE;
				fireIfChanged(oldCursor);
				return;
			}
			else if(thirdItem(py))
			{
				configCursor = NORMALBOOT;					
			}	
			else if(fourthItem(py))
			{
				configCursor = ALTBOOT;						
			}
		}
		else if(configCursor >= PAGE9 && configCursor < PAGE10) // page 9
		{
			if(firstItem(py))
			{
				configCursor = AUTOCONNECT;					
			}
			else if(secondItem(py))
			{
				configCursor = IRCSERVER;
				fireIfChanged(oldCursor);
				return;
			}	
			else if(thirdItem(py))
			{
				configCursor = NICKNAME;
				fireIfChanged(oldCursor);
				return;
			}
			else if(fourthItem(py))
			{
				configCursor = ALTNICK;
				fireIfChanged(oldCursor);
				return;
			}
			else if(fifthItem(py))
			{
				configCursor = IRCFIXED;							
			}	
		}
		else if(configCursor >= PAGE10 && configCursor < PAGE11) // page 10
		{
			if(firstItem(py))
			{
				configCursor = HTMLMODE;						
			}
			else if(secondItem(py))
			{
				configCursor = PROXY;
				fireIfChanged(oldCursor);
				return;
			}
			else if(thirdItem(py))
			{
				configCursor = HOMEPAGE;
				fireIfChanged(oldCursor);
				return;
			}
			else if(fourthItem(py))
			{
				configCursor = DOWNDIR;
				fireIfChanged(oldCursor);
				return;
			}
			if(fifthItem(py))
			{
				configCursor = AUTOHIDE;						
			}
		}
		else if(configCursor >= PAGE11) // page 11
		{
			if(firstItem(py))
			{
				configCursor = WIFISWITCH;
			}
			else
			{
				if(py >= (Y_SECOND + 8) && py <= (Y_FIFTH + 23))
				{
					handleWifiClick(px, py);
					return;
				}
			}
		}
		
		fireIfChanged(oldCursor);
	}
	
	checkForLR(px);
}

void configurationDrag(int px, int py)
{
	if(!isDragging)
		return;
	
	if(px == 0 || py == 0)
		return;
	
	lastX = px;
	lastY = py;
	
	return;
}

void configurationDrop()
{
	if(!isDragging)
		return;
	
	isDragging = false;
	
	int x = 16;
	int y = Y_OFFSET + DRAG_CLIP_Y + 2;
	int dropItem = -1;
	
	for(int z=0;z<MAXDRAG;z++)
	{
		if(lastX >= x && lastX <= x + 32 && lastY >= y && lastY <= y + 32)
		{
			dropItem = z;
			break;
		}
		
		x += X_JUMP;
		if(x >= 224)
		{
			x = 16;
			y += Y_JUMP;
		}
	}
	
	if(dragItem == -1 || dropItem == -1)
	{
		dragItem = -1;
		return;	
	}
	if(dragItem == dropItem)
	{
		dragItem = -1;
		return;	
	}
	
	if(abs(dragItem - dropItem) == 1)
	{
		int tl = locations[dragItem];
		locations[dragItem] = locations[dropItem];
		locations[dropItem] = tl;
		
		dragItem = -1;
		return;	
	}
	
	if(dragItem < dropItem)
	{
		int tl = locations[dragItem];
		
		for(int x=dragItem;x<dropItem;x++)
		{
			int y = x;
			int z = x+1;
			
			locations[y] = locations[z];
		}
		
		locations[dropItem] = tl;
		
		dragItem = -1;
		return;
	}
	
	if(dragItem > dropItem)
	{
		int tl = locations[dragItem];
		
		for(int x=dragItem;x>dropItem;x--)
		{
			int y = x;
			int z = x-1;
			
			locations[y] = locations[z];
		}
		
		locations[dropItem] = tl;
		
		dragItem = -1;
		return;
	}
	
	dragItem = -1;
}

void configurationSwitchPages(int x, int y)
{
	if(configCursor >= PAGE10 && wifiState != WIFI_NORMAL)
	{
		if(wifiState == WIFI_EDITPROFILE)
		{
			// edit setting		
			insertCurrentSetting();
			wifiState = WIFI_EDITSETTING;			
			createConfigButtons();
		}
		
		return;
	}
	
	cursorLeaveEvent(configCursor);
	
	if(configCursor < PAGE2)
		configCursor = PAGE2;
	else if(configCursor >= PAGE2 && configCursor < PAGE3)
		configCursor = PAGE3;
	else if(configCursor >= PAGE3 && configCursor < PAGE4)
		configCursor = PAGE4;
	else if(configCursor >= PAGE4 && configCursor < PAGE5)
		configCursor = PAGE5;
	else if(configCursor >= PAGE5 && configCursor < PAGE6)
		configCursor = PAGE6;
	else if(configCursor >= PAGE6 && configCursor < PAGE7)
		configCursor = PAGE7;		
	else if(configCursor >= PAGE7 && configCursor < PAGE8)
		configCursor = PAGE8;
	else if(configCursor >= PAGE8 && configCursor < PAGE9)
		configCursor = PAGE9;
	else if(configCursor >= PAGE9 && configCursor < PAGE10)
		configCursor = PAGE10;
	else if(configCursor >= PAGE10 && configCursor < PAGE11)
		configCursor = PAGE11;
	else
		configCursor = PAGE1;
	
	setTab(configCursor);
	checkKeyboard();
}

void resetScreenFlip()
{
	if(isCFlipped)
		lcdSwap();
	
	isCFlipped = false;
	
	// also free memory here
	
	if(aLanguages)
		trackFree(aLanguages);
	
	if(cIcons)
		trackFree(cIcons);
	
	aLanguages = NULL;
	cIcons = NULL;
}

bool configFlipped()
{
	return isCFlipped;
}

void resetProxy()
{
	if(strlen(getProxy()) == 0)
		setProxy(false, NULL);
	else
		setProxy(true, getProxy());
}

void editConfigAction(char c)
{
	if(!isCFlipped && wifiState != WIFI_EDITSETTING) return;
	if(c == 0) return;
	
	setPressedChar(c);
	
	if(c == RET)
		return;
	if(c == CAP)
	{
		toggleCaps();
		return;
	}
	
	// figure out which buffer to go to, how big....
	char *buf = 0; // get rid of stupid warning message
	int size = 0;
	
	switch(configCursor)
	{
		case DEFAULTSAVE:
			buf = defaultSavePath;
			size = 255;
			break;
		case PROXY:
			buf = getProxy();
			size = 255;
			break;
		case NICKNAME:
			if(c == 32) // no spaces in nicks
				return;
			buf = nickName;
			size = NICK_LEN - 1;
			break;
		case ALTNICK:
			if(c == 32) // no spaces in nicks
				return;
			buf = altnickName;
			size = NICK_LEN - 1;
			break;
		case IRCSERVER:
			buf = ircServer;
			size = 59;
			break;
		case HOMEPAGE:
			buf = getHomePage();
			size = 255;
			break;
		case WIFISWITCH:
			buf = wifiInput;
			size = 32;
			break;
		case DOWNDIR:
			buf = getDownloadDir();
			size = 255;
			break;
	}
	
	if(c == CLEAR_KEY)
	{
		memset(buf, 0, size+1);
		resetKBCursor();
		return;
	}	
	
	char *tmpBuffer = (char *)trackMalloc(size + 1,"configuration kbd");
	memcpy(tmpBuffer,buf,size + 1);
	int oldCursor = getKBCursor();
	
	genericAction(tmpBuffer, size, c);
	
	int *pts = NULL;
	int numPts = getWrapPoints(3, 5, tmpBuffer, 13, 19, 239, 33, &pts, font_gautami_10);
	free(pts);
	
	if(numPts == 1)
	{
		memcpy(buf,tmpBuffer,size);
	}
	else
	{
		moveKBCursorAbsolute(oldCursor);
	}
	
	trackFree(tmpBuffer);
}

void configDecCursor(int x, int y)
{
	if(configCursor >= PAGE11 && wifiState != WIFI_NORMAL)
	{
		if(wifiState == WIFI_EDITPROFILE)
		{
			switch(wifiCursor)
			{
				case 6:
					if(wifiInfo[wifiEditProfile].dhcp)
					{
						wifiCursor = 2;
						return;
					}
					break;
			}
			
			if(wifiCursor > 0)
			{
				wifiCursor--;
			}
		}
		
		return;
	}
	
	cursorLeaveEvent(configCursor);
	
	if(configCursor > 0)
	{
		--configCursor;
	}
	else
	{
		configCursor = NUMOPTIONS - 1;
	}
	
	clearSelect();
	checkKeyboard();
	
	setTab(configCursor);
}

void configIncCursor(int x, int y)
{	
	if(configCursor >= PAGE11 && wifiState != WIFI_NORMAL)
	{
		if(wifiState == WIFI_EDITPROFILE)
		{
			switch(wifiCursor)
			{
				case 2: // jump hidden settings
					if(wifiInfo[wifiEditProfile].dhcp)
					{
						wifiCursor = 6;
						return;
					}
					break;
				case 6:
					if(wifiInfo[wifiEditProfile].autodns)
					{
						return;
					}
					break;
			}
			
			if(wifiCursor < 8)
			{
				wifiCursor++;
			}
		}
		
		return;
	}
	
	cursorLeaveEvent(configCursor);
	
	if(configCursor < NUMOPTIONS - 1)
	{
		++configCursor;
	}
	else
	{
		configCursor = 0;
	}
	
	clearSelect();
	checkKeyboard();
	
	setTab(configCursor);
}

void configurationForward(int x, int y)
{
	if(configCursor >= PAGE11 && wifiState != WIFI_NORMAL)
	{
		switch(wifiState)
		{
			case WIFI_EDITPROFILE:
			{
				// make sure they at least have a ssid
				
				if(strlen(wifiInfo[wifiEditProfile].ssid) == 0)
				{
					return;
				}
				
				free(wifiSave);
				
				wifiState = WIFI_NORMAL;
				createConfigButtons();
				return;
			}
			case WIFI_EDITSETTING:
			{
				// validate setting here
				
				if(wifiCursor == -2)
				{
					if(Wifi_GetNumAP() > 0)
					{
						Wifi_AccessPoint ap;
						
						if(Wifi_GetAPData(getCursor(),&ap) == WIFI_RETURN_OK) 
						{
							memset(wifiInfo[wifiEditProfile].ssid, 0, 33);
							strcpy(wifiInfo[wifiEditProfile].ssid, ap.ssid);
						}
						else
						{
							return;
						}
					}
					else
					{
						return;
					}
					
					Wifi_DisableWifi();
					wifiCursor = 0;
					
					createConfigButtons();
				}
				else
				{
					// validate and add back setting
					if(!validateSetting())
					{
						return;
					}
				}
				
				wifiState = WIFI_EDITPROFILE;
				createConfigButtons();
				return;
			}
		}
	}
	
	cursorLeaveEvent(configCursor);
	
	configurationSetStartMode();
	configurationSetLanguages();
	
	// save to firmware, etc
	saveWifiInfo(wifiInfo);
	saveToSettings();
	
	// save firmware wifi
	soundIPC->channels = 0;
	SendArm7Command(ARM7COMMAND_SAVE_WIFI, 0);
	
	while(soundIPC->channels != 1)
	{
		vblankWait();
	}
	
	// save dso wifi
	saveWifi();
	
	cloneIfNeeded(wifiInfo + 3);
	
	saveSettings();
	destroyConfiguration();
	resetProxy();
	returnHome();
}

void configurationBack(int x, int y)
{	
	if(configCursor >= PAGE11 && wifiState != WIFI_NORMAL)
	{
		switch(wifiState)
		{
			case WIFI_EDITPROFILE:
			{
				memcpy(&wifiInfo[wifiEditProfile], wifiSave, sizeof(WIFI_PROFILE));
				free(wifiSave);
				
				wifiState = WIFI_NORMAL;
				createConfigButtons();
				return;
			}
			case WIFI_EDITSETTING:
			{
				if(wifiCursor == -2)
				{
					Wifi_DisableWifi();
					wifiCursor = 0;
				}
				
				wifiState = WIFI_EDITPROFILE;
				createConfigButtons();
				return;
			}
		}
	}

	loadSettings();	
	destroyConfiguration();
	returnHome();	
}

