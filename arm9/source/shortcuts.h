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
 
#ifndef _SHORTCUTS_INCLUDED
#define _SHORTCUTS_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#define CRC32_XINIT 0xFFFFFFFFL		/* initial value */
#define CRC32_XOROT 0xFFFFFFFFL		/* final xor value */

#define MAX_SHORTCUT_SIZE 1024

typedef struct
{
	char name[32];
	char location[256];
	char path[256];
	char largeiconpath[256];
	uint16 *loadedIcon;
	bool alternateBoot;
} SHORTCUT;

bool loadShortcut(char *filename, SHORTCUT *sc);
void loadShortcutIcon(SHORTCUT *sc);
void launchShortcut(SHORTCUT *sc);
void loadShortcutIcon(SHORTCUT *sc);
void freeShortcutIcon(SHORTCUT *sc);

unsigned long CalcCRC32(char *p);

#ifdef __cplusplus
}
#endif

#endif
