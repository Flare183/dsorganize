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
 
#ifndef _FATWRAPPER_INCLUDED
#define _FATWRAPPER_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <nds.h>

typedef struct {
	void *fp;
	u32 firstCluster;
	u32 chanceOfUnicode;
	
	// for file caching
	bool cacheEnabled;
	bool needsUpdate;
	u8 *cache;
	u32 filePos;
	u32 fileLen;
	u32 lagCursor;
	u32 leadCursor;	
} DRAGON_FILE;

enum {
	FE_NONE, 
	FE_FILE, 
	FE_DIR
};

#ifndef EOF
#define EOF -1
#define SEEK_SET	0
#define SEEK_CUR	1
#define SEEK_END	2
#endif

#ifndef ATTRIB_ARCH
#define ATTRIB_ARCH	0x20			// Archive
#define ATTRIB_DIR	0x10			// Directory
#define ATTRIB_LFN	0x0F			// Long file name
#define ATTRIB_VOL	0x08			// Volume
#define ATTRIB_SYS	0x04			// System
#define ATTRIB_HID	0x02			// Hidden
#define ATTRIB_RO	0x01			// Read only
#endif

#define MAX_FILES 4 // to match the max files in gba_nds_fat
#define CACHE_SIZE (64 * 1024) // 512 kb
#define CACHE_BUFFER 1024 // 1 kb buffer between the lead and lag
#define SEEK_BUFFER (16 * 1024) // 16 kb of data left after seeking

bool			DRAGON_InitFiles();
bool 			DRAGON_FreeFiles();
u32 			DRAGON_DiscType();
char 			*DRAGON_tryingInterface();
void 			DRAGON_preserveVars();
void 			DRAGON_restoreVars();
bool 			DRAGON_GetLongFilename(char* filename);
u32 			DRAGON_GetFileSize();
u32 			DRAGON_GetFileCluster();
int				DRAGON_mkdir(const char* path);
bool			DRAGON_chdir(const char* path);
int 			DRAGON_remove(const char* path);
bool 			DRAGON_rename(const char *oldName, const char *newName);
int				DRAGON_FileExists(const char* filename);
u8 				DRAGON_GetFileAttributes();
u8 				DRAGON_SetFileAttributes(const char* filename, u8 attributes, u8 mask);
bool 			DRAGON_GetAlias(char* alias);
DRAGON_FILE* 	DRAGON_fopen(const char* path, const char* mode);
bool 			DRAGON_fclose (DRAGON_FILE* df);
bool 			DRAGON_feof(DRAGON_FILE* df);
u32 			DRAGON_fread(void* buffer, u32 size, u32 count, DRAGON_FILE* df);
u32 			DRAGON_fwrite(const void* buffer, u32 size, u32 count, DRAGON_FILE* df);
u32 			DRAGON_flength(DRAGON_FILE* df);
int 			DRAGON_FindFirstFile(char* lfn);
int 			DRAGON_FindNextFile(char* lfn);
void			DRAGON_closeFind();
uint16 			DRAGON_fgetc(DRAGON_FILE *df);
void 			DRAGON_fputc(uint16 c, DRAGON_FILE* df);
char 			*DRAGON_fgets(char *tgtBuffer, int num, DRAGON_FILE* df);
int 			DRAGON_fputs(const char *string, DRAGON_FILE* df);
int 			DRAGON_fseek(DRAGON_FILE* df, s32 offset, int origin);
u32 			DRAGON_ftell(DRAGON_FILE* df);
void 			DRAGON_detectUnicode(DRAGON_FILE* df);
void 			DRAGON_enableCaching(DRAGON_FILE *df);
void 			DRAGON_cacheLoop();
bool 			DRAGON_isCached(DRAGON_FILE *df);
bool 			DRAGON_needsUpdate(DRAGON_FILE *df);

#ifdef __cplusplus
}
#endif

#endif
