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
 
#ifndef _SCRIBBLE_INCLUDED
#define _SCRIBBLE_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <nds.h>

#define TOOL_PENCIL 0
#define TOOL_PEN 1
#define TOOL_COLORGRAB 2
#define TOOL_ERASOR 3
#define TOOL_FLOODFILL 4
#define TOOL_LINE 5
#define TOOL_THICKLINE 6
#define TOOL_RECT 7
#define TOOL_THICKRECT 8
#define TOOL_CIRCLE 9
#define TOOL_THICKCIRCLE 10
#define TOOL_SPRAY 11
#define TOOL_REPLACE 12
#define TOOL_TEXT 13

#define MAX_TOOLS 14

#define SAVE_BMP 0
#define SAVE_PNG 1

#define SCRIBBLE_LEN 1024

void initScribble();
uint16 *getCustomColors();
int getScribbleEntries();
bool screensFlipped();

#ifdef __cplusplus
}
#endif

#endif
