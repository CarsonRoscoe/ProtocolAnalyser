#pragma once

#include "ProtocolBase.h"
#include <WinSock2.h>
#include <string>

class UDP: public Protocol{
public:
	UDP() {}
	UDP(Mode mode); //Idle/Unused
	~UDP();
	void SendPacket(size_t port, char* IP, size_t packetSize, size_t packetsToSend);

protected:
	void ReceivePacket(size_t port, WPARAM wParam);
};

