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
 
#include <nds.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dswifi9.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include "wifi.h"
#include "general.h"
#include "sound.h"
#include "settings.h"
#include ".\drawing\database.h"

static CONNECTION wifiConn[NUM_CONNECTIONS];
static bool wifiConnected = false;

void *sgIP_malloc(int size) 
{ 
	return safeMalloc(size); 
}

void sgIP_free(void * ptr) 
{ 
	safeFree(ptr); 
}

void Timer_50ms(void) 
{
	Wifi_Timer(50);
}

void arm9_synctoarm7() 
{
	SendArm7Command(0xF1, 0);
}

void getWifiSync()
{
	Wifi_Sync();
}

void initWifi()
{
	u32 Wifi_pass = Wifi_Init(WIFIINIT_OPTION_USELED);
	
    REG_IPC_FIFO_TX = 0xF0000000;
    REG_IPC_FIFO_TX = Wifi_pass;
   	
	*((volatile u16 *)0x0400010E) = 0; // disable timer3
	
	irqSet(IRQ_TIMER3, Timer_50ms); // setup timer IRQ
	irqEnable(IRQ_TIMER3);
   	
   	Wifi_SetSyncHandler(arm9_synctoarm7); // tell wifi lib to use our handler to notify arm7

	*((volatile u16 *)0x0400010C) = (u16)-6553; // 6553.1 * 256 cycles = ~50ms;
	*((volatile u16 *)0x0400010E) = 0x00C2; // enable, irq, 1/256 clock
	
	while(Wifi_CheckInit()==0) 	// wait for arm7 to be initted successfully
		vblankWait();
	
	// init ssl
	InitCyaSSL();
	
	// init the ssl settings block
	for(int i = 0;i < NUM_CONNECTIONS; i++)
	{
		wifiConn[i].sock = -1;
	}
}

int findValidSockID(int sock)
{
	// Invalid socket
	if(sock == -1)
	{
		return -1;
	}
	
	// First look for valid socket
	for(int findSock = 0; findSock < NUM_CONNECTIONS; findSock++)
	{
		if(wifiConn[findSock].sock == sock)
		{
			return findSock;
		}
	}
	
	// Doesn't exist so we will return first empty
	for(int findSock = 0; findSock < NUM_CONNECTIONS; findSock++)
	{
		if(wifiConn[findSock].sock == -1)
		{
			wifiConn[findSock].sock = sock;
			return findSock;
		}
	}
	
	// We are full up
	return -1;
}

int doConnect(int sock, struct sockaddr *tcp_sain, int size, bool useSSL)
{
	if(useSSL)
	{
		// first connect
		setBlocking(sock);
		connect(sock, tcp_sain, size);
		
		// find a valid socket id
		int id = findValidSockID(sock);
		
		if(id == -1)
		{
			// couldn't get valid socket id
			closesocket(sock);
			return -1;
		}
		
		wifiConn[id].isSSL = true;
		
		wifiConn[id].method = 0;
		wifiConn[id].ctx = 0;
		wifiConn[id].ssl = 0;
		
		wifiConn[id].method = TLSv1_client_method();
		wifiConn[id].ctx = SSL_CTX_new(wifiConn[id].method);
		
		SSL_CTX_set_verify(wifiConn[id].ctx, SSL_VERIFY_NONE, 0);
		
		wifiConn[id].ssl = SSL_new(wifiConn[id].ctx);
		SSL_set_fd(wifiConn[id].ssl, sock);
		
		if(SSL_connect(wifiConn[id].ssl) != SSL_SUCCESS)
		{
			// couldn't get valid socket id
			wifiClose(sock);
			return -1;
		}
	}
	else
	{	
		// just connect
		setNonBlocking(sock);
		connect(sock, tcp_sain, size);
		
		// ensure we've marked that the socket is not ssl
		int id = findValidSockID(sock);
		
		if(id != -1)
		{
			wifiConn[id].isSSL = false;
		}
		else
		{
			// couldn't get valid socket id
			closesocket(sock);
			return -1;
		}
	}
	
	return sock;
}

