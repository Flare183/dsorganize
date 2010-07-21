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

#include <nds/bios.h>
#include <nds/arm7/touch.h>
#include <nds/arm7/clock.h>
#include <dswifi7.h>

#include "resetmem.h"
#include "../../arm9/source/soundIPC.h"
#include "wifi_shared.h"
#include "microphone7.h"

#include "click_raw.h"

#define VRAM_D		((s16*)0x06000000)
#define SIWRAM		((s16*)0x037F8000)

#define MIC_8
//#define MIC_16

extern volatile Wifi_MainStruct * WifiData;

bool touchDown = false;
bool touchDelay = false;

s16 *strpcmL0 = NULL;
s16 *strpcmL1 = NULL;
s16 *strpcmR0 = NULL;
s16 *strpcmR1 = NULL;

int lastL = 0;
int lastR = 0;

int multRate = 1;
int pollCount = 100; //start with a read

u32 sndCursor = 0;
u32 micBufLoc = 0;
u32 sampleLen = 0;
int sndRate = 0;

void FIFO_Receive();
void TIMER1Handler();
void micInterrupt();
void SendArm9Command(u32 command);

#ifdef __cplusplus
extern "C" {
#endif

int ReadFlashByte(int address);
int ReadFlashBytes(int address, int numbytes);
int crc16_slow(u8 * data, int length);
void GetWfcSettings();

#ifdef __cplusplus
}
#endif

// identical to the mic controls but i'm lazy, and arm7 has no need to be memory efficient

void pmSetReg(int reg, int control)
{
	SerialWaitBusy();
	REG_SPICNT = SPI_ENABLE | SPI_DEVICE_POWER | SPI_BAUD_1MHz | SPI_CONTINUOUS;
	REG_SPIDATA = reg;
	SerialWaitBusy();
	REG_SPICNT = SPI_ENABLE | SPI_DEVICE_POWER | SPI_BAUD_1MHz;
	REG_SPIDATA = control; 
}

u8 pmGetReg(int reg)
{
	SerialWaitBusy();
	REG_SPICNT = SPI_ENABLE | SPI_DEVICE_POWER | SPI_BAUD_1MHz | SPI_CONTINUOUS;
	REG_SPIDATA = reg | 0x80;
	SerialWaitBusy();
	REG_SPICNT = SPI_ENABLE | SPI_DEVICE_POWER | SPI_BAUD_1MHz ;
	REG_SPIDATA = 0;
	SerialWaitBusy();
	return REG_SPIDATA & 0xFF;
}

typedef void(*call3)(u32,void*,u32);
void read_nvram(u32 src, void *dst, u32 size) {
	((call3)0x2437)(src,dst,size);
}

u8 writeread(u8 data) {
	while (REG_SPICNT & SPI_BUSY);
	REG_SPIDATA = data;
	while (REG_SPICNT & SPI_BUSY);
	return REG_SPIDATA;
}

int flash_verify(u8 *src,u32 dst) 
{
	int i;
	u8 tmp[256];
	static int result=2;
	
	read_nvram(dst,tmp,256);
	for(i=0;i<256;i++) {
		if(tmp[i]!=src[i])
			break;
	}
	if(i==256)
		return result;
	
	result = 1;
	//write enable
	REG_SPICNT = SPI_ENABLE|SPI_CONTINUOUS|SPI_DEVICE_NVRAM;
	writeread(6);
	REG_SPICNT = 0;
	
	//Wait for Write Enable Latch to be set
	REG_SPICNT = SPI_ENABLE|SPI_CONTINUOUS|SPI_DEVICE_NVRAM;
	writeread(5);
	while((writeread(0)&0x02)==0); //Write Enable Latch
	REG_SPICNT = 0;
		
		

	 
	//page write
	REG_SPICNT = SPI_ENABLE|SPI_CONTINUOUS|SPI_DEVICE_NVRAM;
	writeread(0x0A);
	writeread((dst&0xff0000)>>16);
	writeread((dst&0xff00)>>8);
	writeread(0);
	for (i=0; i<256; i++) {
		writeread(src[i]);
	}

	REG_SPICNT = 0;
	// wait programming to finish
	REG_SPICNT = SPI_ENABLE|SPI_CONTINUOUS|SPI_DEVICE_NVRAM;
	writeread(0x05);
	while(writeread(0)&0x01);	//Write In Progress
	REG_SPICNT = 0;

	//read back & compare
	read_nvram(dst,tmp,256);
	for(i=0;i<256;i++) {
		if(tmp[i]!=src[i])
			return 0;
	}
	return 1;
}

