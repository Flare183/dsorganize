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
 
#ifndef _SOUNDPLAYER_INCLUDED
#define _SOUNDPLAYER_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#define TYPE_NORMAL 0
#define TYPE_PLS 1

// sound modes
#define SOUND_ONESHOT 0
#define SOUND_NEXTFILE 1
#define SOUND_RANDFILE 2

void initSound();
void initRandomList();
void destroyRandomList();
void loadSoundMode();
void saveSoundMode();
void checkEndSound();

#ifdef __cplusplus
}
#endif

#endif
