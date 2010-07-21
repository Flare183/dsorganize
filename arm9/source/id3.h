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
 
#ifndef _ID3_INCLUDED
#define _ID3_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "fatwrapper.h"
#include <libsound/ivorbisfile.h>

typedef struct
{
    bool present;
	
    char title[31];
    char artist[31];
    char album[31];
    char year[5];
    int genre;
	char genreStr[60];
    char comment[29];
    int track;
}  ID3V1_TYPE;

void getID3V1(DRAGON_FILE *fp, ID3V1_TYPE *id3);
void getGenre(int g, char *str);
void getOggInfo(OggVorbis_File *vf, ID3V1_TYPE *id3);

#ifdef __cplusplus
}
#endif

#endif
