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
 
#ifndef _DATABASE_INCLUDED
#define _DATABASE_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#define DB_DISCONNECTED 0
#define DB_CONNECTING 1
#define DB_CONNECTED 2
#define DB_FAILEDCONNECT 3
#define DB_RECIEVEDLIST 4
#define DB_GETLIST 6
#define DB_GETPACKAGE 7
#define DB_RECIEVEDPACKAGE 8
#define DB_GETTINGMOTD 9
#define DB_GETTINGLIST 10
#define DB_GETTINGPACKAGE 11
#define DB_FAILEDPACKAGE 12
#define DB_GETTINGFILE 13

#define MOTD_SIZE 2048
#define LIST_SIZE 100000
#define PACKAGE_SIZE 4096

#define URL_SIZE 512

#define TIMEOUT 180
#define RETRIES 3

#define CHDR 0
#define MKDR 1
#define DOWN 2
#define DELE 3
#define CLS  4
#define ECHO 5
#define WAIT 6

#define ENCODING_RAW 0
#define ENCODING_DEFLATE 1
#define ENCODING_GZIP 2

#define DECODE_CHUNK (32*1024)

#define DOWN_SIZE 8192 // 8kb recieve

typedef struct
{
	char category[33];
} CAT_LIST;

typedef struct
{
	int command;
	char instruction[256];
} INST_LIST;

typedef struct
{
	char name[31];
	char description[101];
	char dataURL[65];
	char date[11];
	char version[11];
	char size[11];
	char category[33];
} HB_LIST;

void initDatabase();
void customDB(char *str);
void customPackage(char *str);
void freeWifiMem();

// for webbrowser
void getFile(char *url, char *uAgent);
void checkFile();
void resetRCount();
void cancelDownload();
int getLastCharset();
int getLastContentType();
void setPostData(char *toPost);
void setReferrer(char *toRefer);
void setContentType(int ct);
bool isDownloading();
char *getDownStatus();
void runSpeed();
int getDownloadRatio(int max);

#ifdef __cplusplus
}
#endif

#endif
