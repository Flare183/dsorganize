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
 
#ifndef _TODO_INCLUDED
#define _TODO_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "filerout.h"

#define ONHOLD 0
#define NORMAL 1
#define NORMALSEMI 2
#define URGENT 3
#define URGENTSEMI 4
#define FINISHED 5

void initTodo();
void initTodoFile(char *fName);
void loadCurrentTodos(char **rStr);
int getTodoEntries();
void fixGautami();

#ifdef __cplusplus
}
#endif

#endif
