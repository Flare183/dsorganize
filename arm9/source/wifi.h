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
 
#ifndef _WIFI_INCLUDED
#define _WIFI_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <dswifi9.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <openssl/ssl.h>

#define NUM_CONNECTIONS 10
#define DB_DISCONNECTED 0

typedef struct
{
	int sock;
	bool isSSL;	
    SSL_METHOD* method;
    SSL_CTX* ctx;
    SSL* ssl;
    SSL_SESSION* session;
} CONNECTION;

void *sgIP_malloc(int size);
void sgIP_free(void * ptr);

void initWifi();
void getWifiSync();

int wifiConnect(char *host, unsigned short port, bool useSSL);
int wifiConnectIP(u32 ip, unsigned short port, bool useSSL);
u32 ipToLong(char i1, char i2, char i3, char i4);
u32 stringToLong(char *inIP);
void longToString(u32 ip, char *out);
void sendData(int sock, char *data, int len);
void sendString(int sock, char *data);
int recvData(int sock, char *data, int len);
void wifiClose(int sock);

void disconnectWifi();
void connectWifi();
bool isConnected();

void setBlocking(int sock);
void setNonBlocking(int sock);

#ifdef __cplusplus
}
#endif

#endif
