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
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <libfb/libcommon.h>
#include <libsound/spc.h>
#include "soundplayer.h"
#include "browser.h"
#include "emulated/nsf.h"
#include "fatwrapper.h"
#include "colors.h"
#include "general.h"
#include "keyboard.h"
#include "fonts.h"
#include "globals.h"
#include "settings.h"
#include "lcdfunctions.h"
#include "graphics.h"
#include "sound.h"
#include "filerout.h"
#include "id3.h"
#include "pls.h"
#include "http.h"
#include "language.h"
#include "soundipc.h"
#include "help.h"
#include "controls.h"
#include "errors.h"

#define HEIGHT_ADJUST 10

static bool soundLoaded = false;
static int sampleWidth = 0;
static int soundMode = SOUND_ONESHOT;
static bool hold = false;
static PLS_TYPE curPlaylist;
static int sndMode = TYPE_NORMAL;
static int plsPos = 0;
static bool firstTime = false;
static bool queued = false;
ID3V1_TYPE id3Data;

extern s16 *lBuffer;
extern s16 *rBuffer;
extern char *tmpMeta;

typedef struct
{
	char fileName[256];
	int browserLoc;
} PLAYLIST_LIST;

static PLAYLIST_LIST *history = NULL;
static int h_position = 0;
static int h_count = 0;
static int h_max = 0;

// prototypes
void drawTopSoundScreen();
void drawBottomSoundScreen();
void destroySound();
void drawPlaylistData();
void togglePause(int x, int y);
void volumeUp(int x, int y);
void volumeDown(int x, int y);
void switchSoundMode(int x, int y);
void exitSound(int x, int y);
void seekSound(int xPos, int yPos);

// random playlist history code
void initRandomList()
{
	history = NULL;
	h_position = 0;
	h_count = 0;
	h_max = 0;
}

void createRandomList()
{
	if(history == NULL)
	{
		history = (PLAYLIST_LIST *)trackMalloc(sizeof(PLAYLIST_LIST),"playlist create");
		
		h_count = 1;
		h_max = 1;
		h_position = 0;
	}
}

void destroyRandomList()
{
	if(history != NULL)
	{
		setGlobalError(ERROR_SOUND_FREEPLAYLIST);
		trackFree(history);
		setGlobalError(ERROR_NONE);
	}
	
	history = NULL;
}

bool isValidRandomList()
{
	if(history == NULL)
		return false;
	
	return true;
}

void addToRandomList(char *fName, int browserLoc)
{
	if(soundMode == SOUND_NEXTFILE) // we let oneshot add because sometimes that's the default, and we want it to stay in the history
		return;
	
	if(!isValidRandomList())
	{
		createRandomList();
		strcpy(history[0].fileName, fName);
		history[0].browserLoc = browserLoc;
		return;
	}
	
	h_position++;
	if(h_count == h_position) // we've hit max
	{
		h_count++;
		
		history = (PLAYLIST_LIST *)trackRealloc(history, sizeof(PLAYLIST_LIST) * h_count);
	}
	
	strcpy(history[h_position].fileName, fName);
	history[h_position].browserLoc = browserLoc;
	
	h_max = h_position + 1; // fake end
}

bool nextRandomList(char *fName, int *bLoc)
{
	if(!isValidRandomList())
		return false;
	
	if(h_position == h_count - 1) // there is no next
		return false;
	
	if(h_position == h_max - 1) // there is no next
		return false;
	
	h_position++;
	strcpy(fName, history[h_position].fileName);
	*bLoc = history[h_position].browserLoc;
	return true;
}

bool prevRandomList(char *fName, int *bLoc)
{
	if(!isValidRandomList())
		return false;
	
	if(h_position == 0) // there is no prev
		return false;
	
	h_position--;
	strcpy(fName, history[h_position].fileName);
	*bLoc = history[h_position].browserLoc;
	return true;
}

// end random playlist history code

bool getNextSong()
{
	int fType = UNKNOWN;
	int tmpCur = getCursor();
	
	while(fType != SOUNDFILE)
	{
		tmpCur = nextFile(getFileName(), getBrowserEntries());
		
		if(tmpCur == -1)
			return false;
		
		fType = getFileType(getFileName());
	}
	
	moveCursorAbsolute(tmpCur);
	return true;
}

bool getPrevSong()
{
	int fType = UNKNOWN;
	int tmpCur = getCursor();
	
	while(fType != SOUNDFILE)
	{
		tmpCur = prevFile(getFileName());
		
		if(tmpCur == -1)
			return false;
		
		fType = getFileType(getFileName());
	}
	
	moveCursorAbsolute(tmpCur);
	return true;
}

