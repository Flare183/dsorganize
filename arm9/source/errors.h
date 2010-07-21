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
 
#ifndef _ERRORS_INCLUDED
#define _ERRORS_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#define ERROR_NONE 0

#define ERROR_DB_TEMPSPACE_MOTD 0xA00
#define ERROR_DB_TEMPSPACE_FILE 0xA01
#define ERROR_DB_TEMPURL 0xA02

#define ERROR_WEB_RENDERSPACE 0xF00
#define ERROR_WEB_HISTORY 0xF01
#define ERROR_WEB_SEARCHTEMP 0xF02
#define ERROR_WEB_SEARCHPREF 0xF03
#define ERROR_WEB_FAVORITES 0xF04
#define ERROR_WEB_FAVORITESTEMP 0xF05
#define ERROR_WEB_ADDFAVORITES 0xF06
#define ERROR_WEB_CAPTURED 0xF07
#define ERROR_WEB_DLQUEUECSS 0xF08
#define ERROR_WEB_DLQUEUEIMG 0xF09
#define ERROR_WEB_MASKPASSWORD 0xF10
#define ERROR_WEB_INPUTBUF 0xF11
#define ERROR_WEB_RENDERIMAGE 0xF12
#define ERROR_WEB_CREATEURL 0xF13

#define ERROR_IRC_FINDURL 0xB00
#define ERROR_IRC_SCREENBUFFER 0xB01
#define ERROR_IRC_COLORBUFFER 0xB02
#define ERROR_IRC_BACKBUFFER 0xB03
#define ERROR_IRC_NICKLIST 0xB04
#define ERROR_IRC_TABARRAY 0xB05
#define ERROR_IRC_DRAWTABS 0xB06
#define ERROR_IRC_KICKREASON 0xB07
#define ERROR_IRC_PARSEMESSAGE 0xB08
#define ERROR_IRC_ACTIVATETAB 0xB09
#define ERROR_IRC_INPUTBUFFER 0xB10
#define ERROR_IRC_RECEIVEBUFFER 0xB11
#define ERROR_IRC_TEMPRECV 0xB12

#define ERROR_COOKIES_CREATE 0xC00

#define ERROR_SOUND_FREEPLAYLIST 0xD00

#ifdef __cplusplus
}
#endif

#endif
