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
 
#ifndef _HTTP_INCLUDED
#define _HTTP_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#define ICY_PARSE_SIZE 1024
#define HTTP_HEADER_EXTRA 256
#define ICY_REQUEST_SIZE 512

typedef struct
{
	char username[30];
	char password[30];
	char server[60];
	u32 serverIP;
	unsigned short serverPort;
	char *remotePath;
	void *remoteSentinal;
	bool isSSL;
} URL_TYPE;

typedef struct
{
	//char icyNotice1[1024];
	//char icyNotice2[1024];
	char icyName[1024];
	char icyGenre[64];
	char icyURL[128];
	char icyCurSong[1024];
	char icyMimeType[32];
	bool icyPublic;
	int icyBitrate;
	int icyMetaInt;
} ICY_HEADER;

bool isURL(char *url);
bool urlFromString(char *url, URL_TYPE *site);
void destroyURL(URL_TYPE *site);
void fillIPFromServer(URL_TYPE *site);
void stripHTTP(char *str);
bool parseIcyHeader(char *header, ICY_HEADER *outFile);
u32 getHTTPLength(char *header);
bool getHTTPHeaderValue(char *header, char *search, char *out);
int getHTTPReplyCode(char *header);
int sendHTTPRequest(char *req);
int sendHTTPRequestRaw(char *req, char *uAgent, char *postData, char *referData, bool allowCompression);
int sendStreamRequest(u32 ip, unsigned short port, char *remotePath, bool useMeta);
void setProxy(bool enabled, char *address);

#ifdef __cplusplus
}
#endif

#endif