void getNextSoundInternal(bool toAdd)
{
	if(soundMode == SOUND_ONESHOT)
		return;
	
	destroySound();
	
	switch(soundMode)
	{
		case SOUND_NEXTFILE:
			if(getNextSong()) // if there is a next song
			{
				soundLoaded = false;			
			}
			else
			{
				exitSound(0,0);
			}
			break;
		case SOUND_RANDFILE:
			int tmpCur = 0;
			
			if(nextRandomList(getFileName(), &tmpCur)) // look in playlist first
			{
				moveCursorAbsolute(tmpCur);
				soundLoaded = false;
			}
			else
			{
				int x = randFile(getFileName(), SOUNDFILE, getBrowserEntries());
				
				if(x == -1)
				{
					exitSound(0,0);
					return;
				}
				
				moveCursorAbsolute(x);
				
				if(toAdd)
				{
					addToRandomList(getFileName(), x);
				}
				
				soundLoaded = false;
			}
			
			break;
	}
}

void previousPlaylistField()
{
	if(curPlaylist.numEntries < 2)
		return;
	if(!plsPos)
		return;
	
	if(sndMode == TYPE_PLS)
	{
		queued = true;
		plsPos--;
		closeSound();
	}
}

void nextPlaylistField()
{
	if(curPlaylist.numEntries < 2)
		return;
	if(plsPos == curPlaylist.numEntries - 1)
		return;
	
	if(sndMode == TYPE_PLS)
	{
		queued = true;
		plsPos++;
		closeSound();
	}
}

void getNextSound(int x, int y)
{
	if(hold && getControlID() != CONTROL_R)
		return;
	
	if(sndMode == TYPE_PLS)
	{
		nextPlaylistField();
		return;
	}
	
	getNextSoundInternal(true);
}

void getPrevSound(int x, int y)
{
	if(hold && getControlID() != CONTROL_L)
		return;
	
	if(sndMode == TYPE_PLS)
	{
		previousPlaylistField();
		return;
	}
	
	if(soundMode == SOUND_ONESHOT)
		return;
	
	destroySound();
	
	switch(soundMode)
	{
		case SOUND_NEXTFILE:
			if(getPrevSong()) // if there is a prev song
				soundLoaded = false;
			else
			{
				exitSound(0,0);
			}
			break;
		case SOUND_RANDFILE:
			int tmpCur = 0;
			
			if(prevRandomList(getFileName(), &tmpCur))
			{
				moveCursorAbsolute(tmpCur);
				soundLoaded = false;
			}
			else
			{
				destroyRandomList();
				exitSound(0,0);
			}
			break;
	}
}

void toggleHold(int x, int y)
{
	hold = !hold;
	setControlGrayed(CONTROL_HOME, !hold);
}

void exitSound(int x, int y)
{
	destroySound();
	setMode(BROWSER);
	createBrowser(false);
}

void soundUpdateScreen()
{
	checkEndSound();
	drawTopSoundScreen();
	drawBottomSoundScreen();
}

void setSoundCaptions()
{
	if(sndMode == TYPE_NORMAL)
	{
		switch(soundMode)
		{
			case SOUND_NEXTFILE:
				updateControl(CONTROL_L, l_prevfile);
				updateControl(CONTROL_R, l_nextfile);
				break;
			case SOUND_RANDFILE:
				updateControl(CONTROL_L, l_back);
				updateControl(CONTROL_R, l_randfile);
				break;
			default:
				updateControl(CONTROL_L, NULL);
				updateControl(CONTROL_R, NULL);
				break;
		}
		
		setControlEnabled(CONTROL_L, true);
		setControlEnabled(CONTROL_R, true);
	}
	else
	{
		updateControl(CONTROL_L, l_prevfile);
		updateControl(CONTROL_R, l_nextfile);
		
		if(curPlaylist.numEntries >= 2)
		{
			setControlEnabled(CONTROL_L, true);
			setControlEnabled(CONTROL_R, true);
		}
		else
		{
			setControlEnabled(CONTROL_L, false);
			setControlEnabled(CONTROL_R, false);
		}
	}
	
	switch(getSourceFmt())
	{
		case SRC_STREAM_MP3:
		case SRC_STREAM_OGG:
		case SRC_STREAM_AAC:
			updateControl(CONTROL_FORWARD, NULL);
			break;
		default:		
			if(getState() == STATE_PLAYING)
			{
				updateControl(CONTROL_FORWARD, l_pause);
			}
			else
			{
				updateControl(CONTROL_FORWARD, l_play);
			}
			break;
	}
}