void writeWifiSettings()
{
	// figure out where settings are
	u32 wfcBase = ReadFlashBytes(0x20, 2) * 8 - 0x400;
	
	// loop through wifi settings and reconstruct data
	int i;
	for(i=0;i<3;i++)
	{
		u8 wifiBlock[256];
		memset(wifiBlock, 0, 256);
		
		// read in the current data (if we don't do this, a new friendcode will have to be generated after saving)		
		Read_Flash(wfcBase + (i<<8), (char *)wifiBlock, 256);
		
		if(WifiData->wfc_enable[i] > 0)
		{
			// valid profile
			wifiBlock[0xE7] = 0x00;
			
			// modified from the wifi_arm7 read fuction
			wifiBlock[0xE6] = (WifiData->wfc_enable[i] & 0x0F);
			
			int n;
			for(n=0;n<16;n++) wifiBlock[0x80+n] = WifiData->wfc_wepkey[i][n];
			for(n=0;n<32;n++) wifiBlock[0x40+n] = WifiData->wfc_ap[i].ssid[n];
			
			memcpy(&wifiBlock[0xC0], (const void *)&WifiData->wfc_config[i][0], 4);
			memcpy(&wifiBlock[0xC4], (const void *)&WifiData->wfc_config[i][1], 4);
			memcpy(&wifiBlock[0xC8], (const void *)&WifiData->wfc_config[i][3], 4);
			memcpy(&wifiBlock[0xCC], (const void *)&WifiData->wfc_config[i][4], 4);
			
			// very simple subnet function
			// assumes they typed it in correctly
			char subnet = 0;
			for(n=0;n<32;n++)
			{
				if((WifiData->wfc_config[i][2] >> n) & 1)
				{
					subnet++;
				}
			}
			
			wifiBlock[0xD0] = subnet;
		}
		else
		{
			// invalid profile
			wifiBlock[0xE7] = 0xFF;
		}
		
		// calculate the crc
		uint16 crcBlock = crc16_slow(wifiBlock, 254);		
		memcpy(&wifiBlock[0xFE], &crcBlock, 2);
		
		// flash back to the firmware
		flash_verify(wifiBlock, wfcBase + (i<<8));
	}
}

static inline s16 checkClipping(int data)
{
	if(data > 32767)
		return 32767;
	if(data < -32768)
		return -32768;
	
	return data;
}

void mallocData(int size)
{
    // this no longer uses malloc due to using vram bank d.
  
	strpcmL0 = VRAM_D;
	strpcmL1 = strpcmL0 + (size >> 1);
	strpcmR0 = strpcmL1 + (size >> 1);
	strpcmR1 = strpcmR0 + (size >> 1);
	
	// clear vram d bank to not have sound leftover
	
	int i;
	
	for(i=0;i<(size);++i)
	{
		strpcmL0[i] = 0;
	}
	
	for(i=0;i<(size);++i)
	{
		strpcmR0[i] = 0;
	}
}

void freeData()
{	
	//free(strpcmR0);
}

void startSound(int sampleRate, const void* data, u32 bytes, u8 channel, u8 vol,  u8 pan, u8 format) 
{
	SCHANNEL_TIMER(channel)  = SOUND_FREQ(sampleRate);
	SCHANNEL_SOURCE(channel) = (u32)data;
	SCHANNEL_LENGTH(channel) = bytes >> 2;
	SCHANNEL_CR(channel)     = SCHANNEL_ENABLE | SOUND_ONE_SHOT | SOUND_VOL(vol) | SOUND_PAN(pan) | (format==1?SOUND_8BIT:SOUND_16BIT);
}

