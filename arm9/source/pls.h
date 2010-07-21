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
 
#ifndef _PLS_INCLUDED
#define _PLS_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	char data[256];
} DCHAR;

typedef struct
{
	DCHAR *descriptionEntry;
	DCHAR *urlEntry;
	int numEntries;
} PLS_TYPE;

bool loadPlaylist(char *fName, PLS_TYPE *outData);
void destroyPlaylist(PLS_TYPE *outData);

#ifdef __cplusplus
}
#endif

#endif
