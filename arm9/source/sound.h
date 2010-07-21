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
 
#ifndef _SOUND_INCLUDED
#define _SOUND_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <libsound/aacdec.h>
#include "fatwrapper.h"
#include "http.h"
#include "api68\api68.h"

enum {
	SRC_NONE,
	SRC_WAV,
	SRC_MIKMOD,
	SRC_MP3,
	SRC_OGG,
	SRC_AAC,
	SRC_FLAC,
	SRC_SID,
	SRC_NSF,
	SRC_SPC,
	SRC_SNDH,
	SRC_GBS,
	SRC_STREAM_MP3,
	SRC_STREAM_OGG,
	SRC_STREAM_AAC
};

#define ARM9COPY 0
#define ARM7COPY 1

#define STATE_PLAYING 0
#define STATE_PAUSED 1
#define STATE_STOPPED 2
#define STATE_UNLOADED 3

// 2048 creates the crash bug in fat_fread
#define WAV_READ_SIZE 4096

#define MP3_READ_SIZE 8192
#define MP3_WRITE_SIZE 2048
#define STREAM_MP3_READ_SIZE 1024
#define STREAM_BUFFER_SIZE (1024*512) // 512 kb buffer
#define STREAM_WIFI_READ_SIZE (1024*20) // 20 kb recieve
#define STREAM_CACHE_SIZE (1024*64) // 64 kb cache size

#define OGG_READ_SIZE 1024

#define MIKMOD_FREQ 44100

#define AAC_READBUF_SIZE	(2 * AAC_MAINBUF_SIZE * AAC_MAX_NCHANS)
#define AAC_OUT_SIZE 32768
#ifdef AAC_ENABLE_SBR
  #define SBR_MUL		2
#else
  #define SBR_MUL		1
#endif

#define FLAC_OUT_SIZE 4096

#define SID_FREQ 48000
#define SID_OUT_SIZE 2048
#define SID_META_LOC 0x16

#define NSF_FREQ 48000
#define NSF_OUT_SIZE 2048

#define SPC_FREQ 32000
#define SPC_OUT_SIZE 2048

#define SNDH_OUT_SIZE 4096

#define GBS_FREQ 32000
#define GBS_OUT_SIZE 2048

// streaming stuff
#define STREAM_DISCONNECTED 0
#define STREAM_CONNECTING 1
#define STREAM_CONNECTED 2
#define STREAM_FAILEDCONNECT 3
#define STREAM_STREAMING 4
#define STREAM_STARTING 5
#define STREAM_BUFFERING 6
#define STREAM_TRYNEXT 7

#define ICY_HEADER_SIZE 2048

#define REG_SIWRAMCNT (*(vu8*)0x04000247)
#define SIWRAM0 ((s16 *)0x037F8000)
#define SIWRAM1 ((s16 *)0x037FC000)

// mikmod
#include ".\mikmod\drivers\drv_nos.h"

typedef struct 
{
	char chunkID[4];
	long chunkSize;

	short wFormatTag;
	unsigned short wChannels;
	unsigned long dwSamplesPerSec;
	unsigned long dwAvgBytesPerSec;
	unsigned short wBlockAlign;
	unsigned short wBitsPerSample;
} wavFormatChunk;

typedef struct
{
	int sourceFmt;
	int bufLoc;
	int channels;
	DRAGON_FILE *filePointer;
	int bits;
	u32 len;
	u32 loc;
	u32 dataOffset;
	u32 dataLen;
	int mp3SampleRate;
} sndData;

void initComplexSound();
void waitForInit();
void setSoundInterrupt();

// playback
bool loadSound(char *fName);
void updateStreamLoop();
void loadSoundGeneric(u32 wPlugin, int rate, int multiplicity, int sampleLength);
void pauseSound(bool pause);
void getSoundLoc(u32 *loc, u32 *max);
void setSoundLoc(u32 loc);
void closeSound();
void freeSound(); // should not be called unless absolutely necessary
int getState();
int getSoundLength();
int getSourceFmt();
void soundPrevTrack(int x, int y);
void soundNextTrack(int x, int y);
int getSNDHTrack();
int getSNDHTotalTracks();
void getSNDHMeta(api68_music_info_t * info);
void loadWavToMemory();
int getSIDTrack();
int getSIDTotalTracks();
int getGBSTrack();
int getGBSTotalTracks();
char *gbsMeta(int which);

// wifi
void SendArm7Command(u32 command, u32 data);
int getCurrentStatus();
ICY_HEADER *getStreamData();

// for streaming record interrupt
void copyChunk();
void setSoundFrequency(u32 freq);
void setSoundLength(u32 len);

u32 getSoundChannels();
u8 getVolume();
void setVolume(u8 volume);
int getStreamLag();
int getStreamLead();
char *sidMeta(int which);

// for about screen to set or clear the ability to loop modules
void setLoop();
void clearLoop();

#ifdef __cplusplus
}
#endif

#endif
