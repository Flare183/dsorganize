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
#include "soundrecorder.h"
#include "browser.h"
#include "fatwrapper.h"
#include "colors.h"
#include "soundIPC.h"
#include "general.h"
#include "fonts.h"
#include "globals.h"
#include "settings.h"
#include "graphics.h"
#include "sound.h"
#include "filerout.h"
#include "language.h"
#include "controls.h"
#include "help.h"

static DRAGON_FILE *recFile = NULL;
static bool isRecording = false;
static int recordMode = SR_RECORDING;
static int wavLength = 0;
static u32 ticCount = 0;

// prototypes
void drawTopRecordScreen();
void drawBottomRecordScreen();
void recorderStartPressed(int x, int y);
void switchRecordingMode(int x, int y);
void recorderForward(int x, int y);
void recorderBack(int x, int y);
void seekRecordSound(int xPos, int yPos);

void updateRecordingCount()
{
	if(isRecording)
	{
		ticCount++;
	}
}

int getWavLength(char *fName)
{
	wavFormatChunk headerChunk;	
	char header[13];
	
	DRAGON_FILE *fp = DRAGON_fopen(fName, "r");	
	DRAGON_fread(header, 1, 12, fp);
	
	header[12] = 0;
	header[4] = ' ';
	header[5] = ' ';
	header[6] = ' ';
	header[7] = ' ';
	
	if(strcmp(header, "RIFF    WAVE") != 0)
	{
		// wrong header
		
		DRAGON_fclose(fp);
		return -1;
	}
	
	DRAGON_fread(&headerChunk, sizeof(wavFormatChunk), 1, fp);
	
	if(strncmp(headerChunk.chunkID, "fmt ", 4) != 0)
	{
		// wrong chunk at beginning
		
		DRAGON_fclose(fp);
		return -1;
	}
	
	if(headerChunk.wFormatTag != 1)
	{
		// compression used, hell no to loading this
		
		DRAGON_fclose(fp);
		return -1;
	}
	
	if(headerChunk.wChannels > 2)
	{
		// more than 2 channels.... uh no!
		
		DRAGON_fclose(fp);
		return -1;
	}
	
	DRAGON_fseek(fp, (headerChunk.chunkSize + 8) + 12, 0); // seek to next chunk
	
	char fmtHeader[5];
	
	DRAGON_fread(fmtHeader, 1, 4, fp);
	fmtHeader[4] = 0;
	
	if(strcmp(fmtHeader, "data") != 0)
	{
		// wrong chunk next, sorry, doing strict only
		
		DRAGON_fclose(fp);
		return -1;
	}
	
	if(headerChunk.wBitsPerSample <= 8)
	{
		headerChunk.wBitsPerSample = 8;
	}
	else if(headerChunk.wBitsPerSample <= 16)
	{
		headerChunk.wBitsPerSample = 16;
	}
	else if(headerChunk.wBitsPerSample <= 24)
	{
		headerChunk.wBitsPerSample = 24;
	}
	else if(headerChunk.wBitsPerSample <= 32)
	{
		headerChunk.wBitsPerSample = 32;
	}
	
	u32 len = 0;
	DRAGON_fread(&len, 4, 1, fp);	
	DRAGON_fclose(fp);
	
	len /= ((headerChunk.wBitsPerSample >> 3) * headerChunk.wChannels * headerChunk.dwSamplesPerSec);
	
	return (int)len;
}

void loadWave()
{
	DRAGON_FILE *fp = DRAGON_fopen(getFileName(), "r");
	
	if(DRAGON_flength(fp) == 0)
	{
		// this file is new
		
		recordMode = SR_RECORDING;
		DRAGON_fclose(fp);
	}
	else
	{
		DRAGON_fclose(fp);
		wavLength = getWavLength(getFileName());
		
		if(wavLength == -1)
		{
			recordMode = SR_RECORDING;
		}
		else
		{
			recordMode = SR_PLAYBACK;
		}
	}
}

void drawRecorderScreen()
{
	drawTopRecordScreen();
	drawBottomRecordScreen();
}

