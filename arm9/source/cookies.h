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
 
#ifndef _COOKIES_INCLUDED
#define _COOKIES_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_TLD 21

#define MAX_DOMAIN 63
#define MAX_PATH 127
#define MAX_NAME 59

typedef struct
{
	char domain[MAX_DOMAIN+1];
	char path[MAX_PATH+1];
	char name[MAX_NAME+1];
	//char wasSent;             // Possibly for caching
	u32 checksum;
} COOKIE_TYPE;

void addCookie(char *url, char *header);
void sendCookies(int sock, char *server, char *path);

#ifdef __cplusplus
}
#endif

#endif
