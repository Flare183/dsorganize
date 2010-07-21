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
#include "font_arial_common.h"
#include <nds.h>
#include <stdio.h>

// Font name: 'Arial' size 9
// [ ] Bold
// [ ] Italics

// Width bias: -1, -2
// Loc bias: 0, 0
// Underline: 13

static uint16 fa9_A032[] = { 0xFFFF, 2, 13, 1, 0, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000 };

static uint16 fa9_A033[] = { 0xFFFF, 2, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2400, 0x4892, 0x0020 };

static uint16 fa9_A034[] = { 0xFFFF, 3, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0A00, 0x00AA, 0x0000, 0x0000 };

static uint16 fa9_A035[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0xA100, 0xCA5F, 0xFE14, 0x4251, 0x0080, 0x0000 };

static uint16 fa9_A036[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0xC501, 0x144A, 0x141C, 0x51A9, 0x00C1, 0x0000 };

static uint16 fa9_A037[] = { 0xFFFF, 10, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x8418, 0x94A0, 0x0113, 0x06AC, 0x4841, 0x0829, 0x00C0, 0x0000 };

static uint16 fa9_A038[] = { 0xFFFF, 7, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x1800, 0x2424, 0x3028, 0x444A, 0x3946, 0x0000 };

static uint16 fa9_A039[] = { 0xFFFF, 1, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x5001, 0x0000 };

static uint16 fa9_A040[] = { 0xFFFF, 3, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0100, 0x4422, 0x4244, 0x0021 };

static uint16 fa9_A041[] = { 0xFFFF, 3, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0800, 0x2244, 0x2422, 0x0048 };

static uint16 fa9_A042[] = { 0xFFFF, 4, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x914F, 0x0040, 0x0000, 0x0000 };

static uint16 fa9_A043[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0402, 0x103E, 0x0020, 0x0000, 0x0000 };

static uint16 fa9_A044[] = { 0xFFFF, 2, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x8024 };

static uint16 fa9_A045[] = { 0xFFFF, 3, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x00E0, 0x0000 };

static uint16 fa9_A046[] = { 0xFFFF, 2, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0020 };

static uint16 fa9_A047[] = { 0xFFFF, 2, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x1200, 0x4A92, 0x0040 };

static uint16 fa9_A048[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0xC401, 0x9148, 0x4422, 0x1189, 0x00C0, 0x0000 };

static uint16 fa9_A049[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x8300, 0x040A, 0x1008, 0x4020, 0x0080, 0x0000 };

static uint16 fa9_A050[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0xC401, 0x8140, 0x0804, 0x8320, 0x00E0, 0x0000 };

static uint16 fa9_A051[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0xC401, 0x8140, 0x040C, 0x1109, 0x00C0, 0x0000 };

static uint16 fa9_A052[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x4100, 0x0A83, 0x4814, 0x20F8, 0x0040, 0x0000 };

static uint16 fa9_A053[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0xE201, 0x1E08, 0x0422, 0x1109, 0x00C0, 0x0000 };

static uint16 fa9_A054[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0xC401, 0x1648, 0x4432, 0x1189, 0x00C0, 0x0000 };

static uint16 fa9_A055[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0xE003, 0x0481, 0x1008, 0x8140, 0x0000, 0x0000 };

static uint16 fa9_A056[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0xC401, 0x9148, 0x441C, 0x1189, 0x00C0, 0x0000 };

static uint16 fa9_A057[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0xC401, 0x9148, 0x3426, 0x1109, 0x00C0, 0x0000 };

static uint16 fa9_A058[] = { 0xFFFF, 2, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0080, 0x0020 };

static uint16 fa9_A059[] = { 0xFFFF, 2, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0080, 0x8024 };

static uint16 fa9_A060[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x8E00, 0x3820, 0x0008, 0x0000, 0x0000 };

static uint16 fa9_A061[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x3F00, 0x0100, 0x00F8, 0x0000, 0x0000 };

static uint16 fa9_A062[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0E08, 0x3802, 0x0080, 0x0000, 0x0000 };

static uint16 fa9_A063[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0xC401, 0x8148, 0x1004, 0x0020, 0x0080, 0x0000 };

static uint16 fa9_A064[] = { 0xFFFF, 11, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0700, 0x8681, 0x2220, 0x4969, 0x1195, 0x1551, 0x4F12, 0x01C2, 0x6010, 0x00F8 };

static uint16 fa9_A065[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x8200, 0x0A85, 0x7C22, 0x0C8A, 0x0010, 0x0000 };

static uint16 fa9_A066[] = { 0xFFFF, 7, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x7C00, 0x4242, 0x7E42, 0x4242, 0x7C42, 0x0000 };

