#include <nds.h>
#include <stdio.h>
#include <nds.h>
#include <stdio.h>

// Font name: 'Profontwindows' size 9
// [ ] Bold
// [ ] Italics

// Width bias: 0, 0
// Loc bias: 0, 0
// Underline: 11

static uint16 fp9_A032[] = { 0xFFFF, 6, 12, 1, 0, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 };

static uint16 fp9_A033[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0281, 0x0804, 0x0010, 0x0040, 0x0000 };

static uint16 fp9_A034[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0xA100, 0x8042, 0x0000, 0x0000, 0x0000, 0x0000 };

static uint16 fp9_A035[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0100, 0xC547, 0x141F, 0x0000, 0x0000, 0x0000 };

static uint16 fp9_A036[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x4100, 0x4AC5, 0x0A0E, 0xA814, 0x80E0, 0x0000 };

static uint16 fp9_A037[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0100, 0x4AE5, 0x0C8A, 0xA934, 0x0020, 0x0000 };

static uint16 fp9_A038[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0100, 0x8A84, 0x2808, 0x9054, 0x00D0, 0x0000 };

static uint16 fp9_A039[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x4000, 0x0081, 0x0000, 0x0000, 0x0000, 0x0000 };

static uint16 fp9_A040[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2000, 0x0482, 0x1008, 0x4020, 0x4040, 0x0000 };

static uint16 fp9_A041[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x8000, 0x8180, 0x0402, 0x1008, 0x0041, 0x0000 };

static uint16 fp9_A042[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x4785, 0x0815, 0x0000, 0x0000, 0x0000 };

static uint16 fp9_A043[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0200, 0x3E04, 0x2010, 0x0000, 0x0000 };

static uint16 fp9_A044[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x6000, 0x82C0, 0x0000 };

static uint16 fp9_A045[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x1C00, 0x0000, 0x0000, 0x0000 };

static uint16 fp9_A046[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x3000, 0x0060, 0x0000 };

static uint16 fp9_A047[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x4120, 0x0802, 0x4010, 0x0482, 0x0000 };

static uint16 fp9_A048[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0100, 0x49C4, 0x3295, 0x8844, 0x00E0, 0x0000 };

static uint16 fp9_A049[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0287, 0x0804, 0x2110, 0x00F0, 0x0000 };

static uint16 fp9_A050[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0100, 0x40C4, 0x0882, 0x8120, 0x00F0, 0x0000 };

static uint16 fp9_A051[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0100, 0x40C4, 0x0286, 0x8804, 0x00E0, 0x0000 };

static uint16 fp9_A052[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x8541, 0x3E12, 0x1008, 0x0070, 0x0000 };

static uint16 fp9_A053[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0300, 0x08E4, 0x021E, 0x8804, 0x00E0, 0x0000 };

static uint16 fp9_A054[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0100, 0x08C4, 0x221E, 0x8844, 0x00E0, 0x0000 };

static uint16 fp9_A055[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0300, 0x40E0, 0x0882, 0x2010, 0x0040, 0x0000 };

static uint16 fp9_A056[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0100, 0x48C4, 0x228E, 0x8844, 0x00E0, 0x0000 };

static uint16 fp9_A057[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0100, 0x48C4, 0x1E91, 0x0804, 0x00E0, 0x0000 };

static uint16 fp9_A058[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0300, 0x0006, 0x3000, 0x0060, 0x0000 };

static uint16 fp9_A059[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0600, 0x000C, 0x6000, 0x82C0, 0x0000 };

static uint16 fp9_A060[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x8220, 0x0808, 0x0808, 0x0000, 0x0000 };

static uint16 fp9_A061[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0F00, 0x3E80, 0x0000, 0x0000, 0x0000 };

static uint16 fp9_A062[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0100, 0x0101, 0x0401, 0x4010, 0x0000, 0x0000 };

static uint16 fp9_A063[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0100, 0x40C4, 0x0481, 0x0010, 0x0040, 0x0000 };

static uint16 fp9_A064[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0100, 0x4BC4, 0x2E95, 0x8040, 0x00F0, 0x0000 };

static uint16 fp9_A065[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x8582, 0x2211, 0x897C, 0x0010, 0x0000 };

static uint16 fp9_A066[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0300, 0x48C4, 0x229E, 0x8944, 0x00E0, 0x0000 };