s32 getFreeSoundChannel() // modified to only look on channels 4 and up
{
	int i;
	for (i=4;i<16;++i) 
		if (!(SCHANNEL_CR(i) & SCHANNEL_ENABLE)) return i;
	
	return -1;
}

#define SPI_BAUD_1MHZ    2
#define SPI_BYTE_MODE   (0<<10)
#define PM_BATTERY_REG    1
#define PM_READ_REGISTER (1<<7)

//---------------------------------------------------------------------------------
int writePowerManagement(int reg, int command) {
//---------------------------------------------------------------------------------
  // Write the register / access mode (bit 7 sets access mode)
  while (REG_SPICNT & SPI_BUSY);
  REG_SPICNT = SPI_ENABLE | SPI_BAUD_1MHZ | SPI_BYTE_MODE | SPI_CONTINUOUS | SPI_DEVICE_POWER;
  REG_SPIDATA = reg;

  // Write the command / start a read
  while (REG_SPICNT & SPI_BUSY);
  REG_SPICNT = SPI_ENABLE | SPI_BAUD_1MHZ | SPI_BYTE_MODE | SPI_DEVICE_POWER;
  REG_SPIDATA = command;

  // Read the result
  while (REG_SPICNT & SPI_BUSY);
  return REG_SPIDATA & 0xFF;
}

int readPowerManagement(int reg) 
{
	return writePowerManagement((reg)|PM_READ_REGISTER, 0);
}

void VblankHandler(void) 
{
	uint16 but=0, x=0, y=0, xpx=0, ypx=0;
	uint8 ct[sizeof(IPC->curtime)];
	u32 i;
	
	// Read the touch screen
 
	but = REG_KEYXY;
 
	if (!(but & (1<<6))) 
	{
		if(touchDelay == false)
		{
			touchDelay = true;
		}
		else
		{
			touchPosition tempPos = touchReadXY();
			
			x = tempPos.x;
			y = tempPos.y;
			xpx = tempPos.px;
			ypx = tempPos.py;
			
			if(!touchDown)
			{
				touchDown = true;
				soundIPC->tX=tempPos.px;
				soundIPC->tY=tempPos.py;
				SendArm9Command(ARM9COMMAND_TOUCHDOWN);
			}
			else
			{
				soundIPC->tX=tempPos.px;
				soundIPC->tY=tempPos.py;
				SendArm9Command(ARM9COMMAND_TOUCHMOVE);	
			}
		}
	}
	else
	{
		touchDelay = false;
		if(touchDown)
		{
			touchDown = false;
			SendArm9Command(ARM9COMMAND_TOUCHUP);
		}
	}
	
	// Update the IPC struct
	IPC->buttons  = but;
	IPC->touchX   = x;
	IPC->touchY   = y;
	IPC->touchXpx = xpx;
	IPC->touchYpx = ypx;
	IPC->battery  = readPowerManagement(PM_BATTERY_REG);
	
	if(pollCount >= 30)
	{
		// Read the time
		rtcGetTime((uint8 *)ct);
		BCDToInteger((uint8 *)&(ct[1]), 7);	  
		
		for(i=0; i<sizeof(ct); ++i)
			IPC->curtime[i] = ct[i];
		
		pollCount = 0;
	}
	else
		pollCount++;
 
	TransferSound *snd = IPC->soundData;
	IPC->soundData = 0;
	
	if (0 != snd) {
	
		for (i=0; i<snd->count; ++i) {
			s32 chan = getFreeSoundChannel();
			
			if (chan >= 0) {
				startSound(snd->data[i].rate, snd->data[i].data, snd->data[i].len, chan, snd->data[i].vol, snd->data[i].pan, snd->data[i].format);
			}
		}
	}
	
	Wifi_Update();
}