static uint16 fa9_A067[] = { 0xFFFF, 8, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0300, 0x2282, 0x0009, 0x4080, 0x8820, 0x8083, 0x0000 };

static uint16 fa9_A068[] = { 0xFFFF, 8, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0F00, 0x2284, 0x0409, 0x4182, 0x9020, 0x808F, 0x0000 };

static uint16 fa9_A069[] = { 0xFFFF, 7, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x7E00, 0x4040, 0x7E40, 0x4040, 0x7E40, 0x0000 };

static uint16 fa9_A070[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0xE403, 0x1008, 0x403C, 0x0281, 0x0000, 0x0000 };

static uint16 fa9_A071[] = { 0xFFFF, 8, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0300, 0x2282, 0x0009, 0x418E, 0x8820, 0x8083, 0x0000 };

static uint16 fa9_A072[] = { 0xFFFF, 8, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0800, 0x1224, 0x0409, 0x41FE, 0x9020, 0x2048, 0x0000 };

static uint16 fa9_A073[] = { 0xFFFF, 2, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2400, 0x4992, 0x0020 };

static uint16 fa9_A074[] = { 0xFFFF, 5, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0802, 0x8220, 0x280A, 0x009C, 0x0000 };

static uint16 fa9_A075[] = { 0xFFFF, 7, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x4100, 0x4442, 0x5048, 0x4468, 0x4142, 0x0000 };

static uint16 fa9_A076[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0402, 0x1008, 0x4020, 0x0381, 0x00F0, 0x0000 };

static uint16 fa9_A077[] = { 0xFFFF, 8, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0800, 0x3326, 0x5419, 0x55AA, 0x922A, 0x2049, 0x0000 };

static uint16 fa9_A078[] = { 0xFFFF, 8, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0800, 0x1226, 0x4489, 0x4592, 0x9022, 0x20C8, 0x0000 };

static uint16 fa9_A079[] = { 0xFFFF, 8, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0300, 0x2282, 0x0409, 0x4182, 0x8820, 0x8083, 0x0000 };

static uint16 fa9_A080[] = { 0xFFFF, 7, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x7C00, 0x4242, 0x7C42, 0x4040, 0x4040, 0x0000 };

static uint16 fa9_A081[] = { 0xFFFF, 8, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0300, 0x2282, 0x0409, 0x4182, 0x8826, 0xA083, 0x0000 };

static uint16 fa9_A082[] = { 0xFFFF, 8, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0F00, 0x12C4, 0x0409, 0x44FC, 0x1021, 0x2088, 0x0000 };

static uint16 fa9_A083[] = { 0xFFFF, 7, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x3C00, 0x4242, 0x3C40, 0x4202, 0x3C42, 0x0000 };

static uint16 fa9_A084[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0xF107, 0x0402, 0x1008, 0x4020, 0x0080, 0x0000 };

static uint16 fa9_A085[] = { 0xFFFF, 8, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0800, 0x1224, 0x0409, 0x4182, 0x8820, 0x8083, 0x0000 };

static uint16 fa9_A086[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x1804, 0x9128, 0x2822, 0x4050, 0x0080, 0x0000 };

static uint16 fa9_A087[] = { 0xFFFF, 10, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x1842, 0x14A3, 0x9292, 0x518A, 0x284A, 0x1082, 0x0040, 0x0000 };

static uint16 fa9_A088[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x1404, 0x8A48, 0x2808, 0x1489, 0x0010, 0x0000 };

static uint16 fa9_A089[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x1404, 0x8A48, 0x1008, 0x4020, 0x0080, 0x0000 };

static uint16 fa9_A090[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0xF003, 0x0241, 0x2008, 0x0741, 0x00F0, 0x0000 };

static uint16 fa9_A091[] = { 0xFFFF, 2, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x3400, 0x4992, 0xC024 };

static uint16 fa9_A092[] = { 0xFFFF, 2, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x4800, 0x4892, 0x0090 };

static uint16 fa9_A093[] = { 0xFFFF, 2, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x6400, 0x4992, 0x8025 };

static uint16 fa9_A094[] = { 0xFFFF, 4, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x2945, 0x0051, 0x0000, 0x0000 };

static uint16 fa9_A095[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x1F00, 0x00C0 };

static uint16 fa9_A096[] = { 0xFFFF, 3, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0400, 0x0020, 0x0000, 0x0000 };

static uint16 fa9_A097[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x1107, 0x3C02, 0x3189, 0x00A0, 0x0000 };

static uint16 fa9_A098[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0402, 0x190B, 0x4422, 0x9289, 0x00C0, 0x0000 };

