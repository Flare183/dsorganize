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
 
#ifndef _CALCULATOR_INCLUDED
#define _CALCULATOR_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#define RECIPROCAL 0
#define FACTORIAL 1
#define SQRT 2
#define PI 3
#define EXP 4
#define LN 5
#define SIN 6
#define COS 7
#define TAN 8
#define LOG 9
#define SQUARE 10
#define ARCSIN 11
#define ARCCOS 12
#define ARCTAN 13
#define M_INT 14

#define ACCURACY 20
#define DIGITS 199999

#define NUM_CONTROLS 36
#define NUM_SCREENS 2
#define NUM_BUTTONS_X 6
#define NUM_BUTTONS_Y 6

void initCalc();
void drawAnswerScreen();
bool isWorking();

#ifdef __cplusplus
}
#endif

#endif
