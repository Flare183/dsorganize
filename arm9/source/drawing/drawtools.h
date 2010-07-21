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
 
#ifndef _DRAWTOOLS_INCLUDED
#define _DRAWTOOLS_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#define STACK_SIZE 49152*2

void s_setPixel(int x, int y, uint16 color);
void s_drawLine(int x0, int y0, int x1, int y1, uint16 color);
void s_drawEllipse(int centerX, int centerY, int XRadius, int YRadius, uint16 color);
void s_floodFill(int x, int y, uint16 newColor, uint16 oldColor);
void s_spray(int x, int y, int radius, uint16 color);

// color picker
void drawChooser(int x, int y, uint16 *buffer);
uint16 drawSlider(int x, int y, uint16 baseColor, int slidePosition, uint16 *buffer);

// accessor routines
void setPenWidth(int x);
void setDrawCheckered(bool x);
void setDrawBuffer(uint16 *x);
uint16 *getDrawBuffer();

#ifdef __cplusplus
}
#endif

#endif