static uint16 fa9_A099[] = { 0xFFFF, 5, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x90C4, 0x0441, 0x008C, 0x0000 };

static uint16 fa9_A100[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x2000, 0x9346, 0x4422, 0x1189, 0x00E0, 0x0000 };

static uint16 fa9_A101[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x1107, 0x7C22, 0x1181, 0x00C0, 0x0000 };

static uint16 fa9_A102[] = { 0xFFFF, 2, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x1500, 0x49D2, 0x0020 };

static uint16 fa9_A103[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x9306, 0x4422, 0x3189, 0x4FA0, 0x0000 };

static uint16 fa9_A104[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0402, 0x190B, 0x4422, 0x1289, 0x0020, 0x0000 };

static uint16 fa9_A105[] = { 0xFFFF, 2, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2000, 0x4992, 0x0020 };

static uint16 fa9_A106[] = { 0xFFFF, 2, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2000, 0x4992, 0x0025 };

static uint16 fa9_A107[] = { 0xFFFF, 5, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x4110, 0x9414, 0x2471, 0x0091, 0x0000 };

static uint16 fa9_A108[] = { 0xFFFF, 2, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2400, 0x4992, 0x0020 };

static uint16 fa9_A109[] = { 0xFFFF, 10, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0xB300, 0x9219, 0x4422, 0x8948, 0x2211, 0x0020, 0x0000 };

static uint16 fa9_A110[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x190B, 0x4422, 0x1289, 0x0020, 0x0000 };

static uint16 fa9_A111[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x1107, 0x4422, 0x1189, 0x00C0, 0x0000 };

static uint16 fa9_A112[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x190B, 0x4422, 0x9289, 0x08C4, 0x0000 };

static uint16 fa9_A113[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x9306, 0x4422, 0x3189, 0x40A0, 0x0080 };

static uint16 fa9_A114[] = { 0xFFFF, 3, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x6405, 0x4444, 0x0000 };

static uint16 fa9_A115[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x1107, 0x3820, 0x1109, 0x00C0, 0x0000 };

static uint16 fa9_A116[] = { 0xFFFF, 2, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2500, 0x49D2, 0x0030 };

static uint16 fa9_A117[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x9108, 0x4422, 0x1189, 0x00E0, 0x0000 };

static uint16 fa9_A118[] = { 0xFFFF, 4, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x4600, 0x522A, 0x4088, 0x0000 };

static uint16 fa9_A119[] = { 0xFFFF, 8, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0400, 0x2246, 0x55AA, 0x882A, 0x4084, 0x0000 };

static uint16 fa9_A120[] = { 0xFFFF, 4, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x4500, 0x224A, 0x1095, 0x0000 };

static uint16 fa9_A121[] = { 0xFFFF, 4, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x4600, 0x522A, 0x4288, 0x0020 };

static uint16 fa9_A122[] = { 0xFFFF, 4, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x7C00, 0x2242, 0xF011, 0x0000 };

static uint16 fa9_A123[] = { 0xFFFF, 3, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0200, 0x4444, 0x4484, 0x0042 };

static uint16 fa9_A124[] = { 0xFFFF, 2, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2400, 0x4992, 0x8024 };

static uint16 fa9_A125[] = { 0xFFFF, 3, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0400, 0x2222, 0x2212, 0x0024 };

static uint16 fa9_A126[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x4C19, 0x0000, 0x0000, 0x0000 };

static uint16 fa9_A128[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0xF200, 0x3F08, 0xFEA0, 0x8080, 0x00F0, 0x0000 };

static uint16 fa9_A130[] = { 0xFFFF, 2, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x8024 };

static uint16 fa9_A131[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x7000, 0x8487, 0x1008, 0x8120, 0x1C02, 0x0000 };

static uint16 fa9_A132[] = { 0xFFFF, 3, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0A00, 0x00AA };

static uint16 fa9_A133[] = { 0xFFFF, 11, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x4404, 0x0000, 0x0000 };

static uint16 fa9_A134[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x8100, 0x840F, 0x1008, 0x4020, 0x0281, 0x0000 };

static uint16 fa9_A135[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x8100, 0x840F, 0x1008, 0x4320, 0x02E1, 0x0000 };

static uint16 fa9_A136[] = { 0xFFFF, 3, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0200, 0x0050, 0x0000, 0x0000 };

static uint16 fa9_A137[] = { 0xFFFF, 10, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0A24, 0x5081, 0x0014, 0x1480, 0x5445, 0x92AA, 0x0020, 0x0000 };

static uint16 fa9_A138[] = { 0xFFFF, 7, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x1028, 0x3C00, 0x4242, 0x3C40, 0x4202, 0x3C42, 0x0000 };