static uint16 fp9_A067[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0100, 0x48C4, 0x2010, 0x8840, 0x00E0, 0x0000 };

static uint16 fp9_A068[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0300, 0x48C4, 0x2291, 0x8944, 0x00E0, 0x0000 };

static uint16 fp9_A069[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0300, 0x08E4, 0x201E, 0x8140, 0x00F0, 0x0000 };

static uint16 fp9_A070[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0300, 0x08E4, 0x201E, 0x8140, 0x0000, 0x0000 };

static uint16 fp9_A071[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0100, 0x48C4, 0x2610, 0x8844, 0x00E0, 0x0000 };

static uint16 fp9_A072[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0200, 0x4824, 0x229F, 0x8944, 0x0010, 0x0000 };

static uint16 fp9_A073[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0300, 0x02E1, 0x0804, 0x2110, 0x00F0, 0x0000 };

static uint16 fp9_A074[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x4020, 0x0281, 0x8844, 0x00E0, 0x0000 };

static uint16 fp9_A075[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0200, 0x8A24, 0x3018, 0x9150, 0x0010, 0x0000 };

static uint16 fp9_A076[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0200, 0x0804, 0x2010, 0x8140, 0x00F0, 0x0000 };

static uint16 fp9_A077[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0200, 0xCA26, 0x2295, 0x8944, 0x0010, 0x0000 };

static uint16 fp9_A078[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0200, 0x4A26, 0x2293, 0x8944, 0x0010, 0x0000 };

static uint16 fp9_A079[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0100, 0x48C4, 0x2291, 0x8844, 0x00E0, 0x0000 };

static uint16 fp9_A080[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0300, 0x48C4, 0x209E, 0x8140, 0x0000, 0x0000 };

static uint16 fp9_A081[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0100, 0x48C4, 0x2291, 0xA844, 0x20E0, 0x0000 };

static uint16 fp9_A082[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0300, 0x48C4, 0x229E, 0x8944, 0x0010, 0x0000 };

static uint16 fp9_A083[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0100, 0x48C4, 0x020E, 0x8804, 0x00E0, 0x0000 };

static uint16 fp9_A084[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0300, 0x02E1, 0x0804, 0x2010, 0x0040, 0x0000 };

static uint16 fp9_A085[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0200, 0x4824, 0x2291, 0x8844, 0x00E0, 0x0000 };

static uint16 fp9_A086[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0200, 0x4824, 0x1491, 0x2028, 0x0040, 0x0000 };

static uint16 fp9_A087[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0200, 0x4824, 0x2A91, 0xD954, 0x0010, 0x0000 };

static uint16 fp9_A088[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0200, 0x4824, 0x088A, 0x8928, 0x0010, 0x0000 };

static uint16 fp9_A089[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0200, 0x4824, 0x1491, 0x2010, 0x0040, 0x0000 };

static uint16 fp9_A090[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0300, 0x41E0, 0x1004, 0x8140, 0x00F0, 0x0000 };

static uint16 fp9_A091[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x6000, 0x0281, 0x0804, 0x2010, 0xC040, 0x0000 };

static uint16 fp9_A092[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0100, 0x0202, 0x0404, 0x0808, 0x1010, 0x0020 };

static uint16 fp9_A093[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0xC000, 0x0281, 0x0804, 0x2010, 0x8041, 0x0000 };

static uint16 fp9_A094[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x8882, 0x0080, 0x0000, 0x0000, 0x0000 };

static uint16 fp9_A095[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x0000, 0x0700, 0x00E0 };

static uint16 fp9_A096[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x8000, 0x0080, 0x0000, 0x0000, 0x0000, 0x0000 };

static uint16 fp9_A097[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x220F, 0x9844, 0x00D0, 0x0000 };

static uint16 fp9_A098[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0200, 0x0804, 0x221E, 0x8944, 0x00E0, 0x0000 };

static uint16 fp9_A099[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x220E, 0x8040, 0x00F0, 0x0000 };

static uint16 fp9_A100[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x4020, 0x228F, 0x8844, 0x00F0, 0x0000 };

static uint16 fp9_A101[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x220E, 0x807C, 0x00F0, 0x0000 };

static uint16 fp9_A102[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0761, 0x0804, 0x2010, 0x0040, 0x0000 };

static uint16 fp9_A103[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x220F, 0x8844, 0x23F0, 0x0080 };

