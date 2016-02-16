#pragma once

#include "ProtocolBase.h"
#include <WinSock2.h>
#include <string>

class UDP: public Protocol{
public:
	UDP() {}
	UDP(Mode m) {}; //Idle/Unused
	~UDP() {};
	void SendPacket(size_t port, char* IP, size_t packetSize, size_t packetsToSend);
	void StartServer(int port);
	void ReceivePacket(size_t port, WPARAM wParam);
	void Cleanup();
};