static uint16 fa9_A139[] = { 0xFFFF, 3, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0200, 0x4248, 0x0000 };

static uint16 fa9_A140[] = { 0xFFFF, 11, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0xBE03, 0x0446, 0x4220, 0x3E04, 0x0442, 0x4620, 0xBE03, 0x0000, 0x0000 };

static uint16 fa9_A142[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x1014, 0xF003, 0x0241, 0x2008, 0x0741, 0x00F0, 0x0000 };

static uint16 fa9_A145[] = { 0xFFFF, 2, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2400, 0x0080, 0x0000 };

static uint16 fa9_A146[] = { 0xFFFF, 2, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2400, 0x0080, 0x0000 };

static uint16 fa9_A147[] = { 0xFFFF, 3, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0A00, 0x00AA, 0x0000, 0x0000 };

static uint16 fa9_A148[] = { 0xFFFF, 3, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0500, 0x0055, 0x0000, 0x0000 };

static uint16 fa9_A149[] = { 0xFFFF, 3, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0xEE00, 0x00E0, 0x0000 };

static uint16 fa9_A150[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0xFE00, 0x0000, 0x0000, 0x0000 };

static uint16 fa9_A151[] = { 0xFFFF, 11, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xF0FF, 0x0000, 0x0000, 0x0000, 0x0000 };

static uint16 fa9_A152[] = { 0xFFFF, 3, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0500, 0x00A0, 0x0000, 0x0000 };

static uint16 fa9_A153[] = { 0xFFFF, 11, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0xA20F, 0x6223, 0x2336, 0x2A62, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 };

static uint16 fa9_A154[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x80A0, 0x1107, 0x3820, 0x1109, 0x00C0, 0x0000 };

static uint16 fa9_A155[] = { 0xFFFF, 3, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0400, 0x2421, 0x0000 };

static uint16 fa9_A156[] = { 0xFFFF, 10, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x7700, 0x1211, 0x4722, 0x81C8, 0x1D11, 0x00C0, 0x0000 };

static uint16 fa9_A158[] = { 0xFFFF, 4, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x1400, 0x7C40, 0x2242, 0xF011, 0x0000 };

static uint16 fa9_A159[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x5000, 0x1404, 0x8A48, 0x1008, 0x4020, 0x0080, 0x0000 };

static uint16 fa9_A160[] = { 0xFFFF, 2, 13, 1, 0, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000 };

static uint16 fa9_A161[] = { 0xFFFF, 2, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x4982, 0x8024 };

static uint16 fa9_A162[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x4000, 0x1387, 0x5028, 0x91A9, 0x04C2, 0x0000 };

static uint16 fa9_A163[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0xC401, 0x1048, 0x207C, 0xC441, 0x0060, 0x0000 };

static uint16 fa9_A164[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x8A0A, 0x2822, 0x00A8, 0x0000, 0x0000 };

static uint16 fa9_A165[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x1404, 0x0A45, 0x117F, 0x40FC, 0x0080, 0x0000 };

static uint16 fa9_A166[] = { 0xFFFF, 2, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2400, 0x0990, 0x8024 };

static uint16 fa9_A167[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x8401, 0x1C88, 0x844C, 0xE0C8, 0x8E48, 0x0000 };

static uint16 fa9_A168[] = { 0xFFFF, 3, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0A00, 0x0000, 0x0000, 0x0000 };

static uint16 fa9_A169[] = { 0xFFFF, 8, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0700, 0x14C4, 0x8BE6, 0xA241, 0x50CE, 0xC047, 0x0000 };

static uint16 fa9_A170[] = { 0xFFFF, 3, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0700, 0xF01F, 0x0000, 0x0000 };

static uint16 fa9_A171[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x280A, 0xA0A0, 0x00A0, 0x0000 };

static uint16 fa9_A172[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x3F00, 0x0402, 0x0008, 0x0000, 0x0000 };

static uint16 fa9_A173[] = { 0xFFFF, 3, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x00E0, 0x0000 };

static uint16 fa9_A174[] = { 0xFFFF, 8, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0700, 0x15C4, 0x8BE6, 0xA479, 0x50D1, 0xC047, 0x0000 };

static uint16 fa9_A175[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0300, 0x00F8, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 };

static uint16 fa9_A176[] = { 0xFFFF, 4, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x38E5, 0x0000, 0x0000, 0x0000 };

static uint16 fa9_A177[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0402, 0x103E, 0x0320, 0x00E0, 0x0000 };

static uint16 fa9_A178[] = { 0xFFFF, 3, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0F00, 0xF016, 0x0000, 0x0000 };

