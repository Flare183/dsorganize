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
 
#ifndef _BROWSER_INCLUDED
#define _BROWSER_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#define CONFIRMDELETE 5
#define LOADBIN 4
#define MAXBROWSERCYCLES 4

#define COPY_SIZE 8192

void initBrowser();
void createBrowser(bool reload);
char *getFileName();
void createRenameScreen(char *curFileName, void (*forward)(int,int), void (*back)(int,int));
void drawFileInfoScreen();

void loadFile(int which);
void alternateLoadFile(int which);
void browserChangeDir(char *dir);
void deleteFAT(char *entry, bool deleteFinal);
bool setBrowserCursor(char *file);
int getBrowserEntries();

#ifdef __cplusplus
}
#endif

#endif
