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
 
/*---------------------------------------------------------------------------------
	$Id: touch.c,v 1.19 2006/12/16 22:29:37 wntrmute Exp $

	Touch screen control for the ARM7

	Copyright (C) 2005
		Michael Noland (joat)
		Jason Rogers (dovoto)
		Dave Murphy (WinterMute)

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any
	damages arising from the use of this software.

	Permission is granted to anyone to use this software for any
	purpose, including commercial applications, and to alter it and
	redistribute it freely, subject to the following restrictions:

	1.	The origin of this software must not be misrepresented; you
			must not claim that you wrote the original software. If you use
			this software in a product, an acknowledgment in the product
			documentation would be appreciated but is not required.
	2.	Altered source versions must be plainly marked as such, and
			must not be misrepresented as being the original software.
	3.	This notice may not be removed or altered from any source
			distribution.

	$Log: touch.c,v $
	Revision 1.19  2006/12/16 22:29:37  wntrmute
	Incorporate Padrinator's touch code
	
	Revision 1.18  2006/04/04 23:05:19  wntrmute
	Added pressure reading to touchscreen function
	
	Revision 1.17  2006/02/21 23:50:10  wntrmute
	corrected build errors
	
	Revision 1.16  2006/02/21 20:25:49  dovoto
	Fixed some compilation errors (missing paranthesis and missing include)
	
	Revision 1.15  2006/02/21 00:28:32  wntrmute
	disable interrupts around touch screen reading
	
	Revision 1.14  2006/01/30 18:59:45  wntrmute
	improved touch code
	
	Revision 1.13  2006/01/12 11:13:55  wntrmute
	modified touch reading code from suggesrions found here -> http://forum.gbadev.org/viewtopic.php?t=7980
	
	Revision 1.12  2005/12/17 01:03:05  wntrmute
	corrected typos
	changed to median values
	
	Revision 1.11  2005/12/11 22:49:53  wntrmute
	use con for console device name
	
	Revision 1.10  2005/10/17 15:35:56  wntrmute
	use weighted averaging
	
	Revision 1.9  2005/10/03 21:19:34  wntrmute
	use ratiometric mode
	lock touchscreen on and average several readings
	
	Revision 1.8  2005/09/12 06:51:58  wntrmute
	tidied touch code
	
	Revision 1.7  2005/09/07 18:05:37  wntrmute
	use macros for device settings

	Revision 1.6  2005/08/23 17:06:10  wntrmute
	converted all endings to unix

	Revision 1.5  2005/08/01 23:12:17  wntrmute
	extended touchReadXY to return touchscreen co-ordinates as well as screen co-ordinates

	Revision 1.4  2005/07/29 00:57:40  wntrmute
	updated file headers
	added touchReadXY function
	made header C++ compatible

	Revision 1.3  2005/07/12 17:32:20  wntrmute
	updated file header

	Revision 1.2  2005/07/11 23:12:15  wntrmute
	*** empty log message ***

---------------------------------------------------------------------------------*/

#include <nds/jtypes.h>
#include <nds/system.h>
#include <nds/arm7/touch.h>
#include <nds/interrupts.h>

#include <stdlib.h>

static u8 last_time_touched = 0;

static u8 range_counter_1 = 0;
static u8 range_counter_2 = 0;
static u8 range = 20;
static u8 min_range = 20;

//---------------------------------------------------------------------------------
u8 CheckStylus(){
//---------------------------------------------------------------------------------

	SerialWaitBusy();

	REG_SPICNT = SPI_ENABLE | SPI_BAUD_2MHz | SPI_DEVICE_TOUCH | SPI_CONTINUOUS; //0x8A01;
	REG_SPIDATA = TSC_MEASURE_TEMP1;

	SerialWaitBusy();

	REG_SPIDATA = 0;

	SerialWaitBusy();

	REG_SPICNT = SPI_ENABLE | SPI_BAUD_2MHz | SPI_DEVICE_TOUCH;// 0x8201;
	REG_SPIDATA = 0;

	SerialWaitBusy();

	if(last_time_touched == 1){
		if( !(REG_KEYXY & 0x40) )
			return 1;
		else{
			REG_SPICNT = SPI_ENABLE | SPI_BAUD_2MHz | SPI_DEVICE_TOUCH | SPI_CONTINUOUS;
			REG_SPIDATA = TSC_MEASURE_TEMP1;

			SerialWaitBusy();

			REG_SPIDATA = 0;

			SerialWaitBusy();

			REG_SPICNT = SPI_ENABLE | SPI_BAUD_2MHz | SPI_DEVICE_TOUCH;
			REG_SPIDATA = 0;

			SerialWaitBusy();

			return !(REG_KEYXY & 0x40) ? 2 : 0;
		}
	}else{
		return !(REG_KEYXY & 0x40) ? 1 : 0;
	}
}