void initSound()
{
	firstTime = true;
	soundLoaded = false;
	
	if(getFileType(getFileName()) == SOUNDFILE)
	{
		addToRandomList(getFileName(), getCursor());
	}
	
	clearControls();
	
	registerScreenUpdate(soundUpdateScreen, NULL);
	registerHelpScreen("sound.html", true);
	
	addHome(l_hold, toggleHold);
	setControlGrayed(CONTROL_HOME, !hold);
	
	addDefaultLR(NULL, getPrevSound, NULL, getNextSound);	
	addHotKey(0xF0, KEY_L, getPrevSound, 0, NULL);
	addHotKey(0xF1, KEY_R, getNextSound, 0, NULL);
	
	addDefaultButtons(l_pause, togglePause, l_back, exitSound);
	
	// for dpad
	addHotKey(0, KEY_UP, volumeUp, 0, NULL);
	addHotKey(1, KEY_DOWN, volumeDown, 0, NULL);
	addHotKey(2, KEY_LEFT, soundPrevTrack, 0, NULL);
	addHotKey(3, KEY_RIGHT, soundNextTrack, 0, NULL);
	
	addHotKey(4, KEY_X, switchSoundMode, 0, NULL);
	
	// for seek bar
	newControl(0xFF, 10, 20, 244, 40, CT_HITBOX, NULL);
	setControlCallbacks(0xFF, seekSound, NULL, NULL);
	setControlAbsoluteCoords(0xFF, true);
}

void checkEndSound()
{
	if(getMode() != SOUNDPLAYER)
	{
		return;
	}
	
	if(!soundLoaded)
	{
		char ext[256];
		char tmp[256];
		
		strcpy(tmp,getFileName());
		separateExtension(tmp,ext);
		strlwr(ext);
		
		if(strcmp(ext,".pls") == 0 || strcmp(ext,".m3u") == 0)
		{	
			sndMode = TYPE_PLS;
			if(loadPlaylist(getFileName(), &curPlaylist))
			{
				DRAGON_chdir("/");
				loadSound(curPlaylist.urlEntry[0].data);
				plsPos = 0;
			}
			else
			{
				destroyRandomList();
				exitSound(0,0);
				return;
			}
		}
		else
		{
			sndMode = TYPE_NORMAL;
			
			bool success = loadSound(getFileName());
			
			if(soundMode == SOUND_ONESHOT || firstTime)
			{
				if(success == false)
				{
					destroyRandomList();
					exitSound(0,0);
					
					return;
				}
			}
			else
			{			
				while(!success)	
				{
					getNextSoundInternal(false);
					
					if(getMode() == BROWSER) // cover for no files found
					{	
						destroyRandomList();
						
						return;
					}
					
					success = loadSound(getFileName());
				}
			}
		}
		
		sampleWidth = (getSoundLength() / 236);
		soundLoaded = true;
	}
	
	firstTime = false;
	
	if(getState() == STATE_STOPPED || getState() == STATE_UNLOADED)
	{
		if(sndMode == TYPE_NORMAL)
		{
			if(soundMode == SOUND_ONESHOT)
			{
				exitSound(0,0);
			}
			else
			{
				getNextSoundInternal(true);
			}
		}
		if(sndMode == TYPE_PLS)
		{
			if(!queued)
			{
				if(plsPos == curPlaylist.numEntries - 1)
					plsPos = 0;
				else
					plsPos++;
			}
			
			DRAGON_chdir("/");
			loadSound(curPlaylist.urlEntry[plsPos].data);
			
			queued = false;
		}		
	}
}

void drawTopSoundScreen()
{
	drawFileInfoScreen();
	fb_setDefaultClipping();
	
	char str[128];;
	
	setColor(genericTextColor);
	sprintf(str, "%c%c %d%c %s", BUTTON_UP, BUTTON_DOWN, (getVolume() * 25), '%', l_volume);
	fb_dispString(5, 160, str);
	
	sprintf(str, "%c %s: ", BUTTON_X, l_soundmode);
	
	switch(soundMode)
	{
		case SOUND_ONESHOT:
			strcat(str, l_oneshot);
			break;
		case SOUND_NEXTFILE:
			strcat(str, l_sequential);
			break;
		case SOUND_RANDFILE:
			strcat(str, l_randfile);
			break;
	}
	
	fb_dispString(5, 175, str);
	
	drawPlaylistData();
}

