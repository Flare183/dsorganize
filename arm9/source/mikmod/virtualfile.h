#ifndef _VF_INCLUDED
#define _VF_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	char *buffer;
	u32 size;
	u32 pos;
} VIRTUAL_FILE;

#ifdef __cplusplus
}
#endif

#endif