void setSwapChannel()
{
	s16 *buf;
  
	if(!sndCursor)
		buf = strpcmL0;
	else
		buf = strpcmL1;
    
	// Left channel
	SCHANNEL_SOURCE((sndCursor << 1)) = (uint32)buf;
	SCHANNEL_CR((sndCursor << 1)) = SCHANNEL_ENABLE | SOUND_ONE_SHOT | SOUND_VOL(0x7F) | SOUND_PAN(0) | SOUND_16BIT;
    	
	if(!sndCursor)
		buf = strpcmR0;
	else
		buf = strpcmR1;
	
	// Right channel
	SCHANNEL_SOURCE((sndCursor << 1) + 1) = (uint32)buf;
	SCHANNEL_CR((sndCursor << 1) + 1) = SCHANNEL_ENABLE | SOUND_ONE_SHOT | SOUND_VOL(0x7F) | SOUND_PAN(0x3FF) | SOUND_16BIT;
  
	sndCursor = 1 - sndCursor;
}

void SetupSound() 
{
    sndCursor = 0;
	
	if(multRate != 1 && multRate != 2 && multRate != 4)
		multRate = 1;
	
	mallocData(sampleLen * 2 * multRate);
    
	TIMER0_DATA = SOUND_FREQ((sndRate * multRate));
	TIMER0_CR = TIMER_DIV_1 | TIMER_ENABLE;
  
	TIMER1_DATA = 0x10000 - (sampleLen * 2 * multRate);
	TIMER1_CR = TIMER_CASCADE | TIMER_IRQ_REQ | TIMER_ENABLE;
	
	irqSet(IRQ_TIMER1, TIMER1Handler);
	
	int ch;
	
	for(ch=0;ch<4;++ch)
	{
		SCHANNEL_CR(ch) = 0;
		SCHANNEL_TIMER(ch) = SOUND_FREQ((sndRate * multRate));
		SCHANNEL_LENGTH(ch) = (sampleLen * multRate) >> 1;
		SCHANNEL_REPEAT_POINT(ch) = 0;
	}
	
	irqSet(IRQ_VBLANK, 0);
	irqDisable(IRQ_VBLANK);
	
	lastL = 0;
	lastR = 0;
}

void StopSound() 
{
	irqSet(IRQ_TIMER1, 0);
	TIMER0_CR = 0;
	TIMER1_CR = 0;
	
	SCHANNEL_CR(0) = 0;
	SCHANNEL_CR(1) = 0;
	SCHANNEL_CR(2) = 0;
	SCHANNEL_CR(3) = 0;
	
	freeData();
	irqSet(IRQ_VBLANK, VblankHandler);
	irqEnable(IRQ_VBLANK);	
}

void TIMER1Handler() 
{	
	setSwapChannel();
	SendArm9Command(ARM9COMMAND_UPDATE_BUFFER);
}

void micStartRecording()
{
	micBufLoc = 0;
	sndCursor = 0;
	
	TurnOnMicrophone();
	PM_SetGain(GAIN_160);
	
	TIMER2_DATA = TIMER_FREQ(sndRate);
	TIMER2_CR = TIMER_ENABLE | TIMER_DIV_1 | TIMER_IRQ_REQ;	
	irqSet(IRQ_TIMER2, micInterrupt); // microphone stuff
}

void micStopRecording()
{
	irqSet(IRQ_TIMER2, 0); // microphone stuff	
	TIMER2_CR = 0;
	
	TurnOffMicrophone();
}

