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

// Font name: 'Arial' size 11
// [ ] Bold
// [ ] Italics

// Width bias: -1, 0
// Loc bias: 0, 0
// Underline: 15

static uint16 fa11_A032[] = { 0xFFFF, 3, 17, 1, 0, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A033[] = { 0xFFFF, 4, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x1042, 0x2184, 0x4008, 0x0010, 0x0000 };

static uint16 fa11_A034[] = { 0xFFFF, 4, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x29A5, 0x0040, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A035[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x1200, 0x1212, 0x24FF, 0x2424, 0x48FF, 0x4848, 0x0000, 0x0000 };

static uint16 fa11_A036[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x3C10, 0x9256, 0x5090, 0x1438, 0x9212, 0x3854, 0x0010, 0x0000 };

static uint16 fa11_A037[] = { 0xFFFF, 12, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x6000, 0x8884, 0x4124, 0x0924, 0x3220, 0x2460, 0x2481, 0x2011, 0x0889, 0x0030, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A038[] = { 0xFFFF, 9, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x2270, 0x8208, 0x5020, 0x1138, 0x4924, 0x430C, 0x308F, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A039[] = { 0xFFFF, 2, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2400, 0x0090, 0x0000, 0x0000 };

static uint16 fa11_A040[] = { 0xFFFF, 4, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x1122, 0x4208, 0x8410, 0x8430, 0x0010 };

static uint16 fa11_A041[] = { 0xFFFF, 4, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x1082, 0x1042, 0x2184, 0x8418, 0x0040 };

static uint16 fa11_A042[] = { 0xFFFF, 5, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0xF808, 0x1485, 0x0000, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A043[] = { 0xFFFF, 8, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x2040, 0x7F10, 0x0204, 0x0001, 0x0000, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A044[] = { 0xFFFF, 3, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x2202, 0x0000 };

static uint16 fa11_A045[] = { 0xFFFF, 4, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x003C, 0x0000, 0x0000 };

static uint16 fa11_A046[] = { 0xFFFF, 3, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x0002, 0x0000 };

static uint16 fa11_A047[] = { 0xFFFF, 3, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0100, 0x2212, 0x4444, 0x0088, 0x0000 };

static uint16 fa11_A048[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x3C00, 0x4242, 0x4242, 0x4242, 0x4242, 0x3C42, 0x0000, 0x0000 };

static uint16 fa11_A049[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0800, 0x2818, 0x0808, 0x0808, 0x0808, 0x0808, 0x0000, 0x0000 };

static uint16 fa11_A050[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x3C00, 0x4242, 0x0202, 0x0404, 0x1008, 0x7E20, 0x0000, 0x0000 };

static uint16 fa11_A051[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x3C00, 0x4242, 0x0202, 0x021C, 0x4202, 0x3C42, 0x0000, 0x0000 };

static uint16 fa11_A052[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0400, 0x140C, 0x2424, 0x8444, 0x04FE, 0x0404, 0x0000, 0x0000 };

static uint16 fa11_A053[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x3E00, 0x2020, 0x7C40, 0x0242, 0x4202, 0x3C44, 0x0000, 0x0000 };

static uint16 fa11_A054[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x3C00, 0x4242, 0x5C40, 0x4262, 0x4242, 0x3C42, 0x0000, 0x0000 };

static uint16 fa11_A055[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x7E00, 0x0402, 0x0804, 0x0808, 0x1010, 0x1010, 0x0000, 0x0000 };

static uint16 fa11_A056[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x3C00, 0x4242, 0x4242, 0x423C, 0x4242, 0x3C42, 0x0000, 0x0000 };

static uint16 fa11_A057[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x3C00, 0x4242, 0x4242, 0x3A46, 0x4202, 0x3C42, 0x0000, 0x0000 };

static uint16 fa11_A058[] = { 0xFFFF, 3, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x4000, 0x0000, 0x0004, 0x0000 };

static uint16 fa11_A059[] = { 0xFFFF, 3, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x4000, 0x0000, 0x4404, 0x0000 };

static uint16 fa11_A060[] = { 0xFFFF, 8, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x3808, 0x4060, 0x0318, 0x2080, 0x0000, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A061[] = { 0xFFFF, 8, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0xFC01, 0x0000, 0x1F00, 0x00C0, 0x0000, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A062[] = { 0xFFFF, 8, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0200, 0xE000, 0x010C, 0x0E03, 0x0008, 0x0000, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A063[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x3C00, 0x4242, 0x0402, 0x1008, 0x1010, 0x1000, 0x0000, 0x0000 };

static uint16 fa11_A064[] = { 0xFFFF, 14, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x013E, 0x0483, 0x1001, 0x26EB, 0x8832, 0x2025, 0x418A, 0x8214, 0x8C48, 0xEE88, 0x0110, 0x0C18, 0xE00F, 0x0000 };

static uint16 fa11_A065[] = { 0xFFFF, 8, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0100, 0x4001, 0x50A0, 0x2244, 0x9F20, 0x28C8, 0x040C, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A066[] = { 0xFFFF, 9, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0100, 0x40FC, 0x2490, 0x0409, 0x107F, 0x0924, 0x4002, 0xC09F, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A067[] = { 0xFFFF, 10, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x8307, 0x4008, 0x0290, 0x4000, 0x0108, 0x1000, 0x0823, 0x001E, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A068[] = { 0xFFFF, 10, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x843F, 0x8008, 0x1290, 0x4002, 0x0948, 0x2001, 0x0824, 0x00FE, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A069[] = { 0xFFFF, 9, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0100, 0x40FE, 0x0410, 0x0001, 0x907F, 0x0104, 0x4000, 0xE01F, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A070[] = { 0xFFFF, 8, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0F00, 0x02E4, 0x0001, 0x7E80, 0x1020, 0x0408, 0x0002, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A071[] = { 0xFFFF, 10, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x820F, 0x8008, 0x0290, 0x4300, 0x09C8, 0x2001, 0x0822, 0x003E, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A072[] = { 0xFFFF, 9, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0100, 0x4002, 0x2490, 0x0209, 0x907F, 0x0924, 0x4002, 0x2090, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A073[] = { 0xFFFF, 2, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2400, 0x4992, 0x8024, 0x0000 };

static uint16 fa11_A074[] = { 0xFFFF, 6, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x2000, 0x8140, 0x0402, 0x1408, 0x4F28, 0x0000, 0x0000 };

static uint16 fa11_A075[] = { 0xFFFF, 9, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0100, 0x4102, 0x8410, 0x2041, 0x1854, 0x2184, 0x4004, 0x1090, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A076[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x4000, 0x4040, 0x4040, 0x4040, 0x4040, 0x7F40, 0x0000, 0x0000 };

static uint16 fa11_A077[] = { 0xFFFF, 10, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x2620, 0xC10C, 0x5294, 0x518A, 0x4949, 0x2529, 0x4424, 0x8088, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A078[] = { 0xFFFF, 9, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0100, 0x6002, 0x2594, 0x2209, 0x9148, 0x2924, 0x410A, 0x2090, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A079[] = { 0xFFFF, 11, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0xF000, 0xC230, 0x4004, 0x0224, 0x2440, 0x4002, 0x0422, 0xC030, 0x00F0, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A080[] = { 0xFFFF, 9, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0100, 0x40FC, 0x2490, 0x0209, 0x107F, 0x0104, 0x4000, 0x0010, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A081[] = { 0xFFFF, 11, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0xF000, 0xC230, 0x4004, 0x0224, 0x2440, 0x4002, 0x3422, 0xC030, 0x00F2, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A082[] = { 0xFFFF, 10, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0xC43F, 0x8004, 0x1290, 0x7F02, 0x4188, 0x2004, 0x0884, 0x8080, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A083[] = { 0xFFFF, 9, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x2178, 0x2410, 0xE001, 0x0007, 0x0920, 0x6102, 0x8007, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A084[] = { 0xFFFF, 8, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x1F00, 0x80F0, 0x2040, 0x0810, 0x0204, 0x0001, 0x4080, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A085[] = { 0xFFFF, 9, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0100, 0x4002, 0x2490, 0x0209, 0x9040, 0x0924, 0x2102, 0x8007, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A086[] = { 0xFFFF, 8, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x1000, 0x0A18, 0x0409, 0x2244, 0x0511, 0x8002, 0x4080, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A087[] = { 0xFFFF, 14, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0400, 0x1808, 0x2828, 0x9050, 0x22A1, 0x4422, 0x4844, 0xA090, 0x41A1, 0x0141, 0x0202, 0x0000, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A088[] = { 0xFFFF, 8, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x1000, 0x1114, 0x8810, 0x0828, 0x080A, 0x4484, 0x0414, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A089[] = { 0xFFFF, 8, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x1000, 0x1114, 0x8810, 0x0828, 0x0204, 0x0001, 0x4080, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A090[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x7F00, 0x0402, 0x0804, 0x1010, 0x2020, 0xFF40, 0x0000, 0x0000 };

static uint16 fa11_A091[] = { 0xFFFF, 3, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0700, 0x4444, 0x4444, 0x4444, 0x0070 };

static uint16 fa11_A092[] = { 0xFFFF, 3, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0800, 0x4484, 0x2222, 0x0011, 0x0000 };

static uint16 fa11_A093[] = { 0xFFFF, 3, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0E00, 0x2222, 0x2222, 0x2222, 0x00E0 };

static uint16 fa11_A094[] = { 0xFFFF, 4, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x2945, 0x8851, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A095[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x00FF };

static uint16 fa11_A096[] = { 0xFFFF, 4, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0082, 0x0000, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A097[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x423C, 0x1E02, 0x4222, 0x3A46, 0x0000, 0x0000 };

static uint16 fa11_A098[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x4000, 0x4040, 0x625C, 0x4242, 0x4242, 0x5C62, 0x0000, 0x0000 };

static uint16 fa11_A099[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x423C, 0x4040, 0x4040, 0x3C42, 0x0000, 0x0000 };

static uint16 fa11_A100[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0200, 0x0202, 0x463A, 0x4242, 0x4242, 0x3A46, 0x0000, 0x0000 };

static uint16 fa11_A101[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x423C, 0x7E42, 0x4040, 0x3C42, 0x0000, 0x0000 };

static uint16 fa11_A102[] = { 0xFFFF, 3, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0300, 0xF444, 0x4444, 0x0044, 0x0000 };

static uint16 fa11_A103[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x463A, 0x4242, 0x4242, 0x3A46, 0x4202, 0x003C };

static uint16 fa11_A104[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x4000, 0x4040, 0x625C, 0x4242, 0x4242, 0x4242, 0x0000, 0x0000 };

static uint16 fa11_A105[] = { 0xFFFF, 2, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2000, 0x4912, 0x8024, 0x0000 };

static uint16 fa11_A106[] = { 0xFFFF, 2, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2000, 0x4912, 0x9224, 0x0080 };

static uint16 fa11_A107[] = { 0xFFFF, 6, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0402, 0x1008, 0x48A2, 0xA2A1, 0x4824, 0x0040, 0x0000 };

static uint16 fa11_A108[] = { 0xFFFF, 2, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2400, 0x4992, 0x8024, 0x0000 };

static uint16 fa11_A109[] = { 0xFFFF, 12, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x0000, 0x0100, 0x8C73, 0x4262, 0x1012, 0x8490, 0x2484, 0x2121, 0x0008, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A110[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x625C, 0x4242, 0x4242, 0x4242, 0x0000, 0x0000 };

static uint16 fa11_A111[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x423C, 0x4242, 0x4242, 0x3C42, 0x0000, 0x0000 };

static uint16 fa11_A112[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x625C, 0x4242, 0x4242, 0x5C62, 0x4040, 0x0040 };

static uint16 fa11_A113[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x463A, 0x4242, 0x4242, 0x3A46, 0x0202, 0x0002 };

static uint16 fa11_A114[] = { 0xFFFF, 4, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0100, 0x426C, 0x8410, 0x0020, 0x0000 };

static uint16 fa11_A115[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x423C, 0x3040, 0x020C, 0x3C42, 0x0000, 0x0000 };

static uint16 fa11_A116[] = { 0xFFFF, 3, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0xF444, 0x4444, 0x0047, 0x0000 };

static uint16 fa11_A117[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x4242, 0x4242, 0x4242, 0x3A46, 0x0000, 0x0000 };

static uint16 fa11_A118[] = { 0xFFFF, 6, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x2000, 0x44C1, 0xA188, 0x0241, 0x0000, 0x0000 };

static uint16 fa11_A119[] = { 0xFFFF, 10, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x0000, 0x0C21, 0x4A51, 0x4949, 0x2845, 0x08A2, 0x0041, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A120[] = { 0xFFFF, 6, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x2000, 0x28A2, 0x4120, 0x5044, 0x0040, 0x0000 };

static uint16 fa11_A121[] = { 0xFFFF, 6, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x2000, 0x44C1, 0xA188, 0x8242, 0x0804, 0x0060 };

static uint16 fa11_A122[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x047E, 0x0808, 0x1010, 0x7E20, 0x0000, 0x0000 };

static uint16 fa11_A123[] = { 0xFFFF, 4, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x1032, 0x2184, 0x4230, 0x8410, 0x0018 };

static uint16 fa11_A124[] = { 0xFFFF, 2, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2400, 0x4992, 0x9224, 0x0040 };

static uint16 fa11_A125[] = { 0xFFFF, 4, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0100, 0x1082, 0x2184, 0x4206, 0x8410, 0x00C0 };

static uint16 fa11_A126[] = { 0xFFFF, 8, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x0000, 0x7100, 0x0047, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A128[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x1E00, 0x4020, 0xFE40, 0xFE40, 0x4040, 0x1E20, 0x0000, 0x0000 };

static uint16 fa11_A130[] = { 0xFFFF, 2, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x9200, 0x0000 };

static uint16 fa11_A131[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0E00, 0x0808, 0x103C, 0x1010, 0x1010, 0x2020, 0x2020, 0x00E0 };

static uint16 fa11_A132[] = { 0xFFFF, 4, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x0000, 0x9452, 0x0000 };

static uint16 fa11_A133[] = { 0xFFFF, 14, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x4404, 0x0000, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A134[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0800, 0x0808, 0x087F, 0x0808, 0x0808, 0x0808, 0x0808, 0x0008 };

static uint16 fa11_A135[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0800, 0x0808, 0x087F, 0x0808, 0x0808, 0x7F08, 0x0808, 0x0008 };

static uint16 fa11_A136[] = { 0xFFFF, 4, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x8064, 0x0000, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A137[] = { 0xFFFF, 13, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x1800, 0x9480, 0x5002, 0x4009, 0x0026, 0x186B, 0x9132, 0x454A, 0x1429, 0x8CA4, 0x0060, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A138[] = { 0xFFFF, 9, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0312, 0x0000, 0x2178, 0x2410, 0xE001, 0x0007, 0x0920, 0x6102, 0x8007, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A139[] = { 0xFFFF, 4, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x2244, 0x4108, 0x0000, 0x0000 };

static uint16 fa11_A140[] = { 0xFFFF, 14, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0xE2F7, 0x0818, 0x1010, 0x2020, 0x4040, 0x81FC, 0x0201, 0x0402, 0x1802, 0xDF03, 0x0080, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A142[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0C12, 0x7F00, 0x0402, 0x0804, 0x1010, 0x2020, 0xFF40, 0x0000, 0x0000 };

static uint16 fa11_A145[] = { 0xFFFF, 2, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2400, 0x0080, 0x0000, 0x0000 };

static uint16 fa11_A146[] = { 0xFFFF, 2, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2400, 0x0080, 0x0000, 0x0000 };

static uint16 fa11_A147[] = { 0xFFFF, 4, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0100, 0x504A, 0x0000, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A148[] = { 0xFFFF, 4, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x28A5, 0x0000, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A149[] = { 0xFFFF, 4, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x7B06, 0x00CC, 0x0000, 0x0000 };

static uint16 fa11_A150[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x00FF, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A151[] = { 0xFFFF, 14, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFF03, 0x00F8, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A152[] = { 0xFFFF, 3, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0500, 0x00A0, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A153[] = { 0xFFFF, 14, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0300, 0x61EC, 0xC218, 0x842A, 0x0855, 0x11AA, 0x0024, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A154[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x2400, 0x0018, 0x423C, 0x3040, 0x020C, 0x3C42, 0x0000, 0x0000 };

static uint16 fa11_A155[] = { 0xFFFF, 4, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0100, 0x2004, 0x4488, 0x0000, 0x0000 };

static uint16 fa11_A156[] = { 0xFFFF, 13, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xCE03, 0xC410, 0x0942, 0xE40F, 0x1020, 0x4382, 0xF310, 0x0080, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A158[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x2400, 0x0018, 0x047E, 0x0808, 0x1010, 0x7E20, 0x0000, 0x0000 };

static uint16 fa11_A159[] = { 0xFFFF, 8, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0A00, 0x1000, 0x1114, 0x8810, 0x0828, 0x0204, 0x0001, 0x4080, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A160[] = { 0xFFFF, 3, 17, 1, 0, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A161[] = { 0xFFFF, 4, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x2180, 0x4208, 0x8410, 0x0020 };

static uint16 fa11_A162[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0400, 0x0404, 0x2A1C, 0x4848, 0x5250, 0x1C76, 0x2020, 0x0020 };

static uint16 fa11_A163[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x3C00, 0x4042, 0x4040, 0x20F8, 0x4020, 0x8E70, 0x0000, 0x0000 };

static uint16 fa11_A164[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x245A, 0x4242, 0x5A24, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A165[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x8200, 0x4444, 0x2828, 0x10FE, 0x10FE, 0x1010, 0x0000, 0x0000 };

static uint16 fa11_A166[] = { 0xFFFF, 2, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2400, 0x0092, 0x9224, 0x0040 };

static uint16 fa11_A167[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x3800, 0x4444, 0x7060, 0x869C, 0x72C2, 0x0C1C, 0xC484, 0x0078 };

static uint16 fa11_A168[] = { 0xFFFF, 4, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x00A0, 0x0000, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A169[] = { 0xFFFF, 10, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0xC41F, 0x1C05, 0x4C64, 0x9081, 0x2632, 0xC038, 0x0414, 0x007F, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A170[] = { 0xFFFF, 4, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x9FE8, 0x7831, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A171[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x2412, 0x4824, 0x2424, 0x0012, 0x0000, 0x0000 };

static uint16 fa11_A172[] = { 0xFFFF, 8, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x0000, 0x01FE, 0x8000, 0x0040, 0x0000, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A173[] = { 0xFFFF, 4, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x003C, 0x0000, 0x0000 };

static uint16 fa11_A174[] = { 0xFFFF, 10, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0xC41F, 0x3C05, 0x4C64, 0x9E89, 0x4632, 0xC044, 0x0414, 0x007F, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A175[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x00FF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A176[] = { 0xFFFF, 5, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x490C, 0x0023, 0x0000, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A177[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0800, 0x0808, 0x087F, 0x0808, 0x7F00, 0x0000, 0x0000 };

static uint16 fa11_A178[] = { 0xFFFF, 4, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x84E8, 0xF8C8, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A179[] = { 0xFFFF, 4, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x88F8, 0x7031, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A180[] = { 0xFFFF, 4, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0022, 0x0000, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A181[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x4242, 0x4242, 0x4242, 0x5A66, 0x4040, 0x0040 };

static uint16 fa11_A182[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x7F00, 0xF2F2, 0xF2F2, 0x1272, 0x1212, 0x1212, 0x1212, 0x0012 };

static uint16 fa11_A183[] = { 0xFFFF, 3, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0040, 0x0000, 0x0000 };

static uint16 fa11_A184[] = { 0xFFFF, 4, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x0000, 0x8200, 0x0070 };

static uint16 fa11_A185[] = { 0xFFFF, 4, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0865, 0x1042, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A186[] = { 0xFFFF, 4, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0xC6E8, 0x7031, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A187[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x2448, 0x1224, 0x2424, 0x0048, 0x0000, 0x0000 };

static uint16 fa11_A188[] = { 0xFFFF, 12, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x6000, 0x0445, 0x2008, 0x0242, 0x1220, 0x3110, 0x1481, 0x2111, 0x900F, 0x0008, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A189[] = { 0xFFFF, 12, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x6000, 0x0445, 0x4008, 0x0244, 0x1220, 0x2238, 0x0242, 0x2110, 0x1002, 0x003C, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A190[] = { 0xFFFF, 12, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0xE000, 0x0449, 0x4010, 0x1242, 0x6220, 0x2110, 0x1481, 0x2111, 0x900F, 0x0008, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A191[] = { 0xFFFF, 8, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x2000, 0x0800, 0x0204, 0x0202, 0x0102, 0x8604, 0x003E, 0x0000 };

static uint16 fa11_A192[] = { 0xFFFF, 8, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0410, 0x0100, 0x4001, 0x50A0, 0x2244, 0x9F20, 0x28C8, 0x040C, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A193[] = { 0xFFFF, 8, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0404, 0x0100, 0x4001, 0x50A0, 0x2244, 0x9F20, 0x28C8, 0x040C, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A194[] = { 0xFFFF, 8, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x090C, 0x0100, 0x4001, 0x50A0, 0x2244, 0x9F20, 0x28C8, 0x040C, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A195[] = { 0xFFFF, 8, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0A0A, 0x0100, 0x4001, 0x50A0, 0x2244, 0x9F20, 0x28C8, 0x040C, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A196[] = { 0xFFFF, 8, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0A00, 0x0100, 0x4001, 0x50A0, 0x2244, 0x9F20, 0x28C8, 0x040C, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A197[] = { 0xFFFF, 8, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0E00, 0x0305, 0x4081, 0x50A0, 0x2244, 0x9F20, 0x28C8, 0x040C, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A198[] = { 0xFFFF, 14, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0xE03F, 0x0190, 0x0420, 0x0840, 0x2180, 0x42FC, 0xFC01, 0x0802, 0x1008, 0x3F10, 0x0080, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A199[] = { 0xFFFF, 10, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x8307, 0x4008, 0x0290, 0x4000, 0x0108, 0x1000, 0x0823, 0x011E, 0x1000, 0x000E, 0x0000 };

static uint16 fa11_A200[] = { 0xFFFF, 9, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0108, 0x0100, 0x40FE, 0x0410, 0x0001, 0x907F, 0x0104, 0x4000, 0xE01F, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A201[] = { 0xFFFF, 9, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0204, 0x0100, 0x40FE, 0x0410, 0x0001, 0x907F, 0x0104, 0x4000, 0xE01F, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A202[] = { 0xFFFF, 9, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x040C, 0x0180, 0x40FE, 0x0410, 0x0001, 0x907F, 0x0104, 0x4000, 0xE01F, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A203[] = { 0xFFFF, 9, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0200, 0x0180, 0x40FE, 0x0410, 0x0001, 0x907F, 0x0104, 0x4000, 0xE01F, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A204[] = { 0xFFFF, 2, 17, 2, 32767, 0, // width, height, # of pallate entries, pallate entries
0xDB77, 0xB66D, 0x7FDB, 0x00FC };

static uint16 fa11_A205[] = { 0xFFFF, 2, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2428, 0x4992, 0x8024, 0x0000 };

static uint16 fa11_A206[] = { 0xFFFF, 2, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2470, 0x4992, 0x8024, 0x0000 };

static uint16 fa11_A207[] = { 0xFFFF, 2, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2414, 0x4992, 0x8024, 0x0000 };

static uint16 fa11_A208[] = { 0xFFFF, 10, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x843F, 0x8008, 0x1290, 0xFC02, 0x0948, 0x2001, 0x0824, 0x00FE, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A209[] = { 0xFFFF, 9, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x050A, 0x0100, 0x6002, 0x2594, 0x2209, 0x9148, 0x2924, 0x410A, 0x2090, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A210[] = { 0xFFFF, 11, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0004, 0x0020, 0xF000, 0xC230, 0x4004, 0x0224, 0x2440, 0x4002, 0x0422, 0xC030, 0x00F0, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A211[] = { 0xFFFF, 11, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0002, 0x0040, 0xF000, 0xC230, 0x4004, 0x0224, 0x2440, 0x4002, 0x0422, 0xC030, 0x00F0, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A212[] = { 0xFFFF, 11, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0006, 0x0090, 0xF000, 0xC230, 0x4004, 0x0224, 0x2440, 0x4002, 0x0422, 0xC030, 0x00F0, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A213[] = { 0xFFFF, 11, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0005, 0x00A0, 0xF000, 0xC230, 0x4004, 0x0224, 0x2440, 0x4002, 0x0422, 0xC030, 0x00F0, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A214[] = { 0xFFFF, 11, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0050, 0xF000, 0xC230, 0x4004, 0x0224, 0x2440, 0x4002, 0x0422, 0xC030, 0x00F0, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A215[] = { 0xFFFF, 8, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0200, 0x8808, 0x0838, 0x080E, 0x2088, 0x0000, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A216[] = { 0xFFFF, 11, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0xF200, 0xC230, 0x410C, 0x2224, 0x2444, 0x4842, 0x0423, 0xC430, 0x00F0, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A217[] = { 0xFFFF, 9, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0108, 0x0100, 0x4002, 0x2490, 0x0209, 0x9040, 0x0924, 0x2102, 0x8007, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A218[] = { 0xFFFF, 9, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0204, 0x0100, 0x4002, 0x2490, 0x0209, 0x9040, 0x0924, 0x2102, 0x8007, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A219[] = { 0xFFFF, 9, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x040C, 0x0180, 0x4002, 0x2490, 0x0209, 0x9040, 0x0924, 0x2102, 0x8007, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A220[] = { 0xFFFF, 9, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0200, 0x0180, 0x4002, 0x2490, 0x0209, 0x9040, 0x0924, 0x2102, 0x8007, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A221[] = { 0xFFFF, 8, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0404, 0x1000, 0x1114, 0x8810, 0x0828, 0x0204, 0x0001, 0x4080, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A222[] = { 0xFFFF, 9, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0100, 0x4000, 0xC41F, 0x0219, 0x9040, 0x1124, 0x40F8, 0x0010, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A223[] = { 0xFFFF, 8, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0300, 0x4202, 0x1021, 0x4890, 0x1023, 0x2548, 0x7012, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A224[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x1000, 0x0008, 0x423C, 0x1E02, 0x4222, 0x3A46, 0x0000, 0x0000 };

static uint16 fa11_A225[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0800, 0x0010, 0x423C, 0x1E02, 0x4222, 0x3A46, 0x0000, 0x0000 };

static uint16 fa11_A226[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x1800, 0x0024, 0x423C, 0x1E02, 0x4222, 0x3A46, 0x0000, 0x0000 };

static uint16 fa11_A227[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x1400, 0x0028, 0x423C, 0x1E02, 0x4222, 0x3A46, 0x0000, 0x0000 };

static uint16 fa11_A228[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0014, 0x423C, 0x1E02, 0x4222, 0x3A46, 0x0000, 0x0000 };

static uint16 fa11_A229[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x141C, 0x001C, 0x423C, 0x1E02, 0x4222, 0x3A46, 0x0000, 0x0000 };

static uint16 fa11_A230[] = { 0xFFFF, 12, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x88F3, 0x0262, 0x7F08, 0x80CC, 0x1484, 0x1C71, 0x0070, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A231[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x423C, 0x4040, 0x4040, 0x3C42, 0x0810, 0x0038 };

static uint16 fa11_A232[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x1000, 0x0008, 0x423C, 0x7E42, 0x4040, 0x3C42, 0x0000, 0x0000 };

static uint16 fa11_A233[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0800, 0x0010, 0x423C, 0x7E42, 0x4040, 0x3C42, 0x0000, 0x0000 };

static uint16 fa11_A234[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x1800, 0x0024, 0x423C, 0x7E42, 0x4040, 0x3C42, 0x0000, 0x0000 };

static uint16 fa11_A235[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0014, 0x423C, 0x7E42, 0x4040, 0x3C42, 0x0000, 0x0000 };

static uint16 fa11_A236[] = { 0xFFFF, 2, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x4400, 0x4912, 0x8024, 0x0000 };

static uint16 fa11_A237[] = { 0xFFFF, 2, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x1400, 0x4912, 0x8024, 0x0000 };

static uint16 fa11_A238[] = { 0xFFFF, 2, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x3800, 0x4912, 0x8024, 0x0000 };

static uint16 fa11_A239[] = { 0xFFFF, 2, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0A00, 0x4912, 0x8024, 0x0000 };

static uint16 fa11_A240[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x1400, 0x2418, 0x261C, 0x4242, 0x4242, 0x1824, 0x0000, 0x0000 };

static uint16 fa11_A241[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x1400, 0x0028, 0x625C, 0x4242, 0x4242, 0x4242, 0x0000, 0x0000 };

static uint16 fa11_A242[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x1000, 0x0008, 0x423C, 0x4242, 0x4242, 0x3C42, 0x0000, 0x0000 };

static uint16 fa11_A243[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0800, 0x0010, 0x423C, 0x4242, 0x4242, 0x3C42, 0x0000, 0x0000 };

static uint16 fa11_A244[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x1800, 0x0024, 0x423C, 0x4242, 0x4242, 0x3C42, 0x0000, 0x0000 };

static uint16 fa11_A245[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x1400, 0x0028, 0x423C, 0x4242, 0x4242, 0x3C42, 0x0000, 0x0000 };

static uint16 fa11_A246[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0014, 0x423C, 0x4242, 0x4242, 0x3C42, 0x0000, 0x0000 };

static uint16 fa11_A247[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x0008, 0x007F, 0x0008, 0x0000, 0x0000, 0x0000 };

static uint16 fa11_A248[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x241A, 0x4A4A, 0x5252, 0x7824, 0x0000, 0x0000 };

static uint16 fa11_A249[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x1000, 0x0008, 0x4242, 0x4242, 0x4242, 0x3A46, 0x0000, 0x0000 };

static uint16 fa11_A250[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0400, 0x0008, 0x4242, 0x4242, 0x4242, 0x3A46, 0x0000, 0x0000 };

static uint16 fa11_A251[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x1800, 0x0024, 0x4242, 0x4242, 0x4242, 0x3A46, 0x0000, 0x0000 };

static uint16 fa11_A252[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0028, 0x4242, 0x4242, 0x4242, 0x3A46, 0x0000, 0x0000 };

static uint16 fa11_A253[] = { 0xFFFF, 6, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x4100, 0x2000, 0x44C1, 0xA188, 0x8242, 0x0804, 0x0060 };

static uint16 fa11_A254[] = { 0xFFFF, 7, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x4000, 0x4040, 0x6458, 0x4242, 0x4242, 0x5864, 0x4040, 0x0040 };

static uint16 fa11_A255[] = { 0xFFFF, 6, 17, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0200, 0x2080, 0x44C1, 0xA188, 0x8242, 0x0804, 0x0060 };

static uint16 fa11_TAB_STOP[] = { 25 };
static uint16 fa11_TRANSPARENT_VALUE[] = { 0 };
static uint16 fa11_UNDERLINE[] = { 15 };

uint16* font_arial_11[] = {	0, X001, X002, X003, X004, X005, X006, 0, 0, 0,
							0, 0, 0, 0, X007, X008, X009, X010, X011, X012,
							X013, X014, 0, 0, 0, 0, 0, 0, 0, 0,
							0, 0, fa11_A032, fa11_A033, fa11_A034, fa11_A035, fa11_A036, fa11_A037, fa11_A038, fa11_A039,
							fa11_A040, fa11_A041, fa11_A042, fa11_A043, fa11_A044, fa11_A045, fa11_A046, fa11_A047, fa11_A048, fa11_A049,
							fa11_A050, fa11_A051, fa11_A052, fa11_A053, fa11_A054, fa11_A055, fa11_A056, fa11_A057, fa11_A058, fa11_A059,
							fa11_A060, fa11_A061, fa11_A062, fa11_A063, fa11_A064, fa11_A065, fa11_A066, fa11_A067, fa11_A068, fa11_A069,
							fa11_A070, fa11_A071, fa11_A072, fa11_A073, fa11_A074, fa11_A075, fa11_A076, fa11_A077, fa11_A078, fa11_A079,
							fa11_A080, fa11_A081, fa11_A082, fa11_A083, fa11_A084, fa11_A085, fa11_A086, fa11_A087, fa11_A088, fa11_A089,
							fa11_A090, fa11_A091, fa11_A092, fa11_A093, fa11_A094, fa11_A095, fa11_A096, fa11_A097, fa11_A098, fa11_A099,
							fa11_A100, fa11_A101, fa11_A102, fa11_A103, fa11_A104, fa11_A105, fa11_A106, fa11_A107, fa11_A108, fa11_A109,
							fa11_A110, fa11_A111, fa11_A112, fa11_A113, fa11_A114, fa11_A115, fa11_A116, fa11_A117, fa11_A118, fa11_A119,
							fa11_A120, fa11_A121, fa11_A122, fa11_A123, fa11_A124, fa11_A125, fa11_A126, 0, fa11_A128, 0,
							fa11_A130, fa11_A131, fa11_A132, fa11_A133, fa11_A134, fa11_A135, fa11_A136, fa11_A137, fa11_A138, fa11_A139,
							fa11_A140, 0, fa11_A142, 0, 0, fa11_A145, fa11_A146, fa11_A147, fa11_A148, fa11_A149,
							fa11_A150, fa11_A151, fa11_A152, fa11_A153, fa11_A154, fa11_A155, fa11_A156, 0, fa11_A158, fa11_A159,
							fa11_A160, fa11_A161, fa11_A162, fa11_A163, fa11_A164, fa11_A165, fa11_A166, fa11_A167, fa11_A168, fa11_A169,
							fa11_A170, fa11_A171, fa11_A172, fa11_A173, fa11_A174, fa11_A175, fa11_A176, fa11_A177, fa11_A178, fa11_A179,
							fa11_A180, fa11_A181, fa11_A182, fa11_A183, fa11_A184, fa11_A185, fa11_A186, fa11_A187, fa11_A188, fa11_A189,
							fa11_A190, fa11_A191, fa11_A192, fa11_A193, fa11_A194, fa11_A195, fa11_A196, fa11_A197, fa11_A198, fa11_A199,
							fa11_A200, fa11_A201, fa11_A202, fa11_A203, fa11_A204, fa11_A205, fa11_A206, fa11_A207, fa11_A208, fa11_A209,
							fa11_A210, fa11_A211, fa11_A212, fa11_A213, fa11_A214, fa11_A215, fa11_A216, fa11_A217, fa11_A218, fa11_A219,
							fa11_A220, fa11_A221, fa11_A222, fa11_A223, fa11_A224, fa11_A225, fa11_A226, fa11_A227, fa11_A228, fa11_A229,
							fa11_A230, fa11_A231, fa11_A232, fa11_A233, fa11_A234, fa11_A235, fa11_A236, fa11_A237, fa11_A238, fa11_A239,
							fa11_A240, fa11_A241, fa11_A242, fa11_A243, fa11_A244, fa11_A245, fa11_A246, fa11_A247, fa11_A248, fa11_A249,
							fa11_A250, fa11_A251, fa11_A252, fa11_A253, fa11_A254, fa11_A255, fa11_TAB_STOP, fa11_TRANSPARENT_VALUE, fa11_UNDERLINE, 0 };
