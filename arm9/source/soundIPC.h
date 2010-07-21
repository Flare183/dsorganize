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

#define soundIPC ((SoundRegion volatile *)(0x027FF000 + sizeof(TransferRegion)))

#define ARM7STATE_IDLE 0
#define ARM7STATE_WAITING 1
#define ARM7STATE_WAITCOPY 2

#define ARM7COMMAND_START_SOUND 0xC1
#define ARM7COMMAND_STOP_SOUND 0xC2
#define ARM7COMMAND_SOUND_SETMULT 0xC3
#define ARM7COMMAND_SOUND_SETRATE 0xC5
#define ARM7COMMAND_SOUND_SETLEN 0xC6
#define ARM7COMMAND_SOUND_COPY 0xC7
#define ARM7COMMAND_SOUND_DEINTERLACE 0xC8
#define ARM7COMMAND_START_RECORDING 0xC9
#define ARM7COMMAND_STOP_RECORDING 0xCA
#define ARM7COMMAND_BOOT_GBAMP 0xCB
#define ARM7COMMAND_BOOT_SUPERCARD 0xCC
#define ARM7COMMAND_BOOT_MIGHTYMAX 0xCD
#define ARM7COMMAND_BOOT_CHISHM 0xCE
#define ARM7COMMAND_PSG_COMMAND 0xD0
#define ARM7COMMAND_SAVE_WIFI 0xD1 
#define ARM7COMMAND_LOAD_WIFI 0xD2
#define ARM7COMMAND_PLAYCLICK 0xE0

#define ARM9COMMAND_SUCCESS 0xFFFFFF01
#define ARM9COMMAND_INIT 0xFFFFFF02
#define ARM9COMMAND_UPDATE_BUFFER 0xFFFFFF03
#define ARM9COMMAND_SAVE_DATA 0xFFFFFF04
#define ARM9COMMAND_TOUCHDOWN 0xFFFFFF05
#define ARM9COMMAND_TOUCHMOVE 0xFFFFFF06
#define ARM9COMMAND_TOUCHUP 0xFFFFFF07

typedef struct
{
	s16 *arm9L;
	s16 *arm9R;
	
	s16 *interlaced;
	int channels;
	u8 volume;
	
	u32 tX;
	u32 tY;
	
	int psgChannel;
	u32 cr;
	u32 timer;
} SoundRegion;