static uint16 fa9_A179[] = { 0xFFFF, 3, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0F00, 0xF021, 0x0000, 0x0000 };

static uint16 fa9_A180[] = { 0xFFFF, 3, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0200, 0x0040, 0x0000, 0x0000 };

static uint16 fa9_A181[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x9108, 0x4422, 0x1389, 0x08E4, 0x0000 };

static uint16 fa9_A182[] = { 0xFFFF, 5, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0xEB1F, 0x9AAE, 0xA228, 0x288A, 0x00A0 };

static uint16 fa9_A183[] = { 0xFFFF, 2, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0002, 0x0000 };

static uint16 fa9_A184[] = { 0xFFFF, 3, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x0021 };

static uint16 fa9_A185[] = { 0xFFFF, 3, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0200, 0x2062, 0x0000, 0x0000 };

static uint16 fa9_A186[] = { 0xFFFF, 3, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0600, 0x6099, 0x0000, 0x0000 };

static uint16 fa9_A187[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x2828, 0xA228, 0x0080, 0x0000 };

static uint16 fa9_A188[] = { 0xFFFF, 9, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x6184, 0x8208, 0x1040, 0x8408, 0x3C62, 0x0082, 0x0000 };

static uint16 fa9_A189[] = { 0xFFFF, 9, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x6184, 0x8208, 0x1040, 0x840B, 0x0812, 0x008F, 0x0000 };

static uint16 fa9_A190[] = { 0xFFFF, 9, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0300, 0x4182, 0x8E08, 0x1040, 0x8408, 0x3C61, 0x0082, 0x0000 };

static uint16 fa9_A191[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0002, 0x1008, 0x0241, 0x4724, 0x0000 };

static uint16 fa9_A192[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2020, 0x8200, 0x0A85, 0x7C22, 0x0C8A, 0x0010, 0x0000 };

static uint16 fa9_A193[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2008, 0x8200, 0x0A85, 0x7C22, 0x0C8A, 0x0010, 0x0000 };

static uint16 fa9_A194[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x5010, 0x8200, 0x0A85, 0x7C22, 0x0C8A, 0x0010, 0x0000 };

static uint16 fa9_A195[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x5014, 0x8200, 0x0A85, 0x7C22, 0x0C8A, 0x0010, 0x0000 };

static uint16 fa9_A196[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x5000, 0x8200, 0x0A85, 0x7C22, 0x0C8A, 0x0010, 0x0000 };

static uint16 fa9_A197[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x7000, 0xC2A1, 0x0A85, 0x7C22, 0x0C8A, 0x0010, 0x0000 };

static uint16 fa9_A198[] = { 0xFFFF, 11, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0xFE00, 0x0112, 0x2220, 0x3E02, 0x047E, 0x8220, 0x3E08, 0x0000, 0x0000 };

static uint16 fa9_A199[] = { 0xFFFF, 8, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0300, 0x2282, 0x0009, 0x4080, 0x8820, 0x8083, 0x2080 };

static uint16 fa9_A200[] = { 0xFFFF, 7, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0810, 0x7E00, 0x4040, 0x7E40, 0x4040, 0x7E40, 0x0000 };

static uint16 fa9_A201[] = { 0xFFFF, 7, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x1008, 0x7E00, 0x4040, 0x7E40, 0x4040, 0x7E40, 0x0000 };

static uint16 fa9_A202[] = { 0xFFFF, 7, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2810, 0x7E00, 0x4040, 0x7E40, 0x4040, 0x7E40, 0x0000 };

static uint16 fa9_A203[] = { 0xFFFF, 7, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x1400, 0x7E00, 0x4040, 0x7E40, 0x4040, 0x7E40, 0x0000 };

static uint16 fa9_A204[] = { 0xFFFF, 2, 13, 2, 32767, 0, // width, height, # of pallate entries, pallate entries
0xDB77, 0xB66D, 0xC0DF };

static uint16 fa9_A205[] = { 0xFFFF, 2, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2428, 0x4992, 0x0020 };

static uint16 fa9_A206[] = { 0xFFFF, 2, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2454, 0x4992, 0x0020 };

static uint16 fa9_A207[] = { 0xFFFF, 2, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2414, 0x4992, 0x0020 };

static uint16 fa9_A208[] = { 0xFFFF, 8, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0F00, 0x2284, 0x0509, 0x41F2, 0x9020, 0x808F, 0x0000 };

static uint16 fa9_A209[] = { 0xFFFF, 8, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0A0A, 0x0800, 0x1226, 0x4489, 0x4592, 0x9022, 0x20C8, 0x0000 };