void micInterrupt()
{
#ifdef MIC_8
	u8 *micData = NULL;
	
	switch(sndCursor)
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
	
	int tempX = (MIC_GetData8() - 128) * 4;
	
	if(tempX > 127)
		tempX = 127;
	if(tempX < -128)
		tempX = -128;
	
	tempX += 128;
	
	micData[micBufLoc] = (u8)tempX;
	
	++micBufLoc;
	if(micBufLoc == sampleLen)
	{
		micBufLoc = 0;
		soundIPC->channels = sndCursor;
		++sndCursor;
		if(sndCursor > 2)
			sndCursor = 0;
		
		SendArm9Command(ARM9COMMAND_SAVE_DATA); // send command to save the buffer
	}
#endif
#ifdef MIC_16
	s16 *micData = NULL;
	
	switch(sndCursor)
	{
		case 0:
			micData = (s16 *)soundIPC->arm9L;
			break;
		case 1:
			micData = (s16 *)soundIPC->arm9R;
			break;
		case 2:
			micData = (s16 *)soundIPC->interlaced;
			break;
	}
	
	int tempX = (MIC_ReadData12() - 2048) << 6;

	if(tempX > 32767)
		tempX = 32767;
	if(tempX < -32768)
		tempX = -32768;
	
	micData[micBufLoc++] = tempX;
	
	if(micBufLoc == sampleLen)
	{
		micBufLoc = 0;
		soundIPC->channels = sndCursor;
		++sndCursor;
		if(sndCursor > 2)
			sndCursor = 0;
		
		SendArm9Command(ARM9COMMAND_SAVE_DATA); // send command to save the buffer
	}
#endif
}

void SendArm9Command(u32 command) 
{	
	while((REG_IPC_FIFO_CR & IPC_FIFO_SEND_FULL))
		swiWaitForVBlank();
	
    REG_IPC_FIFO_TX = command;
}