static uint16 fp9_A104[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0200, 0x0804, 0x221E, 0x8944, 0x0010, 0x0000 };

static uint16 fp9_A105[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0080, 0x080C, 0x2010, 0x00E0, 0x0000 };

static uint16 fp9_A106[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0080, 0x080C, 0x2010, 0x8640, 0x0000 };

static uint16 fp9_A107[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0200, 0x0804, 0x2812, 0x9170, 0x0010, 0x0000 };

static uint16 fp9_A108[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0100, 0x0281, 0x0804, 0x2010, 0x00E0, 0x0000 };

static uint16 fp9_A109[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x2A1E, 0xA954, 0x0050, 0x0000 };

static uint16 fp9_A110[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x3216, 0x8944, 0x0010, 0x0000 };

static uint16 fp9_A111[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x220E, 0x8844, 0x00E0, 0x0000 };

static uint16 fp9_A112[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x221E, 0x8944, 0x04E2, 0x0000 };

static uint16 fp9_A113[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x220F, 0x8844, 0x20F0, 0x0040 };

static uint16 fp9_A114[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x3216, 0x8140, 0x0000, 0x0000 };

static uint16 fp9_A115[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x200F, 0x0938, 0x00E0, 0x0000 };

static uint16 fp9_A116[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0281, 0x080E, 0x2010, 0x0030, 0x0000 };

static uint16 fp9_A117[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x2211, 0x9844, 0x00D0, 0x0000 };

static uint16 fp9_A118[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x2211, 0x5028, 0x0040, 0x0000 };

static uint16 fp9_A119[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x2A15, 0xA854, 0x00A0, 0x0000 };

static uint16 fp9_A120[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x1411, 0x5110, 0x0010, 0x0000 };

static uint16 fp9_A121[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x2211, 0x8844, 0x23F0, 0x0080 };

static uint16 fp9_A122[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x041F, 0x4110, 0x00F0, 0x0000 };

static uint16 fp9_A123[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2000, 0x0281, 0x1004, 0x2010, 0x8040, 0x0080 };

static uint16 fp9_A124[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x4000, 0x0281, 0x0804, 0x2010, 0x8040, 0x0000 };

static uint16 fp9_A125[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x8000, 0x0281, 0x0404, 0x2010, 0x8240, 0x0000 };

static uint16 fp9_A126[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0600, 0x0096, 0x0000, 0x0000, 0x0000 };

static uint16 fp9_A128[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x7100, 0x0F02, 0x3E88, 0x4020, 0x0070, 0x0000 };

static uint16 fp9_A130[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x4000, 0x0082, 0x0000 };

static uint16 fp9_A131[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x4018, 0x0281, 0x080E, 0x2010, 0x8640, 0x0000 };

static uint16 fp9_A132[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x5000, 0x80A2, 0x0000 };

static uint16 fp9_A133[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x0100, 0x0050, 0x0000 };

static uint16 fp9_A134[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x8283, 0x0804, 0x0010, 0x0000, 0x0000 };

static uint16 fp9_A135[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x8283, 0x1C04, 0x0010, 0x0000, 0x0000 };

static uint16 fp9_A136[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x4100, 0x0040, 0x0000, 0x0000, 0x0000, 0x0000 };

static uint16 fp9_A137[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0xFA00, 0x4595, 0x1A04, 0x546A, 0x0050, 0x0000 };

static uint16 fp9_A138[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x4150, 0x48C4, 0x020E, 0x8804, 0x00E0, 0x0000 };

static uint16 fp9_A139[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x1000, 0x8040, 0x0080, 0x0000 };

static uint16 fp9_A140[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0100, 0x0AE5, 0x2816, 0xA050, 0x00F0, 0x0000 };

static uint16 fp9_A142[] = { 0xFFFF, 6, 12, 1, 0, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 };

static uint16 fp9_A145[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0241, 0x0000, 0x0000, 0x0000, 0x0000 };

static uint16 fp9_A146[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x8240, 0x0000, 0x0000, 0x0000, 0x0000 };

static uint16 fp9_A147[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x85A2, 0x0000, 0x0000, 0x0000, 0x0000 };

static uint16 fp9_A148[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x45A1, 0x0000, 0x0000, 0x0000, 0x0000 };

static uint16 fp9_A149[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x8F03, 0x3E9F, 0x0038, 0x0000, 0x0000 };

static uint16 fp9_A150[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x1C00, 0x0000, 0x0000, 0x0000 };

