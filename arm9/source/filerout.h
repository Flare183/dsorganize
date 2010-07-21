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
 
#ifndef _FILEROUT_INCLUDED
#define _FILEROUT_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

enum {
	UNKNOWN,
	DIRECTORY,
	NDSFILE,
	TXTFILE,
	VCFILE,
	DVFILE,
	REMFILE,
	PICFILE,
	BINFILE,
	TODOFILE,
	SOUNDFILE,
	HBFILE,
	PKGFILE,
	PLAYLISTFILE,
	SHORTCUTFILE
};

#define NDS_ICON_SIZE 1024

#include <nds.h>

typedef struct
{
	char longName[256];
	uint16 fileType;
	u32 fileSize;
	bool hidden;
	bool played;
} BROWSER_FILE;

typedef struct
{
	char internalName[256];
	char formattedName[3][256];
	uint16 iconData[1026];
	uint16 transColor;
	char longName[256];
} FILE_INFO;

typedef struct
{
	char urgency;
	char title[41];
	char message[1025];
	char fileName[256];
} TODO_FILE;

typedef struct
{
	char fileName[256];
	char shortName[64];
} SCRIBBLE_FILE;

// General
void safeFileName(char *fName);

// vcard
int populateVCard(char *dir);
void freeVCard();
int addVCard();
void sortVCard(int pos);

//browser
int populateDirList(char *dir);
void freeDirList();
void getInfo(BROWSER_FILE *bf, char *path, FILE_INFO *file);
int getFileType(char *fName);

//todo
int populateTodoList(char *dir);
void freeTodoList();
int addTodo();
void sortTodoList(int pos);
void clearTodo(TODO_FILE *vf);

//scribble
int populateScribbleList(char *dir);
void freeScribbleList();
void sortScribbleList(int pos);

// random stuff
void separateDir(char *dir, char *file);
int nextFile(char *curFile, int numEntries);
int prevFile(char *curFile);
int randFile(char *curFile, int fileType, int numEntries);

// for pls files
char *getLastDir();

#ifdef __cplusplus
}
#endif

#endif
