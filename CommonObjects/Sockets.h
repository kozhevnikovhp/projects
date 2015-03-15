#ifndef __SOCKETS_H_INCLUDED__
#define __SOCKETS_H_INCLUDED__

#include "winsock2.h"

int InitSockets();
int FinitSockets();

SOCKET CreateUdpSocket(unsigned short PortNo, struct sockaddr_in *pSocketParams, BOOL bNeedBind);
void CloseSocket(SOCKET sock);

#endif // __SOCKETS_H_INCLUDED__