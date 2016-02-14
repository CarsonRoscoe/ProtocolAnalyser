#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment (lib,"ws2_32.lib")
#include <WinSock2.h>

#define DEFAULTPORT 7000
#define BUFFSIZE 200000
#define WM_SOCKET (WM_USER + 1)

enum Mode {
	Server, Client
};

class Protocol {
public:
	//Public Methods
	Protocol() {};
	~Protocol() {};
	virtual void SendPacket(size_t port, char* IP, size_t packetSize, size_t packetsToSend) = 0;

	//Public Variables

protected:
	//Protected Methods

	//Protected Variables
	Mode mode;
	virtual void ReceivePacket(size_t port, WPARAM wParam) = 0;

private:
	//Private Methods

	//Private Variables
};