static uint16 fa9_A210[] = { 0xFFFF, 8, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0410, 0x0300, 0x2282, 0x0409, 0x4182, 0x8820, 0x8083, 0x0000 };

static uint16 fa9_A211[] = { 0xFFFF, 8, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0404, 0x0300, 0x2282, 0x0409, 0x4182, 0x8820, 0x8083, 0x0000 };

static uint16 fa9_A212[] = { 0xFFFF, 8, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0A08, 0x0300, 0x2282, 0x0409, 0x4182, 0x8820, 0x8083, 0x0000 };

static uint16 fa9_A213[] = { 0xFFFF, 8, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0A0A, 0x0300, 0x2282, 0x0409, 0x4182, 0x8820, 0x8083, 0x0000 };

static uint16 fa9_A214[] = { 0xFFFF, 8, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0A00, 0x0300, 0x2282, 0x0409, 0x4182, 0x8820, 0x8083, 0x0000 };

static uint16 fa9_A215[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x8E08, 0x3808, 0x0088, 0x0000, 0x0000 };

static uint16 fa9_A216[] = { 0xFFFF, 8, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0300, 0x22A2, 0x1429, 0x5192, 0x8828, 0x808B, 0x0000 };

static uint16 fa9_A217[] = { 0xFFFF, 8, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0410, 0x0800, 0x1224, 0x0409, 0x4182, 0x8820, 0x8083, 0x0000 };

static uint16 fa9_A218[] = { 0xFFFF, 8, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0404, 0x0800, 0x1224, 0x0409, 0x4182, 0x8820, 0x8083, 0x0000 };

static uint16 fa9_A219[] = { 0xFFFF, 8, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0A08, 0x0800, 0x1224, 0x0409, 0x4182, 0x8820, 0x8083, 0x0000 };

static uint16 fa9_A220[] = { 0xFFFF, 8, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0A00, 0x0800, 0x1224, 0x0409, 0x4182, 0x8820, 0x8083, 0x0000 };

static uint16 fa9_A221[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2008, 0x1404, 0x8A48, 0x1008, 0x4020, 0x0080, 0x0000 };

static uint16 fa9_A222[] = { 0xFFFF, 7, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x4000, 0x7C40, 0x4242, 0x7C42, 0x4040, 0x0000 };

static uint16 fa9_A223[] = { 0xFFFF, 7, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x3800, 0x4444, 0x4848, 0x4244, 0x4C52, 0x0000 };

static uint16 fa9_A224[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x4040, 0x1107, 0x3C02, 0x3189, 0x00A0, 0x0000 };

static uint16 fa9_A225[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x8020, 0x1107, 0x3C02, 0x3189, 0x00A0, 0x0000 };

static uint16 fa9_A226[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x4041, 0x1107, 0x3C02, 0x3189, 0x00A0, 0x0000 };

static uint16 fa9_A227[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x4051, 0x1107, 0x3C02, 0x3189, 0x00A0, 0x0000 };

static uint16 fa9_A228[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x4001, 0x1107, 0x3C02, 0x3189, 0x00A0, 0x0000 };

static uint16 fa9_A229[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x7000, 0xC0A1, 0x1107, 0x3C02, 0x3189, 0x00A0, 0x0000 };

static uint16 fa9_A230[] = { 0xFFFF, 10, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x7700, 0x1011, 0x67E2, 0x81C8, 0x1D11, 0x00C0, 0x0000 };

static uint16 fa9_A231[] = { 0xFFFF, 5, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x90C4, 0x0441, 0x208C, 0x0040 };

static uint16 fa9_A232[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x8080, 0x1107, 0x7C22, 0x1181, 0x00C0, 0x0000 };

static uint16 fa9_A233[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x8020, 0x1107, 0x7C22, 0x1181, 0x00C0, 0x0000 };

static uint16 fa9_A234[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x4041, 0x1107, 0x7C22, 0x1181, 0x00C0, 0x0000 };

static uint16 fa9_A235[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x4001, 0x1107, 0x7C22, 0x1181, 0x00C0, 0x0000 };

static uint16 fa9_A236[] = { 0xFFFF, 2, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2002, 0x4992, 0x0020 };

static uint16 fa9_A237[] = { 0xFFFF, 2, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0xA000, 0x4992, 0x0020 };

static uint16 fa9_A238[] = { 0xFFFF, 2, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x5001, 0x4992, 0x0020 };

static uint16 fa9_A239[] = { 0xFFFF, 2, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x5000, 0x4992, 0x0020 };

static uint16 fa9_A240[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x4701, 0x1F81, 0x8542, 0x130A, 0x00C0, 0x0000 };