void setButtonCaptions()
{
	if(recordMode == SR_RECORDING)
	{			
		if(isRecording)
		{
			updateControl(CONTROL_FORWARD, l_stop);
		}
		else
		{
			updateControl(CONTROL_FORWARD, l_record);
		}
		
		updateControl(CONTROL_BACKWARD, NULL);
	}
	else
	{	
		if(getState() == STATE_PLAYING)
		{
			updateControl(CONTROL_FORWARD, l_pause);
		}
		else
		{
			updateControl(CONTROL_FORWARD, l_play);
		}
		
		updateControl(CONTROL_BACKWARD, l_stop);
	}
}

void createSoundRecorder()
{
	clearControls();
	
	registerScreenUpdate(drawRecorderScreen, NULL);
	registerHelpScreen("recorder.html", true);
	
	addHome(l_back, recorderStartPressed);
	addDefaultButtons(NULL, recorderForward, NULL, recorderBack);
	addHotKey(0, KEY_X, switchRecordingMode, 0, NULL);
	
	newControl(0xFF, 10, 20, 244, 40, CT_HITBOX, NULL);
	setControlCallbacks(0xFF, seekRecordSound, NULL, NULL);
	
	setButtonCaptions();
}

void initSoundRecorder()
{
	// load the wav file in question
	loadWave();
	
	// create the interface
	createSoundRecorder();
}

void copyChunk()
{
	if(!isRecording)
		return;
	
	u8 *micData = NULL;
	
	switch(soundIPC->channels)
	{
		case 0:
			micData = (u8 *)soundIPC->arm9L;
			break;
		case 1:
			micData = (u8 *)soundIPC->arm9R;
			break;
		case 2:
			micData = (u8 *)soundIPC->interlaced;
			break;
	}
	
	checkKeys();
	DRAGON_fwrite(micData, 1, REC_BLOCK_SIZE, recFile);
	checkKeys();
}

void startRecording()
{
	if(isRecording)
		return;
	
	if(DRAGON_FileExists(getFileName()) != FE_NONE)
		DRAGON_remove(getFileName());
	
	recFile = DRAGON_fopen(getFileName(), "w");
	
	// write header
	DRAGON_fwrite("RIFF    WAVE", 1, 12, recFile);
	
	// write fmt tag
	wavFormatChunk headerChunk;
	
	strncpy(headerChunk.chunkID, "fmt ", 4);
	headerChunk.chunkSize = 16;
	headerChunk.wFormatTag = 1;
	headerChunk.wChannels = 1;
	headerChunk.wBitsPerSample = 8;
	headerChunk.dwSamplesPerSec = REC_FREQ;
	headerChunk.wBlockAlign = 1;
	headerChunk.dwAvgBytesPerSec = REC_FREQ;
	
	DRAGON_fwrite(&headerChunk, sizeof(wavFormatChunk), 1, recFile);
	
	// write data chunk
	DRAGON_fwrite("data    ", 1, 8, recFile);
	
	soundIPC->arm9L = (s16 *)trackMalloc(REC_BLOCK_SIZE, "record block");
	soundIPC->arm9R = (s16 *)trackMalloc(REC_BLOCK_SIZE, "record block");
	soundIPC->interlaced = (s16 *)trackMalloc(REC_BLOCK_SIZE, "record block");

	memset(soundIPC->arm9L, 0, REC_BLOCK_SIZE);
	memset(soundIPC->arm9R, 0, REC_BLOCK_SIZE);
	memset(soundIPC->interlaced, 0, REC_BLOCK_SIZE);
	
	setSoundFrequency(REC_FREQ);	
	setSoundLength(REC_BLOCK_SIZE);
	SendArm7Command(ARM7COMMAND_START_RECORDING, 0);
	
	ticCount = 0;
	isRecording = true;
}

void endRecording()
{
	if(!isRecording)
		return;
	
	SendArm7Command(ARM7COMMAND_STOP_RECORDING, 0);
	isRecording = false;
	
	trackFree(soundIPC->arm9L);
	trackFree(soundIPC->arm9R);
	trackFree(soundIPC->interlaced);
	
	u32 length = DRAGON_flength(recFile) - 8;
	DRAGON_fseek(recFile, 4, 0);
	DRAGON_fwrite(&length, 4, 1, recFile);
	
	length = DRAGON_flength(recFile) - 44;
	DRAGON_fseek(recFile, 40, 0);
	DRAGON_fwrite(&length, 4, 1, recFile);
	
	DRAGON_fclose(recFile);
	
	recFile = NULL;
}