void switchSoundMode(int x, int y)
{
	soundMode++;
	if(soundMode > 2)
		soundMode = 0;
}

void drawGreyProgress()
{
	bg_drawFilledRect(10, 20, 244, 40, widgetBorderColor, soundSeekDisabledColor);
}

void drawSidMeta()
{
	char str[256];
	
	setFont(font_gautami_10);
	
	setColor(genericTextColor);
	sprintf(str, "%s: ", l_title);
	bg_dispString(10, 50 + HEIGHT_ADJUST, str);	
	setColor(soundMetaTextColor);
	bg_dispString(getLastX(), 50 + HEIGHT_ADJUST, sidMeta(0));	

	setColor(genericTextColor);
	sprintf(str, "%s: ", l_artist);
	bg_dispString(10, 65 + HEIGHT_ADJUST, str);	
	setColor(soundMetaTextColor);
	bg_dispString(getLastX(), 65 + HEIGHT_ADJUST, sidMeta(1));	
	
	setColor(soundMetaTextColor);
	bg_dispString(10, 95 + HEIGHT_ADJUST, "© ");	
	bg_dispString(getLastX(), 95 + HEIGHT_ADJUST, sidMeta(2));
	
	setColor(genericTextColor);
	sprintf(str, "%s: ", l_track);
	bg_dispString(10, 125 + HEIGHT_ADJUST, str);
	
	setColor(soundMetaTextColor);	
	sprintf(str, "%d/%d", getSIDTrack(), getSIDTotalTracks());	
	bg_dispString(getLastX(), 125 + HEIGHT_ADJUST, str);
	
	setColor(genericTextColor);
	sprintf(str, " (%c%c%c%c %s)", BUTTON_LEFT, POS_RIGHT, POS_RIGHT, BUTTON_RIGHT, l_tochange);
	bg_dispString(getLastX(), 125 + HEIGHT_ADJUST, str);
}

void drawGBSMeta()
{
	char str[256];
	
	setFont(font_gautami_10);
	
	setColor(genericTextColor);
	sprintf(str, "%s: ", l_game);
	bg_dispString(10, 50 + HEIGHT_ADJUST, str);	
	setColor(soundMetaTextColor);
	bg_dispString(getLastX(), 50 + HEIGHT_ADJUST, gbsMeta(0));	

	setColor(genericTextColor);
	sprintf(str, "%s: ", l_artist);
	bg_dispString(10, 65 + HEIGHT_ADJUST, str);	
	setColor(soundMetaTextColor);
	bg_dispString(getLastX(), 65 + HEIGHT_ADJUST, gbsMeta(1));	
	
	setColor(soundMetaTextColor);
	bg_dispString(10, 95 + HEIGHT_ADJUST, "© ");	
	bg_dispString(getLastX(), 95 + HEIGHT_ADJUST, gbsMeta(2));
	
	setColor(genericTextColor);
	sprintf(str, "%s: ", l_comment);
	bg_dispString(10, 110 + HEIGHT_ADJUST, str);	
	setColor(soundMetaTextColor);
	bg_dispString(getLastX(), 110 + HEIGHT_ADJUST, gbsMeta(3));	
	
	setColor(genericTextColor);
	sprintf(str, "%s: ", l_track);
	bg_dispString(10, 125 + HEIGHT_ADJUST, str);
	
	setColor(soundMetaTextColor);	
	sprintf(str, "%d/%d", getGBSTrack(), getGBSTotalTracks());	
	bg_dispString(getLastX(), 125 + HEIGHT_ADJUST, str);
	
	setColor(genericTextColor);
	sprintf(str, " (%c%c%c%c %s)", BUTTON_LEFT, POS_RIGHT, POS_RIGHT, BUTTON_RIGHT, l_tochange);
	bg_dispString(getLastX(), 125 + HEIGHT_ADJUST, str);
}

