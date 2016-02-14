#pragma once

#include "ProtocolBase.h"
#include <string>

typedef struct _SOCKET_INFORMATION {
	BOOL RecvPosted;
	CHAR Buffer[BUFFSIZE];
	WSABUF DataBuf;
	SOCKET Socket;
	DWORD BytesSEND;
	DWORD BytesRECV;
	_SOCKET_INFORMATION *Next;
} SOCKET_INFORMATION, *LPSOCKET_INFORMATION;

void CreateSocketInformation(SOCKET s);
LPSOCKET_INFORMATION GetSocketInformation(SOCKET s);
void FreeSocketInformation(SOCKET s);


class TCP: public Protocol {
public:
	//Public Methods
	TCP(Mode mode); //Idle/Unused
	~TCP();
	void SendPacket(size_t port, char* IP, size_t packetSize, size_t packetsToSend);
	void Accept(WPARAM wParam);
	void ReceivePacket(size_t port, WPARAM wParam);
	void StartServer();
	void Cleanup();

	//Public Variables
};