static uint16 fa9_A241[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x4051, 0x190B, 0x4422, 0x1289, 0x0020, 0x0000 };

static uint16 fa9_A242[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x8080, 0x1107, 0x4422, 0x1189, 0x00C0, 0x0000 };

static uint16 fa9_A243[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x8020, 0x1107, 0x4422, 0x1189, 0x00C0, 0x0000 };

static uint16 fa9_A244[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x4041, 0x1107, 0x4422, 0x1189, 0x00C0, 0x0000 };

static uint16 fa9_A245[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x4051, 0x1107, 0x4422, 0x1189, 0x00C0, 0x0000 };

static uint16 fa9_A246[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x4001, 0x1107, 0x4422, 0x1189, 0x00C0, 0x0000 };

static uint16 fa9_A247[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0002, 0x003E, 0x0020, 0x0000, 0x0000 };

static uint16 fa9_A248[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x9207, 0x5426, 0x93C8, 0x00C0, 0x0000 };

static uint16 fa9_A249[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x8080, 0x9108, 0x4422, 0x1189, 0x00E0, 0x0000 };

static uint16 fa9_A250[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x8020, 0x9108, 0x4422, 0x1189, 0x00E0, 0x0000 };

static uint16 fa9_A251[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x4041, 0x9108, 0x4422, 0x1189, 0x00E0, 0x0000 };

static uint16 fa9_A252[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x4001, 0x9108, 0x4422, 0x1189, 0x00E0, 0x0000 };

static uint16 fa9_A253[] = { 0xFFFF, 4, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0400, 0x4640, 0x522A, 0x4288, 0x0020 };

static uint16 fa9_A254[] = { 0xFFFF, 6, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0402, 0x110F, 0x4422, 0x1389, 0x08C4, 0x0000 };

static uint16 fa9_A255[] = { 0xFFFF, 4, 13, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x46A0, 0x522A, 0x4288, 0x0020 };

static uint16 fa9_TAB_STOP[] = { 25 };
static uint16 fa9_TRANSPARENT_VALUE[] = { 0 };
static uint16 fa9_UNDERLINE[] = { 13 };
static uint16 fa9_VERTBIAS[] = { 	0,		 2,		 2,		 2,		 2,		 2,		 2,  	 0,		 0,		 0,	
							0,		 0,		 0,		 0,		 2,		 2,		 2,  	 2,		 2,		 2,	
							0,		 0,		 0,		 0,		 0,		 0,		 0,  	 0,		 0,		 0,	
							0,		 0,		 0,		 0,		 0,		 0,		 0,  	 0,		 0,		 0,	
							0,		 0,		 0,		 0,		 0,		 0,		 0,  	 0,		 0,		 0,	
							0,		 0,		 0,		 0,		 0,		 0,		 0,  	 0,		 0,		 0,	
							0,		 0,		 0,		 0,		 0,		 0,		 0,  	 0,		 0,		 0,	
							0,		 0,		 0,		 0,		 0,		 0,		 0,  	 0,		 0,		 0,	
							0,		 0,		 0,		 0,		 0,		 0,		 0,  	 0,		 0,		 0,	
							0,		 0,		 0,		 0,		 0,		 0,		 0,  	 0,		 0,		 0,	
							0,		 0,		 0,		 0,		 0,		 0,		 0,  	 0,		 0,		 0,	
							0,		 0,		 0,		 0,		 0,		 0,		 0,  	 0,		 0,		 0,	
							0,		 0,		 0,		 0,		 0,		 0,		 0,  	 0,		 0,		 0,	
							0,		 0,		 0,		 0,		 0,		 0,		 0,  	 0,		 0,		 0,	
							0,		 0,		 0,		 0,		 0,		 0,		 0,  	 0,		 0,		 0,	
							0,		 0,		 0,		 0,		 0,		 0,		 0,  	 0,		 0,		 0,	
							0,		 0,		 0,		 0,		 0,		 0,		 0,  	 0,		 0,		 0,	
							0,		 0,		 0,		 0,		 0,		 0,		 0,  	 0,		 0,		 0,	
							0,		 0,		 0,		 0,		 0,		 0,		 0,  	 0,		 0,		 0,	
							0,		 0,		 0,		 0,		 0,		 0,		 0,  	 0,		 0,		 0,	
							0,		 0,		 0,		 0,		 0,		 0,		 0,  	 0,		 0,		 0,	
							0,		 0,		 0,		 0,		 0,		 0,		 0,  	 0,		 0,		 0,	
							0,		 0,		 0,		 0,		 0,		 0,		 0,  	 0,		 0,		 0,	
							0,		 0,		 0,		 0,		 0,		 0,		 0,  	 0,		 0,		 0,	
							0,		 0,		 0,		 0,		 0,		 0,		 0,  	 0,		 0,		 0,	
							0,		 0,		 0,		 0,		 0,		 0 };