void drawNSFMeta()
{
	char str[256];
	
	setFont(font_gautami_10);
	
	setColor(genericTextColor);
	sprintf(str, "%s: ", l_title);
	bg_dispString(10, 50 + HEIGHT_ADJUST, str);	
	setColor(soundMetaTextColor);
	bg_dispString(getLastX(), 50 + HEIGHT_ADJUST, getNSFMeta(0));	

	setColor(genericTextColor);
	sprintf(str, "%s: ", l_artist);
	bg_dispString(10, 65 + HEIGHT_ADJUST, str);	
	setColor(soundMetaTextColor);
	bg_dispString(getLastX(), 65 + HEIGHT_ADJUST, getNSFMeta(1));	
	
	setColor(soundMetaTextColor);
	bg_dispString(10, 95 + HEIGHT_ADJUST, "© ");	
	bg_dispString(getLastX(), 95 + HEIGHT_ADJUST, getNSFMeta(2));
	
	setColor(genericTextColor);
	sprintf(str, "%s: ", l_track);
	bg_dispString(10, 125 + HEIGHT_ADJUST, str);
	
	setColor(soundMetaTextColor);	
	sprintf(str, "%d/%d", getNSFTrack(), getNSFTotalTracks());	
	bg_dispString(getLastX(), 125 + HEIGHT_ADJUST, str);
	
	setColor(genericTextColor);
	sprintf(str, " (%c%c%c%c %s)", BUTTON_LEFT, POS_RIGHT, POS_RIGHT, BUTTON_RIGHT, l_tochange);
	bg_dispString(getLastX(), 125 + HEIGHT_ADJUST, str);
}

void drawSPCMeta()
{
	char str[256];
	
	setFont(font_gautami_10);
	
	setColor(genericTextColor);
	sprintf(str, "%s: ", l_title);
	bg_dispString(10, 50 + HEIGHT_ADJUST, str);	
	setColor(soundMetaTextColor);
	bg_dispString(getLastX(), 50 + HEIGHT_ADJUST, getSPCMeta(0));	

	setColor(genericTextColor);
	sprintf(str, "%s: ", l_game);
	bg_dispString(10, 65 + HEIGHT_ADJUST, str);	
	setColor(soundMetaTextColor);
	bg_dispString(getLastX(), 65 + HEIGHT_ADJUST, getSPCMeta(1));	
	
	setColor(genericTextColor);
	sprintf(str, "%s: ", l_artist);
	bg_dispString(10, 80 + HEIGHT_ADJUST, str);	
	setColor(soundMetaTextColor);
	bg_dispString(getLastX(), 80 + HEIGHT_ADJUST, getSPCMeta(2));	
	
	setColor(genericTextColor);
	sprintf(str, "%s: ", l_year);
	bg_dispString(10, 95 + HEIGHT_ADJUST, str);	
	setColor(soundMetaTextColor);
	bg_dispString(getLastX(), 95 + HEIGHT_ADJUST, getSPCMeta(3));	
		
	setColor(genericTextColor);
	sprintf(str, "%s: ", l_comment);
	bg_dispString(10, 110 + HEIGHT_ADJUST, str);	
	setColor(soundMetaTextColor);
	bg_dispString(getLastX(), 110 + HEIGHT_ADJUST, getSPCMeta(4));	
}

void drawSNDHMeta()
{
	api68_music_info_t sndhInfo;
	char str[256];
	
	setFont(font_gautami_10);
	
	getSNDHMeta(&sndhInfo);
	
	setColor(genericTextColor);
	sprintf(str, "%s: ", l_title);
	bg_dispString(10, 50 + HEIGHT_ADJUST, str);	
	setColor(soundMetaTextColor);
	bg_dispString(getLastX(), 50 + HEIGHT_ADJUST, sndhInfo.title);	

	setColor(genericTextColor);
	sprintf(str, "%s: ", l_author);
	bg_dispString(10, 65 + HEIGHT_ADJUST, str);	
	setColor(soundMetaTextColor);
	bg_dispString(getLastX(), 65 + HEIGHT_ADJUST, sndhInfo.author);	
	
	setColor(genericTextColor);
	sprintf(str, "%s: ", l_composer);
	bg_dispString(10, 80 + HEIGHT_ADJUST, str);	
	setColor(soundMetaTextColor);
	bg_dispString(getLastX(), 80 + HEIGHT_ADJUST, sndhInfo.composer);	
	
	setColor(genericTextColor);
	sprintf(str, "%s: ", l_converter);
	bg_dispString(10, 95 + HEIGHT_ADJUST, str);	
	setColor(soundMetaTextColor);
	bg_dispString(getLastX(), 95 + HEIGHT_ADJUST, sndhInfo.converter);	
		
	setColor(genericTextColor);
	sprintf(str, "%s: ", l_ripper);
	bg_dispString(10, 110 + HEIGHT_ADJUST, str);	
	setColor(soundMetaTextColor);
	bg_dispString(getLastX(), 110 + HEIGHT_ADJUST, sndhInfo.ripper);
	
	setColor(genericTextColor);
	sprintf(str, "%s: ", l_track);
	bg_dispString(10, 125 + HEIGHT_ADJUST, str);
	
	setColor(soundMetaTextColor);	
	sprintf(str, "%d/%d", getSNDHTrack(), getSNDHTotalTracks());	
	bg_dispString(getLastX(), 125 + HEIGHT_ADJUST, str);
	
	setColor(genericTextColor);
	sprintf(str, " (%c%c%c%c %s)", BUTTON_LEFT, POS_RIGHT, POS_RIGHT, BUTTON_RIGHT, l_tochange);
	bg_dispString(getLastX(), 125 + HEIGHT_ADJUST, str);
}