//---------------------------------------------------------------------------------
uint16 touchRead(uint32 command) {
//---------------------------------------------------------------------------------
	uint16 result, result2;

	uint32 oldIME = REG_IME;

	REG_IME = 0;
	
	SerialWaitBusy();

	// Write the command and wait for it to complete
	REG_SPICNT = SPI_ENABLE | SPI_BAUD_2MHz | SPI_DEVICE_TOUCH | SPI_CONTINUOUS; //0x8A01;
	REG_SPIDATA = command;
	SerialWaitBusy();

	// Write the second command and clock in part of the data
	REG_SPIDATA = 0;
	SerialWaitBusy();
	result = REG_SPIDATA;

	// Clock in the rest of the data (last transfer)
	REG_SPICNT = SPI_ENABLE | 0x201;
	REG_SPIDATA = 0;
	SerialWaitBusy();

	result2 = REG_SPIDATA >>3;

	REG_IME = oldIME;

	// Return the result
	return ((result & 0x7F) << 5) | result2;
}


//---------------------------------------------------------------------------------
uint32 touchReadTemperature(int * t1, int * t2) {
//---------------------------------------------------------------------------------
	*t1 = touchRead(TSC_MEASURE_TEMP1);
	*t2 = touchRead(TSC_MEASURE_TEMP2);
	return 8490 * (*t2 - *t1) - 273*4096;
}


static bool touchInit = false;
static s32 xscale, yscale;
static s32 xoffset, yoffset;

//---------------------------------------------------------------------------------
int16 readTouchValue(uint32 command, int16 *dist_max, u8 *err){
//---------------------------------------------------------------------------------
	int16 values[5];
	int32 aux1, aux2, aux3, dist, dist2, result = 0;
	u8 i, j, k;

	*err = 1;

	SerialWaitBusy();

	REG_SPICNT = SPI_ENABLE | SPI_BAUD_2MHz | SPI_DEVICE_TOUCH | SPI_CONTINUOUS;
	REG_SPIDATA = command;

	SerialWaitBusy();

	for(i=0; i<5; i++){
		REG_SPIDATA = 0;
		SerialWaitBusy();

		aux1 = REG_SPIDATA;
		aux1 = aux1 & 0xFF;
		aux1 = aux1 << 16;
		aux1 = aux1 >> 8;

		values[4-i] = aux1;

		REG_SPIDATA = command;
		SerialWaitBusy();

		aux1 = REG_SPIDATA;
		aux1 = aux1 & 0xFF;
		aux1 = aux1 << 16;

		aux1 = values[4-i] | (aux1 >> 16);
		values[4-i] = ((aux1 & 0x7FF8) >> 3);
	}

	REG_SPICNT = SPI_ENABLE | SPI_BAUD_2MHz | SPI_DEVICE_TOUCH;
	REG_SPIDATA = 0;
	SerialWaitBusy();

	dist = 0;
	for(i=0; i<4; i++){
		aux1 = values[i];

		for(j=i+1; j<5; j++){
			aux2 = values[j];
			aux2 = abs(aux1 - aux2);
			if(aux2>dist) dist = aux2;
		}
	}

	*dist_max = dist;

	for(i=0; i<3; i++){
		aux1 = values[i];

		for(j=i+1; j<4; j++){
			aux2 = values[j];
			dist = abs(aux1 - aux2);

			if( dist <= range ){
				for(k=j+1; k<5; k++){
					aux3 = values[k];
					dist2 = abs(aux1 - aux3);

					if( dist2 <= range ){
						result = aux2 + (aux1 << 1);
						result = result + aux3;
						result = result >> 2;
						result = result & (~7);

						*err = 0;

						break;
					}
				}
			}
		}
	}

	if((*err) == 1){
		result = values[0] + values[4];
		result = result >> 1;
		result = result & (~7);
	}

	return (result & 0xFFF);
}