int wifiConnect(char *host, unsigned short port, bool useSSL)
{
	int sock = 0;
	struct hostent * tcp_he;
	struct sockaddr_in tcp_sain;
	
	tcp_he = gethostbyname(host);
	
	if(tcp_he == 0)
	{
		return -1;
	}
	
	sock = socket(AF_INET,SOCK_STREAM,0);
	tcp_sain.sin_addr.s_addr = *((unsigned long *)(tcp_he->h_addr_list[0]));
	tcp_sain.sin_family = AF_INET;
    tcp_sain.sin_port = htons(port);
	
	return doConnect(sock, (struct sockaddr *)&tcp_sain, sizeof(tcp_sain), useSSL);
}

int wifiConnectIP(u32 ip, unsigned short port, bool useSSL)
{
	int sock = 0;
	struct sockaddr_in tcp_sain;	
	
	sock = socket(AF_INET,SOCK_STREAM,0);
	tcp_sain.sin_addr.s_addr = ip;
	tcp_sain.sin_family = AF_INET;
    tcp_sain.sin_port = htons(port);
	
	return doConnect(sock, (struct sockaddr *)&tcp_sain, sizeof(tcp_sain), useSSL);
}

void wifiClose(int sock)
{
	int id = findValidSockID(sock);
	
	if(wifiConn[id].isSSL)
	{
		SSL_shutdown(wifiConn[id].ssl);
		SSL_free(wifiConn[id].ssl);
		SSL_CTX_free(wifiConn[id].ctx);
	}
	
	closesocket(sock); // close socket for good
	
	wifiConn[id].isSSL = false;
	wifiConn[id].sock = -1;
}

void setBlocking(int sock)
{
	int i = 0;
    ioctl(sock, FIONBIO, &i); // set blocking
}

void setNonBlocking(int sock)
{
	int i = 1;
    ioctl(sock, FIONBIO, &i); // set non-blocking
}

void sendData(int sock, char *data, int len)
{
	int id = findValidSockID(sock);
	
	if(wifiConn[id].isSSL)
	{
		SSL_write(wifiConn[id].ssl, data, len);
	}
	else
	{
		send(sock, data, len, 0);
	}
}

void sendString(int sock, char *data)
{
	if(data)
	{
		sendData(sock, data, strlen(data));
	}
}

int recvData(int sock, char *data, int len)
{	
	int id = findValidSockID(sock);
	
	if(wifiConn[id].isSSL)
	{
		return SSL_read(wifiConn[id].ssl, data, len);
	}
	else
	{
		return recv(sock, data, len, 0);
	}
}

u32 ipToLong(char i1, char i2, char i3, char i4)
{
	u32 l;
	
	l = i1 | (i2 << 8) | (i3 << 16) | (i4 << 24);
	
	return l;
}

u32 stringToLong(char *inIP)
{
	// first verify that it indeed has only numbers and 3 separating dots
	
	int sepDots = 0;
	
	for(u32 i=0;i<strlen(inIP);i++)
	{
		if(inIP[i] == '.')
		{
			// keep tracks of number of dots
			sepDots++;
		}
		else if(!isNumerical(inIP[i]))
		{
			// exit with null ip if they went retarded
			// and entered letters
			return 0;
		}
	}
	
	if(sepDots != 3)
	{
		// they don't have the necessary areas
		return 0;
	}
	
	int tPos = 0;
	int tVal = 0;
	char tIP[4];
	
	// transverse the string and extract the actual data
	for(u32 i=0;i<=strlen(inIP);i++)
	{
		if(inIP[i] == '.' || inIP[i] == 0)
		{
			// make sure the value is < 256 (range of 0-255 per cell)
			if(tVal < 256)
			{
				tIP[tPos] = tVal;
			}
			else
			{
				// invalid ip
				return 0;
			}
			
			tPos++;
			tVal = 0;
		}
		else
		{
			tVal *= 10;
			tVal = tVal + (inIP[i] - '0');
		}
	}
	
	return ipToLong(tIP[0], tIP[1], tIP[2], tIP[3]);
}

void longToString(u32 ip, char *out)
{
	sprintf(out, "%d.%d.%d.%d", ip & 0xFF, (ip >> 8) & 0xFF, (ip >> 16) & 0xFF, (ip >> 24) & 0xFF);
}

void disconnectWifi()
{
	if(!wifiConnected)
		return;
	
	wifiConnected = false;
	
	Wifi_DisconnectAP();
	Wifi_DisableWifi();
}

void connectWifi()
{
	if(wifiConnected)
		return;
	
	wifiConnected = true;
	
	Wifi_AutoConnect();	
}

bool isConnected()
{
	return wifiConnected;
}
