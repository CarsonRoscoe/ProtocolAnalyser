#pragma once

#include "ProtocolBase.h"
#include <string>

class TCP: public Protocol {
public:
	//Public Methods
	TCP(Mode mode) {}; //Idle/Unused
	~TCP() {};
	void SendPacket(size_t port, char* IP, size_t packetSize, size_t packetsToSend);
	void Accept(WPARAM wParam);
	void ReceivePacket(size_t port, WPARAM wParam);
	void StartServer(int port);
	void Cleanup();

	//Public Variables
};