//---------------------------------------------------------------------------------
void UpdateRange(uint8 *this_range, int16 last_dist_max, u8 data_error, u8 tsc_touched){
//---------------------------------------------------------------------------------
	//range_counter_1 = counter_0x380A98C
	//range_counter_2 = counter_0x380A990
	//Initial values:
	// range = 20
	// min_range = 20

	if(tsc_touched != 0){
		if( data_error == 0){
			range_counter_2 = 0;

			if( last_dist_max >= ((*this_range) >> 1)){
				range_counter_1 = 0;
			}else{
				range_counter_1++;

				if(range_counter_1 >= 4){
					range_counter_1 = 0;

					if((*this_range) > min_range){
						(*this_range)--;
						range_counter_2 = 3;
					}
				}
			}
		}else{
			range_counter_1 = 0;
			range_counter_2++;

			if(range_counter_2 >= 4){

				range_counter_2 = 0;

				if((*this_range) < 35){  //0x23 = 35
					*this_range = (*this_range) + 1;
				}
			}
		}
	}else{
		range_counter_2 = 0;
		range_counter_1 = 0;
	}
}

//---------------------------------------------------------------------------------
// reading pixel position:
//---------------------------------------------------------------------------------
touchPosition touchReadXY() {
//---------------------------------------------------------------------------------

	int16 dist_max_y, dist_max_x, dist_max;
	u8 error, error_where, first_check, i;

	touchPosition touchPos = { 0, 0, 0, 0 }; //, 0, 0 };

	if ( !touchInit ) {

		xscale = ((PersonalData->calX2px - PersonalData->calX1px) << 19) / ((PersonalData->calX2) - (PersonalData->calX1));
		yscale = ((PersonalData->calY2px - PersonalData->calY1px) << 19) / ((PersonalData->calY2) - (PersonalData->calY1));

		xoffset = ((PersonalData->calX1 + PersonalData->calX2) * xscale  - ((PersonalData->calX1px + PersonalData->calX2px) << 19) ) / 2;
		yoffset = ((PersonalData->calY1 + PersonalData->calY2) * yscale  - ((PersonalData->calY1px + PersonalData->calY2px) << 19) ) / 2;
		touchInit = true;
	}

	uint32 oldIME = REG_IME;

	REG_IME = 0;

	first_check = CheckStylus();
	if(first_check != 0){
		error_where = 0;

		readTouchValue(TSC_MEASURE_Z1 | 1, &dist_max, &error);
		readTouchValue(TSC_MEASURE_Z2 | 1, &dist_max, &error);

		touchPos.x = readTouchValue(TSC_MEASURE_X | 1, &dist_max_x, &error);
		if(error==1) error_where += 1;

		touchPos.y = readTouchValue(TSC_MEASURE_Y | 1, &dist_max_y, &error);
		if(error==1) error_where += 2;

		REG_SPICNT = SPI_ENABLE | SPI_BAUD_2MHz | SPI_DEVICE_TOUCH | SPI_CONTINUOUS;
		for(i=0; i<12; i++){
			REG_SPIDATA = 0;

			SerialWaitBusy();
		}

		REG_SPICNT = SPI_ENABLE | SPI_BAUD_2MHz | SPI_DEVICE_TOUCH;
		REG_SPIDATA = 0;

		SerialWaitBusy();

		if(first_check == 2) error_where = 3;

		switch( CheckStylus() ){
		case 0:
			last_time_touched = 0;
			break;
		case 1:
			last_time_touched = 1;

			if(dist_max_x > dist_max_y)
				dist_max = dist_max_x;
			else
				dist_max = dist_max_y;

			break;
		case 2:
			last_time_touched = 0;
			error_where = 3;

			break;
		}

		s16 px = ( touchPos.x * xscale - xoffset + xscale/2 ) >>19;
		s16 py = ( touchPos.y * yscale - yoffset + yscale/2 ) >>19;

		if ( px < 0) px = 0;
		if ( py < 0) py = 0;
		if ( px > (SCREEN_WIDTH -1)) px = SCREEN_WIDTH -1;
		if ( py > (SCREEN_HEIGHT -1)) py = SCREEN_HEIGHT -1;

		touchPos.px = px;
		touchPos.py = py;


	}else{
		error_where = 3;
		touchPos.x = 0;
		touchPos.y = 0;
		last_time_touched = 0;
	}

	UpdateRange(&range, dist_max, error_where, last_time_touched);

	REG_IME = oldIME;


	return touchPos;

}

