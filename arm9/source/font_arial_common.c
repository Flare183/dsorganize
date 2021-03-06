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

uint16 X001[] = { 0xFFFF, 10, 10, 3, 0, 15855, 32767, // width, height, # of pallate entries, pallate entries
0x5400, 0x1500, 0x0155, 0x5556, 0x5905, 0x5554, 0x5599, 0xA556, 0x6555, 0x4565, 0x9495, 0x5515, 0x1550, 0x0055, 0x4005, 0x0000 };

uint16 X002[] = { 0xFFFF, 10, 10, 3, 0, 15855, 32767, // width, height, # of pallate entries, pallate entries
0x5400, 0x1500, 0x0155, 0x5555, 0xAA05, 0x5654, 0x5556, 0xA55A, 0x6555, 0x4565, 0x54AA, 0x5515, 0x1550, 0x0055, 0x4005, 0x0000 };

uint16 X003[] = { 0xFFFF, 10, 10, 3, 0, 15855, 32767, // width, height, # of pallate entries, pallate entries
0x5400, 0x1500, 0x0155, 0x5555, 0x9505, 0x5594, 0x5599, 0x9555, 0x5955, 0x4595, 0x9495, 0x5515, 0x1550, 0x0055, 0x4005, 0x0000 };

uint16 X004[] = { 0xFFFF, 10, 10, 3, 0, 15855, 32767, // width, height, # of pallate entries, pallate entries
0x5400, 0x1500, 0x0155, 0x5555, 0x9505, 0x5594, 0x5599, 0x9555, 0x5655, 0x4555, 0x5459, 0x5515, 0x1550, 0x0055, 0x4005, 0x0000 };

uint16 X005[] = { 0xFFFF, 10, 10, 3, 0, 15855, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x1500, 0x4155, 0x5555, 0x5945, 0x5555, 0x5595, 0x5559, 0x5955, 0x4555, 0x5559, 0x5515, 0x1554, 0x4055, 0x0000, 0x0000 };

uint16 X006[] = { 0xFFFF, 10, 10, 3, 0, 15855, 32767, // width, height, # of pallate entries, pallate entries
0x5400, 0x1500, 0x0155, 0x5555, 0x5905, 0x1554, 0x5099, 0x5959, 0x5541, 0x0555, 0x5455, 0x5515, 0x5550, 0x4055, 0x5455, 0x0000 };

uint16 X007[] = { 0xFFFF, 10, 10, 3, 0, 15855, 32767, // width, height, # of pallate entries, pallate entries
0x5505, 0x5540, 0x4155, 0x5555, 0x5505, 0x1554, 0x5055, 0x5959, 0x5941, 0x0595, 0x5459, 0x5515, 0x1550, 0x0055, 0x4005, 0x0000 };

uint16 X008[] = { 0xFFFF, 10, 10, 3, 0, 15855, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x5500, 0x0555, 0x5555, 0x5915, 0x5554, 0x5559, 0x5955, 0x5555, 0x5595, 0x5459, 0x5555, 0x5550, 0x0055, 0x0000, 0x0000 };

uint16 X009[] = { 0xFFFF, 34, 10, 3, 0, 15855, 32767, // width, height, # of pallate entries, pallate entries
0x5500, 0x5555, 0x5555, 0x5555, 0x1554, 0x5555, 0x5555, 0x5555, 0x5155, 0x9656, 0x95A9, 0x5AAA, 0xA99A, 0x6545, 0x5699, 0x5556, 0x9695, 0x5555, 0x6A65, 0x5A59, 0x5596, 0x5559, 0x6555, 0x6595, 0x5965, 0x6555, 0x5951, 0x5566, 0x9595, 0x6565, 0x4595, 0x5A5A, 0xA6A6, 0x6AA9, 0x5556, 0x5505, 0x5555, 0x5555, 0x5555, 0x0154, 0x5555, 0x5555, 0x5555, 0x5055, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 };

uint16 X010[] = { 0xFFFF, 34, 10, 3, 15855, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x0100, 0x0054, 0x0000, 0x0000, 0x0000, 0x5000, 0x2A0A, 0x20A0, 0x8A2A, 0x00A8, 0x8240, 0x0008, 0x8080, 0x0082, 0x0001, 0x2080, 0x8208, 0x0802, 0x0000, 0x8000, 0x2A80, 0xA00A, 0x0020, 0x2000, 0x0282, 0x2002, 0x8020, 0x4000, 0x0828, 0x0808, 0x8280, 0x0100, 0x0000, 0x0000, 0x0000, 0x0000, 0x0014, 0x0000, 0x0000, 0x0000, 0x5505, 0x5555, 0x5555, 0x5555, 0x5555, 0x0040 };

uint16 X011[] = { 0xFFFF, 17, 10, 3, 0, 15855, 32767, // width, height, # of pallate entries, pallate entries
0x1500, 0x5055, 0x1500, 0x5555, 0x0550, 0x9555, 0x4055, 0x5955, 0x5455, 0x5515, 0x5595, 0x5551, 0x5559, 0x1555, 0x9555, 0x5555, 0x5A55, 0x5495, 0x5555, 0x5555, 0x5545, 0x5555, 0x5550, 0x5555, 0x0050 };

uint16 X012[] = { 0xFFFF, 17, 10, 3, 0, 15855, 32767, // width, height, # of pallate entries, pallate entries
0x5500, 0x4055, 0x5500, 0x5555, 0x1540, 0xA956, 0x0155, 0x6555, 0x5065, 0x5655, 0x5556, 0x5545, 0x956A, 0x5554, 0x5656, 0x4155, 0x6555, 0x5565, 0x5515, 0x5555, 0x5550, 0x5555, 0x0055, 0x5555, 0x5055 };

uint16 X013[] = { 0xFFFF, 10, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x3880, 0x830F, 0x00F8, 0x0000, 0x0000, 0x0000, 0x0000 };

uint16 X014[] = { 0xFFFF, 10, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x0000, 0xE00F, 0x0EF8, 0x8000, 0x0000 };

uint16 A183[] = { 0xFFFF, 4, 10, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x7B06, 0x00CC };