static uint16 fp9_A151[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x3F00, 0x0000, 0x0000, 0x0000 };

static uint16 fp9_A152[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0xD200, 0x00C0, 0x0000, 0x0000, 0x0000, 0x0000 };

static uint16 fp9_A153[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0300, 0xE5D2, 0x00C0, 0x0000, 0x0000, 0x0000 };

static uint16 fp9_A154[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0xA000, 0x0080, 0x200F, 0x0938, 0x00E0, 0x0000 };

static uint16 fp9_A155[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x2000, 0x4120, 0x0000, 0x0000 };

static uint16 fp9_A156[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x2A0A, 0xA05C, 0x00B0, 0x0000 };

static uint16 fp9_A158[] = { 0xFFFF, 6, 12, 1, 0, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 };

static uint16 fp9_A159[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0050, 0x4804, 0x1491, 0x2010, 0x0040, 0x0000 };

static uint16 fp9_A160[] = { 0xFFFF, 6, 12, 1, 0, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 };

static uint16 fp9_A161[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0140, 0x0402, 0x1008, 0x0020, 0x0000 };

static uint16 fp9_A162[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0200, 0x2A0E, 0xA050, 0x80F0, 0x0000 };

static uint16 fp9_A163[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0xC200, 0x0E44, 0x2010, 0x8940, 0x00E0, 0x0000 };

static uint16 fp9_A164[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0800, 0x148E, 0x8838, 0x0000, 0x0000 };

static uint16 fp9_A165[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0200, 0x8F22, 0x3E84, 0x2010, 0x0040, 0x0000 };

static uint16 fp9_A166[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x4000, 0x0281, 0x0000, 0x2010, 0x8040, 0x0000 };

static uint16 fp9_A167[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x1171, 0x0925, 0x1211, 0x9114, 0xC011, 0x0000 };

static uint16 fp9_A168[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0100, 0x0040, 0x0000, 0x0000, 0x0000, 0x0000 };

static uint16 fp9_A169[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0100, 0x2BE4, 0xB158, 0x845A, 0x00F0, 0x0000 };

static uint16 fp9_A170[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x9138, 0xC021, 0x1E00, 0x0000, 0x0000, 0x0000 };

static uint16 fp9_A171[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x1200, 0x9048, 0x0090, 0x0000 };

static uint16 fp9_A172[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x021F, 0x0004, 0x0000, 0x0000 };

static uint16 fp9_A173[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x3E00, 0x0000, 0x0000, 0x0000 };

static uint16 fp9_A174[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0100, 0x2EE4, 0xB95A, 0x846A, 0x00F0, 0x0000 };

static uint16 fp9_A175[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0100, 0x00C0, 0x0000, 0x0000, 0x0000, 0x0000 };

static uint16 fp9_A176[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x6100, 0x4322, 0x0000, 0x0000, 0x0000, 0x0000 };

static uint16 fp9_A177[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0201, 0x081F, 0xF810, 0x0000, 0x0000 };

static uint16 fp9_A178[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2060, 0x8083, 0x0000, 0x0000, 0x0000, 0x0000 };

static uint16 fp9_A179[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2060, 0x86C0, 0x0000, 0x0000, 0x0000, 0x0000 };

static uint16 fp9_A180[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x3800, 0x00E0, 0x0000, 0x0000, 0x0000, 0x0000 };

static uint16 fp9_A181[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x2412, 0x9148, 0x04D2, 0x0000 };

static uint16 fp9_A182[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0xF200, 0x4AA5, 0x0A8F, 0x2814, 0x0050, 0x0000 };

static uint16 fp9_A183[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0C06, 0x0000, 0x0000, 0x0000 };

static uint16 fp9_A184[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x0000, 0x0000, 0xE3F0, 0x0080 };

static uint16 fp9_A185[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x4060, 0x8083, 0x0000, 0x0000, 0x0000, 0x0000 };

static uint16 fp9_A186[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x9130, 0x8021, 0x1E00, 0x0000, 0x0000, 0x0000 };

static uint16 fp9_A187[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x2400, 0x4924, 0x0020, 0x0000 };

static uint16 fp9_A188[] = { 0xFFFF, 6, 12, 2, 32767, 0, // width, height, # of pallate entries, pallate entries
0x7E3F, 0xBEE8, 0xEDFB, 0xC7B3, 0xFFEF, 0xE0FF };