void drawProgress()
{
	bg_drawFilledRect(10, 20, 244, 40, widgetBorderColor, soundSeekFillColor);
	
	int pixel;
	u32 curSize;
	u32 maxSize;
	getSoundLoc(&curSize, &maxSize);
	
	if(curSize == 0)
	{
		return;
	}
	else
	{
		float percent = (float)(curSize) / (float)maxSize;
		pixel = (int)(232 * percent);
	}
	
	bg_drawRect(11, 21, 11 + pixel, 39, soundSeekForeColor);	
}

void drawEQ()
{
	bg_drawRect(10, 100 - 32, 245, 100 - 32, soundEQMedianColor);
	bg_drawRect(10, 100 + 32, 245, 100 + 32, soundEQMedianColor);
	
	uint16 *bgBuffer = bg_backBuffer();
	
	switch(getSourceFmt())
	{
		case SRC_WAV:
		case SRC_FLAC:
		case SRC_MP3:
		{
			s16 *lTmp = lBuffer;
			s16 *rTmp = rBuffer;
			
			if(getSoundChannels() == 1)
				rTmp = lTmp;
			
			for(int i=0;i<236;i++)
			{
				bgBuffer[10 + i + ((68 - (lTmp[sampleWidth * i] >> 11)) << 8)] = soundEQDataColor;
				bgBuffer[10 + i + ((132 - (rTmp[sampleWidth * i] >> 11)) << 8)] = soundEQDataColor;
			}
			break;
		}
		case SRC_MIKMOD:
		case SRC_AAC:
		{
			if(getSoundChannels() == 1)
			{
				s16 *tmp = lBuffer;
				
				for(int i=0;i<236;i++)
				{
					bgBuffer[10 + i + ((68 - (tmp[sampleWidth * i] >> 11)) << 8)] = soundEQDataColor;
					bgBuffer[10 + i + ((132 - (tmp[sampleWidth * i] >> 11)) << 8)] = soundEQDataColor;
				}
			}
			else
			{
				s16 *lTmp = lBuffer;
				s16 *rTmp = lBuffer + 1;
				
				for(int i=0;i<236;i++)
				{
					bgBuffer[10 + i + ((68 - (lTmp[sampleWidth * i << 1] >> 11)) << 8)] = soundEQDataColor;
					bgBuffer[10 + i + ((132 - (rTmp[sampleWidth * i << 1] >> 11)) << 8)] = soundEQDataColor;
				}
			}
			break;
		}
	}	
}

void drawID3()
{
	if(!(id3Data.present))
	{
		drawEQ();
		return;
	}
	
	char str[256];
	
	setFont(font_gautami_10);
	
	setColor(genericTextColor);
	sprintf(str, "%s: ", l_title);
	bg_dispString(10, 50 + HEIGHT_ADJUST, str);	
	setColor(soundMetaTextColor);
	bg_dispString(getLastX(), 50 + HEIGHT_ADJUST, id3Data.title);	
		
	setColor(genericTextColor);
	sprintf(str, "%s: ", l_artist);
	bg_dispString(10, 65 + HEIGHT_ADJUST, str);	
	setColor(soundMetaTextColor);
	bg_dispString(getLastX(), 65 + HEIGHT_ADJUST, id3Data.artist);	
	
	setColor(genericTextColor);
	sprintf(str, "%s: ", l_album);
	bg_dispString(10, 80 + HEIGHT_ADJUST, str);	
	setColor(soundMetaTextColor);
	bg_dispString(getLastX(), 80 + HEIGHT_ADJUST, id3Data.album);	
	
	setColor(genericTextColor);
	sprintf(str, "%s: ", l_genre);
	bg_dispString(10, 95 + HEIGHT_ADJUST, str);	
	setColor(soundMetaTextColor);
	bg_dispString(getLastX(), 95 + HEIGHT_ADJUST, id3Data.genreStr);	
		
	setColor(genericTextColor);
	sprintf(str, "%s: ", l_comment);
	bg_dispString(10, 110 + HEIGHT_ADJUST, str);	
	setColor(soundMetaTextColor);
	bg_dispString(getLastX(), 110 + HEIGHT_ADJUST, id3Data.comment);	
	
	setColor(genericTextColor);
	sprintf(str, "%s: ", l_year);
	bg_dispString(10, 125 + HEIGHT_ADJUST, str);	
	setColor(soundMetaTextColor);
	bg_dispString(getLastX(), 125 + HEIGHT_ADJUST, id3Data.year);
	
	setColor(genericTextColor);
	sprintf(str, "%s: ", l_track);
	bg_dispString(128, 125 + HEIGHT_ADJUST, str);	
	setColor(soundMetaTextColor);
	
	if(id3Data.track != 0)
	{
		sprintf(str, "%d", id3Data.track);
		bg_dispString(getLastX(), 125 + HEIGHT_ADJUST, str);	
	}
}

