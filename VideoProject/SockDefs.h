#ifndef _SOCK_DEFS_H_
#define _SOCK_DEFS_H_

#include <ws2tcpip.h>

#define SAFE_CLOSESOCK(s) if(s!=INVALID_SOCKET) {closesocket(s);s=INVALID_SOCKET;}

inline bool get_IP(char* szIP, char* szUrl)
{
	struct addrinfo hints, *addrs;
	memset(&hints,0,sizeof(hints));

	int err = getaddrinfo(szUrl, NULL, &hints, &addrs);
	if(err) return false;

	inet_ntop(addrs->ai_family, &((SOCKADDR_IN *)addrs->ai_addr)->sin_addr, szIP, 64);

	freeaddrinfo(addrs);

	return true;
}
inline bool tcp_listen(SOCKET sock, int iPort)
{
	SOCKADDR_IN addrs;
	memset(&addrs, 0, sizeof(SOCKADDR_IN));

	addrs.sin_family = AF_INET;
	addrs.sin_addr.s_addr = htonl(INADDR_ANY);
	addrs.sin_port = htons(iPort);

	bool b_listen = false;
	int err = ::bind(sock, (SOCKADDR*)&addrs, sizeof(addrs));
	if(!err)
	{
		if(listen(sock, SOMAXCONN) == 0)
			b_listen = true;
	}
	return b_listen;
}
inline bool tcp_listen(SOCKET sock, char* szIP, int iPort)
{
	struct addrinfo hints, *addrs;
	memset(&hints,0,sizeof(hints));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	char szPort[16] = {};
	sprintf_s(szPort, "%d", iPort);

	int err = getaddrinfo(szIP, szPort, &hints, &addrs);
	if(err) return false;

	bool b_listen = false;
	err = ::bind(sock, addrs->ai_addr, (int)addrs->ai_addrlen);
	if(!err)
	{
		if(listen(sock, SOMAXCONN) == 0)
			b_listen = true;
	}

	freeaddrinfo(addrs);
	return b_listen;
}
inline bool tcp_connect(SOCKET sock, char* szUrl, int iPort, int iWait = 5)
{
	struct addrinfo hints, *addrs;
	memset(&hints,0,sizeof(hints));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	char szPort[16] = {};
	sprintf_s(szPort, "%d", iPort);

	int err = getaddrinfo(szUrl, szPort, &hints, &addrs);
	if(err) return false;

	bool b_connect = false;
	if(iWait)
	{
		// Set Nonblocking mode
		u_long argp = 1L;
		err = ioctlsocket(sock, FIONBIO, (u_long FAR *)&argp);
		if(!err)
		{
			connect(sock, addrs->ai_addr, (int)addrs->ai_addrlen);

			// Set blocking mode
			argp = 0;
			err = ioctlsocket(sock, FIONBIO, (u_long FAR *)&argp);
			if(!err)
			{
				timeval tv = {(long)iWait,0L};
				fd_set fdSet, fdE, fdW;
				FD_ZERO(&fdSet);

				FD_SET(sock,&fdSet);
				fdE = fdW = fdSet;
				if(select(0, 0, &fdW, &fdE, &tv) > 0)
				{
					if(FD_ISSET(sock, &fdW) && !FD_ISSET(sock, &fdE))
						b_connect = true;
				}
			}
		}
	}
	else
	{
		if(connect(sock, addrs->ai_addr, (int)addrs->ai_addrlen) == 0)
			b_connect = true;
	}

	freeaddrinfo(addrs);
	return b_connect;
}
inline int tcp_send(SOCKET sock, char * buf, int len)
{
	if(sock != INVALID_SOCKET)
	{
		int nSend = 0;
		if(len > 0)
		{
			while(len > nSend)
			{
				int nRet = send(sock, (char*)(buf + nSend), (len - nSend), 0);
				if(nRet <= 0) return SOCKET_ERROR;
				nSend += nRet;
			}
		}
		return nSend;
	}
	return SOCKET_ERROR;
}
inline int tcp_recv(SOCKET sock, char * buf, int len, int wait = 0)
{
	if(sock != INVALID_SOCKET)
	{
		int nRecv = 0;
		if(len > 0)
		{
			while(len > nRecv)
			{
				if(wait)
				{
					fd_set fdset;
					FD_ZERO(&fdset);
					FD_SET(sock,&fdset);

					timeval tv = {wait,0L};
					int result = select(FD_SETSIZE,&fdset,0,0,&tv);
					if(result < 0) return SOCKET_ERROR;
					else if(result == 0) return 0; // time out
					if(FD_ISSET(sock, (fd_set FAR *)&fdset) < 0) return SOCKET_ERROR;
				}
				int nRet = recv(sock, (char*)(buf + nRecv), (len - nRecv), 0);
				if(nRet <= 0) return SOCKET_ERROR;
				nRecv += nRet;
			}
		}
		return nRecv;
	}
	return SOCKET_ERROR;
}

enum
{
	CMD_OK = 1,
	CMD_FAIL,
	CMD_DISCONNECT,
	
	CMD_SEND_STREAM,
};
struct VMS_header
{
	char marker[4];
	int cmd;
	int size;

	VMS_header()
	{
		memset(this->marker, 0, 4);
		this->marker[0] = 'v';
		this->marker[1] = 'm';
		this->marker[2] = 's';
		this->cmd = 0;
		this->size = 0;
	}
};

#endif // _SOCK_DEFS_H_