static uint16 fp9_A189[] = { 0xFFFF, 6, 12, 2, 32767, 0, // width, height, # of pallate entries, pallate entries
0x7E3F, 0xBEE8, 0xE9FB, 0xF7BD, 0xFFC7, 0xE0FF };

static uint16 fp9_A190[] = { 0xFFFF, 6, 12, 2, 32767, 0, // width, height, # of pallate entries, pallate entries
0xBE3F, 0xB26E, 0xEDFB, 0xC7B3, 0xFFEF, 0xE0FF };

static uint16 fp9_A191[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0280, 0x2008, 0x8840, 0x00E0, 0x0000 };

static uint16 fp9_A192[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x4040, 0x8582, 0x2211, 0x897C, 0x0010, 0x0000 };

static uint16 fp9_A193[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x4010, 0x8582, 0x2211, 0x897C, 0x0010, 0x0000 };

static uint16 fp9_A194[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0xA020, 0x0501, 0x220A, 0x897C, 0x0010, 0x0000 };

static uint16 fp9_A195[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x6069, 0x8582, 0x2211, 0x897C, 0x0010, 0x0000 };

static uint16 fp9_A196[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0050, 0x8582, 0x2211, 0x897C, 0x0010, 0x0000 };

static uint16 fp9_A197[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0xA020, 0x8582, 0x2211, 0x897C, 0x0010, 0x0000 };

static uint16 fp9_A198[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0100, 0x0AE5, 0x281E, 0xA150, 0x0070, 0x0000 };

static uint16 fp9_A199[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0100, 0x48C4, 0x2010, 0x8840, 0x82E0, 0x0000 };

static uint16 fp9_A200[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x4340, 0x08E4, 0x201E, 0x8140, 0x00F0, 0x0000 };

static uint16 fp9_A201[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x4310, 0x08E4, 0x201E, 0x8140, 0x00F0, 0x0000 };

static uint16 fp9_A202[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0xA320, 0x08E4, 0x201E, 0x8140, 0x00F0, 0x0000 };

static uint16 fp9_A203[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0350, 0x08E4, 0x201E, 0x8140, 0x00F0, 0x0000 };

static uint16 fp9_A204[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x4340, 0x02E1, 0x0804, 0x2110, 0x00F0, 0x0000 };

static uint16 fp9_A205[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x4310, 0x02E1, 0x0804, 0x2110, 0x00F0, 0x0000 };

static uint16 fp9_A206[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0xA020, 0xC207, 0x0804, 0x2110, 0x00F0, 0x0000 };

static uint16 fp9_A207[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0350, 0x02E1, 0x0804, 0x2110, 0x00F0, 0x0000 };

static uint16 fp9_A208[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0100, 0x24E2, 0x915C, 0x4422, 0x00F0, 0x0000 };

static uint16 fp9_A209[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x6269, 0x4A26, 0x2293, 0x8944, 0x0010, 0x0000 };

static uint16 fp9_A210[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x4140, 0x48C4, 0x2291, 0x8844, 0x00E0, 0x0000 };

static uint16 fp9_A211[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x4110, 0x48C4, 0x2291, 0x8844, 0x00E0, 0x0000 };

static uint16 fp9_A212[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0xA020, 0x8803, 0x2291, 0x8844, 0x00E0, 0x0000 };

static uint16 fp9_A213[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x6169, 0x48C4, 0x2291, 0x8844, 0x00E0, 0x0000 };

static uint16 fp9_A214[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0150, 0x48C4, 0x2291, 0x8844, 0x00E0, 0x0000 };

static uint16 fp9_A215[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x4504, 0x1404, 0x0044, 0x0000, 0x0000 };

static uint16 fp9_A216[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0100, 0x49C4, 0x3295, 0x8844, 0x00E0, 0x0000 };

static uint16 fp9_A217[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x4240, 0x4824, 0x2291, 0x8844, 0x00E0, 0x0000 };

static uint16 fp9_A218[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x4210, 0x4824, 0x2291, 0x8844, 0x00E0, 0x0000 };

static uint16 fp9_A219[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0xA020, 0x4804, 0x2291, 0x8844, 0x00E0, 0x0000 };

static uint16 fp9_A220[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0250, 0x4824, 0x2291, 0x8844, 0x00E0, 0x0000 };

static uint16 fp9_A221[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x4210, 0x4824, 0x1491, 0x2010, 0x0040, 0x0000 };