void FIFO_Receive() 
{
	while(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY))
	{
		u32 command = REG_IPC_FIFO_RX;
		
		switch((command >> 24) & 0xFF) 
		{
			case ARM7COMMAND_START_SOUND:
				SetupSound();
				break;
			case ARM7COMMAND_STOP_SOUND:
				StopSound();
				break;
			case ARM7COMMAND_SOUND_SETRATE:			
				sndRate = (command & 0x00FFFFFF);
				break;
			case ARM7COMMAND_SOUND_SETLEN:		
				sampleLen = (command & 0x00FFFFFF);
				break;
			case ARM7COMMAND_SOUND_SETMULT:		
				multRate = (command & 0x00FFFFFF);
				break;
			case ARM7COMMAND_SOUND_COPY:
			{
				s16 *lbuf = NULL;
				s16 *rbuf = NULL;
				
				if(!sndCursor)
				{
					lbuf = strpcmL0;
					rbuf = strpcmR0;
				}
				else
				{
					lbuf = strpcmL1;
					rbuf = strpcmR1;
				}
				
				u32 i;
				int vMul = soundIPC->volume;
				int lSample = 0;
				int rSample = 0;
				s16 *arm9LBuf = soundIPC->arm9L;
				s16 *arm9RBuf = soundIPC->arm9R;
				
				switch(multRate)
				{
					case 1:				
						for(i=0;i<sampleLen;++i)
						{
							lSample = ((*arm9LBuf++) * vMul) >> 2;
							rSample = ((*arm9RBuf++) * vMul) >> 2;
							
							*lbuf++ = checkClipping(lSample);
							*rbuf++ = checkClipping(rSample);
						}
						
						break;
					case 2:	
						for(i=0;i<sampleLen;++i)
						{
							lSample = ((*arm9LBuf++) * vMul) >> 2;
							rSample = ((*arm9RBuf++) * vMul) >> 2;
							
							int midLSample = (lastL + lSample) >> 1;
							int midRSample = (lastR + rSample) >> 1;
							
							*lbuf++ = checkClipping(midLSample);
							*rbuf++ = checkClipping(midRSample);
							*lbuf++ = checkClipping(lSample);
							*rbuf++ = checkClipping(rSample);
							
							lastL = lSample;
							lastR = rSample;
						}
						
						break;
					case 4:
						// unrolling this one out completely because it's soo much slower
						
						for(i=0;i<sampleLen;++i)
						{
							lSample = ((*arm9LBuf++) * vMul) >> 2;
							rSample = ((*arm9RBuf++) * vMul) >> 2;
							
							int midLSample = (lastL + lSample) >> 1;
							int midRSample = (lastR + rSample) >> 1;
							
							int firstLSample = (lastL + midLSample) >> 1;
							int firstRSample = (lastR + midRSample) >> 1;
							
							int secondLSample = (midLSample + lSample) >> 1;
							int secondRSample = (midRSample + rSample) >> 1;
							
							*lbuf++ = checkClipping(firstLSample);
							*rbuf++ = checkClipping(firstRSample);
							*lbuf++ = checkClipping(midLSample);
							*rbuf++ = checkClipping(midRSample);
							*lbuf++ = checkClipping(secondLSample);
							*rbuf++ = checkClipping(secondRSample);
							*lbuf++ = checkClipping(lSample);
							*rbuf++ = checkClipping(rSample);							
							
							lastL = lSample;
							lastR = rSample;							
						}
						
						break;
				}
				
				VblankHandler();
				break;
			}
			case ARM7COMMAND_SOUND_DEINTERLACE:
			{
				s16 *lbuf = NULL;
				s16 *rbuf = NULL;
				
				if(!sndCursor)
				{
					lbuf = strpcmL0;
					rbuf = strpcmR0;
				}
				else
				{
					lbuf = strpcmL1;
					rbuf = strpcmR1;
				}
				
				s16 *iSrc = soundIPC->interlaced;
				u32 i = 0;
				int vMul = soundIPC->volume;
				int lSample = 0;
				int rSample = 0;
				
				switch(multRate)
				{
					case 1:
						if(soundIPC->channels == 2)
						{
							for(i=0;i<sampleLen;++i)
							{					
								lSample = *iSrc++;
								rSample = *iSrc++;
								
								*lbuf++ = checkClipping((lSample * vMul) >> 2);
								*rbuf++ = checkClipping((rSample * vMul) >> 2);
							}
						}
						else
						{
							for(i=0;i<sampleLen;++i)
							{					
								lSample = *iSrc++;
								
								lSample = checkClipping((lSample * vMul) >> 2);
								
								*lbuf++ = lSample;
								*rbuf++ = lSample;
							}
						}
						
						break;
					case 2:	
						for(i=0;i<sampleLen;++i)
						{					
							if(soundIPC->channels == 2)
							{
								lSample = *iSrc++;
								rSample = *iSrc++;
							}
							else
							{
								lSample = *iSrc++;
								rSample = lSample;
							}
							
							lSample = ((lSample * vMul) >> 2);
							rSample = ((rSample * vMul) >> 2);
							
							int midLSample = (lastL + lSample) >> 1;
							int midRSample = (lastR + rSample) >> 1;
							
							lbuf[(i << 1)] = checkClipping(midLSample);
							rbuf[(i << 1)] = checkClipping(midRSample);
							lbuf[(i << 1) + 1] = checkClipping(lSample);
							rbuf[(i << 1) + 1] = checkClipping(rSample);
							
							lastL = lSample;
							lastR = rSample;							
						}
						
						break;
					case 4:
						for(i=0;i<sampleLen;++i)
						{				
							if(soundIPC->channels == 2)
							{
								lSample = *iSrc++;
								rSample = *iSrc++;
							}
							else
							{
								lSample = *iSrc++;
								rSample = lSample;
							}
							
							lSample = ((lSample * vMul) >> 2);
							rSample = ((rSample * vMul) >> 2);
							
							int midLSample = (lastL + lSample) >> 1;
							int midRSample = (lastR + rSample) >> 1;
							
							int firstLSample = (lastL + midLSample) >> 1;
							int firstRSample = (lastR + midRSample) >> 1;
							
							int secondLSample = (midLSample + lSample) >> 1;
							int secondRSample = (midRSample + rSample) >> 1;
							
							lbuf[(i << 2)] = checkClipping(firstLSample);
							rbuf[(i << 2)] = checkClipping(firstRSample);
							lbuf[(i << 2) + 1] = checkClipping(midLSample);
							rbuf[(i << 2) + 1] = checkClipping(midRSample);
							lbuf[(i << 2) + 2] = checkClipping(secondLSample);
							rbuf[(i << 2) + 2] = checkClipping(secondRSample);
							lbuf[(i << 2) + 3] = checkClipping(lSample);
							rbuf[(i << 2) + 3] = checkClipping(rSample);							
							
							lastL = lSample;
							lastR = rSample;							
						}
						
						break;
				}
				
				VblankHandler();
				break;
			}
			case ARM7COMMAND_START_RECORDING:
				micStartRecording();
				break;				
			case ARM7COMMAND_STOP_RECORDING:
				micStopRecording();
				break;
			case 0xF0: // sgstair lib
				while((REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY))
					swiWaitForVBlank();
				
				command = REG_IPC_FIFO_RX;
				
				Wifi_Init(command);	
				
				break;
			case 0xF1: // sgstair lib
				Wifi_Sync();
				break;
			case ARM7COMMAND_BOOT_MIGHTYMAX:
				while(1)
				{
					if ((*(vu32 *)0x27FFFFC)) 
					{
						REG_IE = 0 ;
						REG_IME = 0 ;
						REG_IF = 0xFFFF ;
						// now jump where the arm9 calls us too
						// exclusive ram should now be out of use, 
						// and is then ready to be overwritten with the new arm7 code
						
						typedef void (*eloop_type)(void);
						
						eloop_type eloop = *(eloop_type*)0x27FFFFC;
						
						*(vu32 *)0x27FFFFC = 0;
						*(vu32 *)0x27FFFF8 = 0;
						
						eloop();
					}
					
					swiWaitForVBlank();
				}
				break;
			case ARM7COMMAND_BOOT_CHISHM:
				REG_IE = 0;
				REG_IME = 0;
				REG_IF = 0xFFFF;
				
				*((vu32*)0x027FFE34) = (u32)0x06000000;
				swiSoftReset();
				
				break;
			case ARM7COMMAND_PSG_COMMAND:
			{				
				SCHANNEL_CR(soundIPC->psgChannel) = soundIPC->cr;
                SCHANNEL_TIMER(soundIPC->psgChannel) = soundIPC->timer;
				break;
			}
			case ARM7COMMAND_SAVE_WIFI:
			{
				writeWifiSettings();
				soundIPC->channels = 1;
				break;
			}
			case ARM7COMMAND_LOAD_WIFI:
			{
				GetWfcSettings();
				soundIPC->channels = 1;
				break;
			}
			case ARM7COMMAND_PLAYCLICK:
			{
				s32 chan = getFreeSoundChannel();
				
				if (chan >= 0) 
				{
					startSound(11025, click_raw, click_raw_size, chan, 40, 63, 1);
				}
				break;
			}
		}
	}
}

