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
 
#ifndef _MIC_INCLUDED
#define _MIC_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#define SIO_DEVICE_POWER  (0 << 8)
#define SIO_DEVICE_TOUCH  (2 << 8)
#define SIO_BAUDRATE_2Mhz 1
#define SIO_BAUDRATE_1Mhz 2
#define SIO_CONTINUOUS    (1<<11)
#define PM_AMP_GAIN       3
#define PM_AMP_OFFSET     2
#define PM_AMP_ON         1
#define PM_AMP_OFF    	  0

#define GAIN_20           0
#define GAIN_40           1
#define GAIN_80           2
#define GAIN_160          3

void TurnOnMicrophone();
void TurnOffMicrophone();
u8 MIC_GetData8();
u16 MIC_ReadData12();
void PM_SetGain(u8 control);

#ifdef __cplusplus
}
#endif

#endif