uint16* font_arial_9[] = { 	0, X001, X002, X003, X004, X005, X006, 0, 0, 0,
							0, 0, 0, 0, X007, X008, X009, X010, X011, X012,
							X013, X014, 0, 0, 0, 0, 0, 0, 0, 0,
							0, 0, fa9_A032, fa9_A033, fa9_A034, fa9_A035, fa9_A036, fa9_A037, fa9_A038, fa9_A039,
							fa9_A040, fa9_A041, fa9_A042, fa9_A043, fa9_A044, fa9_A045, fa9_A046, fa9_A047, fa9_A048, fa9_A049,
							fa9_A050, fa9_A051, fa9_A052, fa9_A053, fa9_A054, fa9_A055, fa9_A056, fa9_A057, fa9_A058, fa9_A059,
							fa9_A060, fa9_A061, fa9_A062, fa9_A063, fa9_A064, fa9_A065, fa9_A066, fa9_A067, fa9_A068, fa9_A069,
							fa9_A070, fa9_A071, fa9_A072, fa9_A073, fa9_A074, fa9_A075, fa9_A076, fa9_A077, fa9_A078, fa9_A079,
							fa9_A080, fa9_A081, fa9_A082, fa9_A083, fa9_A084, fa9_A085, fa9_A086, fa9_A087, fa9_A088, fa9_A089,
							fa9_A090, fa9_A091, fa9_A092, fa9_A093, fa9_A094, fa9_A095, fa9_A096, fa9_A097, fa9_A098, fa9_A099,
							fa9_A100, fa9_A101, fa9_A102, fa9_A103, fa9_A104, fa9_A105, fa9_A106, fa9_A107, fa9_A108, fa9_A109,
							fa9_A110, fa9_A111, fa9_A112, fa9_A113, fa9_A114, fa9_A115, fa9_A116, fa9_A117, fa9_A118, fa9_A119,
							fa9_A120, fa9_A121, fa9_A122, fa9_A123, fa9_A124, fa9_A125, fa9_A126, 0, fa9_A128, 0,
							fa9_A130, fa9_A131, fa9_A132, fa9_A133, fa9_A134, fa9_A135, fa9_A136, fa9_A137, fa9_A138, fa9_A139,
							fa9_A140, 0, fa9_A142, 0, 0, fa9_A145, fa9_A146, fa9_A147, fa9_A148, fa9_A149,
							fa9_A150, fa9_A151, fa9_A152, fa9_A153, fa9_A154, fa9_A155, fa9_A156, 0, fa9_A158, fa9_A159,
							fa9_A160, fa9_A161, fa9_A162, fa9_A163, fa9_A164, fa9_A165, fa9_A166, fa9_A167, fa9_A168, fa9_A169,
							fa9_A170, fa9_A171, fa9_A172, fa9_A173, fa9_A174, fa9_A175, fa9_A176, fa9_A177, fa9_A178, fa9_A179,
							fa9_A180, fa9_A181, fa9_A182, fa9_A183, fa9_A184, fa9_A185, fa9_A186, fa9_A187, fa9_A188, fa9_A189,
							fa9_A190, fa9_A191, fa9_A192, fa9_A193, fa9_A194, fa9_A195, fa9_A196, fa9_A197, fa9_A198, fa9_A199,
							fa9_A200, fa9_A201, fa9_A202, fa9_A203, fa9_A204, fa9_A205, fa9_A206, fa9_A207, fa9_A208, fa9_A209,
							fa9_A210, fa9_A211, fa9_A212, fa9_A213, fa9_A214, fa9_A215, fa9_A216, fa9_A217, fa9_A218, fa9_A219,
							fa9_A220, fa9_A221, fa9_A222, fa9_A223, fa9_A224, fa9_A225, fa9_A226, fa9_A227, fa9_A228, fa9_A229,
							fa9_A230, fa9_A231, fa9_A232, fa9_A233, fa9_A234, fa9_A235, fa9_A236, fa9_A237, fa9_A238, fa9_A239,
							fa9_A240, fa9_A241, fa9_A242, fa9_A243, fa9_A244, fa9_A245, fa9_A246, fa9_A247, fa9_A248, fa9_A249,
							fa9_A250, fa9_A251, fa9_A252, fa9_A253, fa9_A254, fa9_A255, fa9_TAB_STOP, fa9_TRANSPARENT_VALUE, fa9_UNDERLINE, (uint16*)fa9_VERTBIAS };