void drawTopRecordScreen()
{
	drawFileInfoScreen();
	
	char str[128];	
	sprintf(str, "%c %s: ", BUTTON_X, l_recordingmode);
	
	switch(recordMode)
	{
		case SR_RECORDING:
			strcat(str, l_record);
			break;
		case SR_PLAYBACK:
			strcat(str, l_playback);
			break;
	}
	
	setColor(genericTextColor);
	fb_setDefaultClipping();
	fb_dispString(5, 175, str);
}

void drawPlayProgress(u32 curSize, u32 maxSize)
{
	bg_drawFilledRect(10, 20, 244, 40, widgetBorderColor, soundSeekFillColor);
	
	float percent = (float)(curSize) / (float)maxSize;
	int pixel = (int)(232 * percent);
	
	bg_drawRect(11, 21, 11 + pixel, 39, soundSeekForeColor);	
}

void drawBottomRecordScreen()
{	
	char str[20];
	
	if(recordMode == SR_RECORDING)
	{	
		setColor(genericTextColor);
		sprintf(str, "%s: %d", l_length, ticCount / 60);
		bg_dispString(10, 20, str);	
	}
	else
	{
		if(getState() == STATE_STOPPED || getState() == STATE_UNLOADED)
		{
			drawPlayProgress(0, 1);
		}
		else
		{
			u32 curSize;
			u32 maxSize;
			getSoundLoc(&curSize, &maxSize);
			
			drawPlayProgress(curSize, maxSize);
		}
		
		setColor(genericTextColor);
		sprintf(str, "%s: %d", l_length, wavLength);
		bg_dispString(10, 60, str);	
	}
	
	setButtonCaptions();
}

void recorderStartPressed(int x, int y)
{
	if(recordMode == SR_PLAYBACK)
	{
		switch(getState())
		{
			case STATE_UNLOADED:
			case STATE_STOPPED:
				break;
			default:
				closeSound();
		}
	}
	else if(recordMode == SR_RECORDING)
	{
		if(isRecording)
		{
			endRecording();
		}
	}
	
	setMode(BROWSER);
	freeDirList();
	createBrowser(true);
}

void recorderForward(int x, int y)
{
	if(recordMode == SR_PLAYBACK)
	{
		switch(getState())
		{
			case STATE_PLAYING:
				pauseSound(true);
				break;
			case STATE_PAUSED:
				pauseSound(false);
				break;
			case STATE_STOPPED: // sound hasn't started yet
			case STATE_UNLOADED: // sound hasn't started yet
				loadSound(getFileName());
				break;
		}
		
		return;
	}
	else if(recordMode == SR_RECORDING)
	{
		if(isRecording)
		{
			endRecording();
		}
		else
		{
			startRecording();
		}
		
		return;
	}
}

void recorderBack(int x, int y)
{
	if(recordMode == SR_PLAYBACK)
	{
		switch(getState())
		{
			case STATE_UNLOADED:
			case STATE_STOPPED:
				break;
			default:
				closeSound();
		}
	}
}

void seekRecordSound(int xPos, int yPos)
{
	if(recordMode != SR_PLAYBACK)
		return;
	if(getState() == STATE_STOPPED || getState() == STATE_UNLOADED)
		return;
	
	u32 curSize;
	u32 maxSize;
	getSoundLoc(&curSize, &maxSize);
	
	float percent = (float)xPos / (float)234;
	u32 newLoc = (int)(maxSize * percent);
	
	setSoundLoc(newLoc);
}

void switchRecordingMode(int x, int y)
{
	if(recordMode == SR_PLAYBACK)
	{
		switch(getState())
		{
			case STATE_UNLOADED:
			case STATE_STOPPED:
				break;
			default:
				closeSound();
		}
		
		recordMode = SR_RECORDING;
	}
	else if(recordMode == SR_RECORDING)
	{	
		if(isRecording)
		{
			endRecording();
		}
		
		recordMode = SR_PLAYBACK;		
		wavLength = getWavLength(getFileName());
		
		if(wavLength == -1)
		{
			// Don't allow playback screen if invalid wav file
			recordMode = SR_RECORDING;
		}
	}
}