void displayStreamData(ICY_HEADER *streamData)
{
	char str[256];
	
	setFont(font_gautami_10);
	
	setColor(genericTextColor);
	sprintf(str, "%s: ", l_title);
	bg_dispString(10, 35 + HEIGHT_ADJUST, str);	
	setColor(soundMetaTextColor);
	bg_dispString(getLastX(), 35 + HEIGHT_ADJUST, streamData->icyName);	
		
	setColor(genericTextColor);
	sprintf(str, "%s: ", l_genre);
	bg_dispString(10, 85 + HEIGHT_ADJUST, str);	
	setColor(soundMetaTextColor);
	bg_dispString(getLastX(), 85 + HEIGHT_ADJUST, streamData->icyGenre);	
	
	setColor(genericTextColor);
	sprintf(str, "URL: ");
	bg_dispString(10, 100 + HEIGHT_ADJUST, str);	
	setColor(soundMetaTextColor);
	bg_dispString(getLastX(), 100 + HEIGHT_ADJUST, streamData->icyURL);
	
	if(strlen(streamData->icyCurSong) > 0)
	{	
		setColor(genericTextColor);
		sprintf(str, "%s: ", l_curSong);
		bg_dispString(10, 115 + HEIGHT_ADJUST, str);	
		setColor(soundMetaTextColor);
		bg_dispString(getLastX(), 115 + HEIGHT_ADJUST, streamData->icyCurSong);
	}
	
	setColor(genericTextColor);
	sprintf(str, "%s: ", l_buffer);
	bg_dispString(10, 140 + HEIGHT_ADJUST, str);	
	setColor(soundMetaTextColor);
	
	int x = getStreamLead() - getStreamLag();
	if(x < 0)
		x += STREAM_BUFFER_SIZE;

	sprintf(str, "%d%c", (int)(((double)x / (double)STREAM_CACHE_SIZE) * 100), '%');
	bg_dispString(getLastX(), 140 + HEIGHT_ADJUST, str);
}

void drawPlaylistData()
{
	if(sndMode == TYPE_NORMAL)
	{
		fb_drawRect(5, 85, 250, 151, genericFillColor);
		return;
	}
	
	if(!curPlaylist.numEntries)
	{
		fb_drawRect(5, 85, 250, 151, genericFillColor);
		return;
	}
	
	int z;
	int maxZ;
	char str[256];
	
	setColor(0);
	fb_drawFilledRect(5, 85, 250, 151, listBorderColor, listFillColor);
	
	if(curPlaylist.numEntries <= 4)
	{
		z = 0;
		maxZ = curPlaylist.numEntries;
	}
	else
	{
		z = plsPos - 1;
		maxZ = plsPos + 3;
		
		if(z < 0)
		{
			z++;
			maxZ++;
		}
		
		while(maxZ > curPlaylist.numEntries)
		{
			z--;
			maxZ--;
		}
	}
	
	fb_setDefaultClipping();
	setFont(font_gautami_10);
	
	fb_drawRect(240, 80, 250, 84, genericFillColor);
	fb_drawRect(240, 153, 250, 157, genericFillColor);	
	
	if(z > 0)
		fb_dispCustomSprite(240, 80, up_arrow, 31775, soundArrowColor);
	if(maxZ < curPlaylist.numEntries)
		fb_dispCustomSprite(240, 153, down_arrow, 31775, soundArrowColor);
	
	setColor(listTextColor);
	setFont(font_arial_9);
	
	for(int i=z;i<maxZ;i++)
	{
		strcpy(str, curPlaylist.descriptionEntry[i].data);			
		abbreviateString(str, 240, font_arial_9);
		
		if(i == plsPos)
			fb_drawRect(7, 87 + (i-z)*16, 248, 101 + (i-z)*16, genericHighlightColor);
		
		fb_dispString(9, (i-z)*16 + 87, str);
	}
}

