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
 
// DS Wifi interface code
// Copyright (C) 2005-2006 Stephen Stair - sgstair@akkit.org - http://www.akkit.org
// wifi_shared.h - Shared structures to be used by arm9 and arm7
/****************************************************************************** 
DSWifi Lib and test materials are licenced under the MIT open source licence:
Copyright (c) 2005-2006 Stephen Stair

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

#ifndef WIFI_SHARED_H
#define WIFI_SHARED_H

#include <nds.h>

#define WIFI_MAX_AP			32
#define WIFI_MAX_PROBE		4

#define WIFI_RXBUFFER_SIZE	(1024*12)
#define WIFI_TXBUFFER_SIZE	(1024*24)

typedef struct WIFI_MAINSTRUCT {
	unsigned long dummy1[8];
	// wifi status
	u16 curChannel, reqChannel;
	u16 curMode, reqMode;
	u16 authlevel,authctr;
	u32 flags9, flags7, reqPacketFlags;
	u16 curReqFlags, reqReqFlags;
	u32 counter7,bootcounter7;
	char MacAddr[6];
	u16 authtype;
	u16 iptype,ipflags;
	u32 ip,snmask,gateway;

	// current AP data
	char ssid7[34],ssid9[34];
	u8 bssid7[6], bssid9[6];
	u8 apmac7[6], apmac9[6];
	char wepmode7, wepmode9;
	char wepkeyid7, wepkeyid9;
	u8 wepkey7[20],wepkey9[20];
	u8 baserates7[16], baserates9[16];
	u8 apchannel7, apchannel9;
	u8 maxrate7;
	u16 ap_rssi;
	u16 pspoll_period;

	// AP data
	Wifi_AccessPoint aplist[WIFI_MAX_AP];

	// probe stuff
	u8 probe9_numprobe;
	u8 probe9_ssidlen[WIFI_MAX_PROBE];
	char probe9_ssid[WIFI_MAX_PROBE][32];

	// WFC data
	u8 wfc_enable[4]; // wep mode, or 0x80 for "enabled"
	Wifi_AccessPoint wfc_ap[3];
	unsigned long wfc_config[3][5]; // ip, snmask, gateway, primarydns, 2nddns
	u8 wfc_wepkey[3][16];
	

	// wifi data
	u32 rxbufIn, rxbufOut; // bufIn/bufOut have 2-byte granularity.
	u16 rxbufData[WIFI_RXBUFFER_SIZE/2]; // send raw 802.11 data through! rxbuffer is for rx'd data, arm7->arm9 transfer

	u32 txbufIn, txbufOut;
	u16 txbufData[WIFI_TXBUFFER_SIZE/2]; // tx buffer is for data to tx, arm9->arm7 transfer

	// stats data
	u32 stats[NUM_WIFI_STATS];
   
	u16 debug[30];

   u32 random; // semirandom number updated at the convenience of the arm7. use for initial seeds & such.

	unsigned long dummy2[8];

} Wifi_MainStruct;


#endif