static uint16 fp9_A222[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0200, 0x8807, 0x2291, 0x8178, 0x0000, 0x0000 };

static uint16 fp9_A223[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x6100, 0x4924, 0x2212, 0x8944, 0x0062, 0x0000 };

static uint16 fp9_A224[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x4000, 0x0040, 0x220F, 0x9844, 0x00D0, 0x0000 };

static uint16 fp9_A225[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2000, 0x0080, 0x220F, 0x9844, 0x00D0, 0x0000 };

static uint16 fp9_A226[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x4100, 0x0040, 0x220F, 0x9844, 0x00D0, 0x0000 };

static uint16 fp9_A227[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0xD200, 0x00C0, 0x220F, 0x9844, 0x00D0, 0x0000 };

static uint16 fp9_A228[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0100, 0x0040, 0x220F, 0x9844, 0x00D0, 0x0000 };

static uint16 fp9_A229[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x4100, 0x0041, 0x220F, 0x9844, 0x00D0, 0x0000 };

static uint16 fp9_A230[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x2A0E, 0xA05C, 0x00F0, 0x0000 };

static uint16 fp9_A231[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x220E, 0x8040, 0x82F0, 0x0000 };

static uint16 fp9_A232[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x8000, 0x0080, 0x220E, 0x807C, 0x00F0, 0x0000 };

static uint16 fp9_A233[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2000, 0x0080, 0x220E, 0x807C, 0x00F0, 0x0000 };

static uint16 fp9_A234[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x4100, 0x0040, 0x220E, 0x807C, 0x00F0, 0x0000 };

static uint16 fp9_A235[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0100, 0x0040, 0x220E, 0x807C, 0x00F0, 0x0000 };

static uint16 fp9_A236[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x8000, 0x0080, 0x080C, 0x2010, 0x00E0, 0x0000 };

static uint16 fp9_A237[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2000, 0x0080, 0x080C, 0x2010, 0x00E0, 0x0000 };

static uint16 fp9_A238[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x4100, 0x0040, 0x080C, 0x2010, 0x00E0, 0x0000 };

static uint16 fp9_A239[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0100, 0x0040, 0x080C, 0x2010, 0x00E0, 0x0000 };

static uint16 fp9_A240[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0xC100, 0x8080, 0x228F, 0x8844, 0x00E0, 0x0000 };

static uint16 fp9_A241[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0xD200, 0x00C0, 0x3216, 0x8944, 0x0010, 0x0000 };

static uint16 fp9_A242[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x8000, 0x0080, 0x220E, 0x8844, 0x00E0, 0x0000 };

static uint16 fp9_A243[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2000, 0x0080, 0x220E, 0x8844, 0x00E0, 0x0000 };

static uint16 fp9_A244[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x4100, 0x0040, 0x220E, 0x8844, 0x00E0, 0x0000 };

static uint16 fp9_A245[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0xD200, 0x00C0, 0x220E, 0x8844, 0x00E0, 0x0000 };

static uint16 fp9_A246[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0100, 0x0040, 0x220E, 0x8844, 0x00E0, 0x0000 };

static uint16 fp9_A247[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0200, 0x3E00, 0x2000, 0x0000, 0x0000 };

static uint16 fp9_A248[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0000, 0x0000, 0x260E, 0xC854, 0x00E0, 0x0000 };

static uint16 fp9_A249[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x8000, 0x0080, 0x2211, 0x9844, 0x00D0, 0x0000 };

static uint16 fp9_A250[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2000, 0x0080, 0x2211, 0x9844, 0x00D0, 0x0000 };

static uint16 fp9_A251[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x4100, 0x0040, 0x2211, 0x9844, 0x00D0, 0x0000 };

static uint16 fp9_A252[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0100, 0x0040, 0x2211, 0x9844, 0x00D0, 0x0000 };

static uint16 fp9_A253[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x2000, 0x0080, 0x2211, 0x8844, 0x23F0, 0x0080 };

static uint16 fp9_A254[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0200, 0x0804, 0x221E, 0x8944, 0x04E2, 0x0000 };

static uint16 fp9_A255[] = { 0xFFFF, 6, 12, 2, 0, 32767, // width, height, # of pallate entries, pallate entries
0x0100, 0x0040, 0x2211, 0x8844, 0x23F0, 0x0080 };