void arm7_synctoarm9() 
{
	REG_IPC_FIFO_TX = 0x87654321;
}

int main(int argc, char ** argv) 
{
	// Reset the clock if needed
	rtcReset();
	
	POWER_CR = POWER_SOUND;
	SOUND_CR = SOUND_ENABLE | 127;
	
	*(vu32 *)0x27FFFFC = 0; // arm7 loop address to default
 
	irqInit();
	irqSet(IRQ_VBLANK, VblankHandler);
	irqEnable(IRQ_VBLANK);
	
	irqSet(IRQ_TIMER1, 0);
	irqEnable(IRQ_TIMER1);
	
	irqSet(IRQ_WIFI, Wifi_Interrupt); // set up wifi interrupt
	irqEnable(IRQ_WIFI);
	
	irqSet(IRQ_TIMER2, 0); // microphone stuff
	irqEnable(IRQ_TIMER2);
	
	irqSet(IRQ_FIFO_NOT_EMPTY, FIFO_Receive);
	irqEnable(IRQ_FIFO_NOT_EMPTY);
	REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_SEND_CLEAR | IPC_FIFO_RECV_IRQ;
	
	Wifi_SetSyncHandler(arm7_synctoarm9);
	SendArm9Command(ARM9COMMAND_INIT);
	
	// Keep the ARM7 out of main RAM
	while (1) 
		swiWaitForVBlank();
}
