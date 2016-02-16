#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment (lib,"ws2_32.lib")
#include <WinSock2.h>

#define DEFAULTPORT 7000
#define BUFFSIZE 1000000
#define WM_SOCKET (WM_USER + 1)
#define MAXTIMEOUT 1000

enum Mode {
	Server, Client
};

class Protocol {
public:
	//Public Methods
	Protocol() {};
	~Protocol() {};
	virtual void SendPacket(size_t port, char* IP, size_t packetSize, size_t packetsToSend) = 0;
	virtual void ReceivePacket(size_t port, WPARAM wParam) {};

	//Public Variables

protected:
	Mode mode;
};