void drawCurrentStatus()
{
	setColor(genericTextColor);
	setFont(font_arial_9);
	
	char str[256];
	
	sprintf(str, "%s:", l_status);
	bg_dispString(10,20, str);
	setBold(true);
	
	switch(getCurrentStatus())
	{
		case STREAM_DISCONNECTED:
			bg_dispString(50,20, l_notconnected);
			break;
		case STREAM_CONNECTING:
			bg_dispString(50,20, l_associating);
			break;
		case STREAM_CONNECTED:
		case STREAM_STARTING:
			bg_dispString(50,20, l_connecting);
			break;
		case STREAM_FAILEDCONNECT:
			bg_dispString(50,20, l_errorconnecting);
			break;
		case STREAM_STREAMING:
			bg_dispString(50,20, l_streaming);
			setBold(false);
			displayStreamData(getStreamData());
			break;
		case STREAM_BUFFERING:
			bg_dispString(50,20, l_buffering);
			setBold(false);
			displayStreamData(getStreamData());
			break;
	}
	
	setBold(false);
}

void drawBottomSoundScreen()
{	
	if(getMode() == BROWSER)
		return;
	
	bg_setDefaultClipping();
	
	switch(getSourceFmt())
	{
		case SRC_WAV:
		case SRC_MIKMOD:
		case SRC_AAC:
		case SRC_FLAC:
			drawEQ();
			drawProgress();
			break;
		case SRC_MP3:
		case SRC_OGG:
			drawID3();
			drawProgress();
			break;
		case SRC_STREAM_MP3:
		case SRC_STREAM_OGG:
		case SRC_STREAM_AAC:
			drawCurrentStatus();
			break;
		case SRC_SID:
			drawGreyProgress();
			drawSidMeta();
			break;
		case SRC_NSF:
			drawGreyProgress();
			drawNSFMeta();
			break;
		case SRC_SPC:
			drawGreyProgress();
			drawSPCMeta();
			break;
		case SRC_SNDH:
			drawGreyProgress();
			drawSNDHMeta();
			break;
		case SRC_GBS:
			drawGreyProgress();
			drawGBSMeta();
			break;
		default:
			drawProgress();
			break;
	}
	
	setSoundCaptions();
}

void togglePause(int x, int y)
{
	switch(getState())
	{
		case STATE_PLAYING:
			pauseSound(true);
			break;
		case STATE_PAUSED:
			pauseSound(false);
			break;
	}
}

void destroySound()
{
	if(soundMode != SOUND_RANDFILE) // we added earlier assuming that it was going to be random, well, it's not.
		destroyRandomList();	
	
	closeSound();
	
	destroyPlaylist(&curPlaylist);
	
	saveSoundMode();
}

void seekSound(int xPos, int yPos)
{
	if(xPos < 10 || xPos > 244)
		return;
	
	u32 curSize;
	u32 maxSize;
	getSoundLoc(&curSize, &maxSize);
	
	float percent = (float)(xPos - 10) / (float)(234);
	u32 newLoc = (int)(maxSize * percent);
	
	setSoundLoc(newLoc);
}
	
void volumeUp(int x, int y)
{
	if(getVolume() < 16)
	{
		setVolume(getVolume() + 1);
	}
}

void volumeDown(int x, int y)
{
	if(getVolume() > 0)
	{
		setVolume(getVolume() - 1);
	}
}

void loadSoundMode()
{
	char sStr[256];
	
	DRAGON_chdir("/");
	sprintf(sStr, "%ssound.dat", d_base);	
	
	DRAGON_FILE *fp = DRAGON_fopen(sStr, "r");
	
	soundMode = DRAGON_fgetc(fp) - '0';
	DRAGON_fclose(fp);
	
	if(soundMode < 0)
		soundMode = 0;
	if(soundMode > 2 && soundMode != 15)
		soundMode = 0;
}

void saveSoundMode()
{
	char sStr[256];
	
	DRAGON_chdir("/");
	sprintf(sStr, "%ssound.dat", d_base);	
	
	DRAGON_FILE *fp = DRAGON_fopen(sStr, "w");
	
	DRAGON_fputc(soundMode + '0', fp);
	DRAGON_fclose(fp);
}
