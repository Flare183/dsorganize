#ifndef _SID_INCLUDED
#define _SID_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#define USE_FILTER

void c64Init(int nSampleRate);
void c64Close();
int sidGetCIA();
int sidGetMixFreq();
unsigned short LoadSIDFromMemory(void *pSidData, unsigned short *load_addr, unsigned short *init_addr, unsigned short *play_addr, unsigned char *subsongs, unsigned char *startsong, unsigned char *speed, unsigned short size);
void cpuJSR(unsigned short npc, unsigned char na);
void synth_render(s16 *buffer, unsigned long len);
void sidPoke(int reg, unsigned char val);

#ifdef __cplusplus
}
#endif

#endif
