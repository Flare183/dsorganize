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
 
#ifndef _VIEWER_INCLUDED
#define _VIEWER_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#define WINDOWS 0
#define MAC 1
#define UNIX 2

#define NUM_BOOKMARKS 10

#define CURSOR_UP 0
#define CURSOR_DOWN 1
#define CURSOR_LEFT 2
#define CURSOR_RIGHT 3
#define CURSOR_PAGEUP 4
#define CURSOR_PAGEDOWN 5

#define PLAINTEXT 0
#define HTML 1

void drawViewerScreens();
void moveViewerCursor(int direction);
void freeViewerText();
void toggleBookmark();
void initViewer();
void setViewerPos(int pos);
int getViewerMax();

#ifdef __cplusplus
}
#endif

#endif
