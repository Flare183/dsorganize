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
 
#ifndef _GLOBALS_INIT
#define _GLOBALS_INIT

#ifdef __cplusplus
extern "C" {
#endif

#define FILENAME_SIZE 256

#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 192
#define SCREEN_BPP 2
#define FULLPAGE_HEIGHT (SCREEN_HEIGHT * 2)

#define CR 0x0D
#define LF 0x0A

#define EOS 0

#define SCRATCH_START (*(void *)0x06880000)

extern char d_base[60];
extern char d_day[60];
extern char d_help[60];
extern char d_lang[60];
extern char d_reminder[60];
extern char d_scribble[60];
extern char d_todo[60];
extern char d_vcard[60];
extern char d_icons[60];
extern char d_res[60];
extern char d_cache[60];
extern char d_cookies[60];
extern char d_home[60];

#ifdef __cplusplus
}
#endif

#endif