static uint16 fp9_TAB_STOP[] = { 25 };
static uint16 fp9_TRANSPARENT_VALUE[] = { 0 };
static uint16 fp9_UNDERLINE[] = { 11 };
uint16* font_profont_9[] = { 	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
								0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
								0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
								0, 0, fp9_A032, fp9_A033, fp9_A034, fp9_A035, fp9_A036, fp9_A037, fp9_A038, fp9_A039,
								fp9_A040, fp9_A041, fp9_A042, fp9_A043, fp9_A044, fp9_A045, fp9_A046, fp9_A047, fp9_A048, fp9_A049,
								fp9_A050, fp9_A051, fp9_A052, fp9_A053, fp9_A054, fp9_A055, fp9_A056, fp9_A057, fp9_A058, fp9_A059,
								fp9_A060, fp9_A061, fp9_A062, fp9_A063, fp9_A064, fp9_A065, fp9_A066, fp9_A067, fp9_A068, fp9_A069,
								fp9_A070, fp9_A071, fp9_A072, fp9_A073, fp9_A074, fp9_A075, fp9_A076, fp9_A077, fp9_A078, fp9_A079,
								fp9_A080, fp9_A081, fp9_A082, fp9_A083, fp9_A084, fp9_A085, fp9_A086, fp9_A087, fp9_A088, fp9_A089,
								fp9_A090, fp9_A091, fp9_A092, fp9_A093, fp9_A094, fp9_A095, fp9_A096, fp9_A097, fp9_A098, fp9_A099,
								fp9_A100, fp9_A101, fp9_A102, fp9_A103, fp9_A104, fp9_A105, fp9_A106, fp9_A107, fp9_A108, fp9_A109,
								fp9_A110, fp9_A111, fp9_A112, fp9_A113, fp9_A114, fp9_A115, fp9_A116, fp9_A117, fp9_A118, fp9_A119,
								fp9_A120, fp9_A121, fp9_A122, fp9_A123, fp9_A124, fp9_A125, fp9_A126, 0, fp9_A128, 0,
								fp9_A130, fp9_A131, fp9_A132, fp9_A133, fp9_A134, fp9_A135, fp9_A136, fp9_A137, fp9_A138, fp9_A139,
								fp9_A140, 0, fp9_A142, 0, 0, fp9_A145, fp9_A146, fp9_A147, fp9_A148, fp9_A149,
								fp9_A150, fp9_A151, fp9_A152, fp9_A153, fp9_A154, fp9_A155, fp9_A156, 0, fp9_A158, fp9_A159,
								fp9_A160, fp9_A161, fp9_A162, fp9_A163, fp9_A164, fp9_A165, fp9_A166, fp9_A167, fp9_A168, fp9_A169,
								fp9_A170, fp9_A171, fp9_A172, fp9_A173, fp9_A174, fp9_A175, fp9_A176, fp9_A177, fp9_A178, fp9_A179,
								fp9_A180, fp9_A181, fp9_A182, fp9_A183, fp9_A184, fp9_A185, fp9_A186, fp9_A187, fp9_A188, fp9_A189,
								fp9_A190, fp9_A191, fp9_A192, fp9_A193, fp9_A194, fp9_A195, fp9_A196, fp9_A197, fp9_A198, fp9_A199,
								fp9_A200, fp9_A201, fp9_A202, fp9_A203, fp9_A204, fp9_A205, fp9_A206, fp9_A207, fp9_A208, fp9_A209,
								fp9_A210, fp9_A211, fp9_A212, fp9_A213, fp9_A214, fp9_A215, fp9_A216, fp9_A217, fp9_A218, fp9_A219,
								fp9_A220, fp9_A221, fp9_A222, fp9_A223, fp9_A224, fp9_A225, fp9_A226, fp9_A227, fp9_A228, fp9_A229,
								fp9_A230, fp9_A231, fp9_A232, fp9_A233, fp9_A234, fp9_A235, fp9_A236, fp9_A237, fp9_A238, fp9_A239,
								fp9_A240, fp9_A241, fp9_A242, fp9_A243, fp9_A244, fp9_A245, fp9_A246, fp9_A247, fp9_A248, fp9_A249,
								fp9_A250, fp9_A251, fp9_A252, fp9_A253, fp9_A254, fp9_A255, fp9_TAB_STOP, fp9_TRANSPARENT_VALUE, fp9_UNDERLINE, 0 };
