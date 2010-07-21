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
#include "microphone7.h"

/*
   microphone code based on neimod's microphone example.
   See: http://neimod.com/dstek/ 
   Chris Double (chris.double@double.co.nz)
   http://www.double.co.nz/nintendo_ds
*/

void PM_SetAmp(u8 control)
{
	SerialWaitBusy();
	
	REG_SPICNT = SPI_ENABLE | SIO_DEVICE_POWER | SIO_BAUDRATE_1Mhz | SIO_CONTINUOUS;
	REG_SPIDATA = PM_AMP_OFFSET;
	
	SerialWaitBusy();
	
	REG_SPICNT = SPI_ENABLE | SIO_DEVICE_POWER | SIO_BAUDRATE_1Mhz;
	REG_SPIDATA = control;
}

void TurnOnMicrophone()
{
	PM_SetAmp(PM_AMP_ON);
}

void TurnOffMicrophone()
{
	PM_SetAmp(PM_AMP_OFF);
}

void PM_SetGain(u8 control)
{
	SerialWaitBusy();
	
	REG_SPICNT = SPI_ENABLE | SIO_DEVICE_POWER | SIO_BAUDRATE_1Mhz | SIO_CONTINUOUS;
	REG_SPIDATA = PM_AMP_GAIN;
	
	SerialWaitBusy();
	
	REG_SPICNT = SPI_ENABLE | SIO_DEVICE_POWER | SIO_BAUDRATE_1Mhz;
	REG_SPIDATA = control;
}

u8 MIC_GetData8()
{
	static u16 result, result2;
	
	SerialWaitBusy();
	
	REG_SPICNT = SPI_ENABLE | SIO_DEVICE_TOUCH | SIO_BAUDRATE_2Mhz | SIO_CONTINUOUS;
	REG_SPIDATA = 0xEC;  // Touchscreen command format for AUX
  
	SerialWaitBusy();
	
	REG_SPIDATA = 0x00;
	
	SerialWaitBusy();
	
	result = REG_SPIDATA;
	REG_SPICNT = SPI_ENABLE | SIO_DEVICE_TOUCH | SIO_BAUDRATE_2Mhz;
	REG_SPIDATA = 0x00; 
	
	SerialWaitBusy();
	
	result2 = REG_SPIDATA;
	
	return (((result & 0x7F) << 1) | ((result2>>7) & 1));
}

// based off of tob's samples
u16 MIC_ReadData12()
{
	static u16 result, result2;

	SerialWaitBusy();

	REG_SPICNT = SPI_ENABLE | SIO_DEVICE_TOUCH | SPI_BAUD_2MHz | SPI_CONTINUOUS;
	REG_SPIDATA = 0xE4;  // Touchscreen command format for AUX, 12bit

	SerialWaitBusy();

	REG_SPIDATA = 0x00;

	SerialWaitBusy();

	result = REG_SPIDATA;
	REG_SPICNT = SPI_ENABLE | SIO_DEVICE_TOUCH | SPI_BAUD_2MHz;
	REG_SPIDATA = 0x00;

	SerialWaitBusy();

	result2 = REG_SPIDATA;

	return (((result & 0x7F) << 5) | ((result2>>3)&0x1F));
}
