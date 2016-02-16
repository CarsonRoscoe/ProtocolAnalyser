/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: UDP.cpp
--
-- PROGRAM: ProtocolAnalyzer
--
-- METHODS:
-- void UDP::SendPacket(size_t port, char* IP, size_t packetSize, size_t packetsToSend) 
-- void UDP::Cleanup()
-- void UDP::StartServer(int port)
-- void UDP::ReceivePacket(size_t port, WPARAM wParam)
--
-- DATE: February 11th, 2016
--
-- REVISIONS: February 11th, 2016: Created UDP setup
--			  February 14th, 2016: Finished UDP
--
-- DESIGNER: Carson Roscoe
--
-- PROGRAMMER: Carson Roscoe
--
-- NOTES:
-- Method definitions for the UDP class. The UDP class is used to communicate via UDP.
----------------------------------------------------------------------------------------------------------------------*/
#include "UDP.h"
#define EOT (char)17

//Externs
extern HWND hwnd;
extern HWND textBoxResult;
extern struct hostent *hp;
extern struct sockaddr_in server, client;

SOCKADDR_IN sockAddr;
SOCKET sock;

/*------------------------------------------------------------------------------------------------------------------
-- METHOD: SendPacket
--
-- DATE: February 11th, 2016
--
-- REVISIONS: February 11th, 2016: SendPacket
--			  February 15th, 2016: Commented
--
-- DESIGNER: Carson Roscoe
--
-- PROGRAMMER: Carson Roscoe
--
-- INTERFACE: void UDP::SendPacket(size_t port, char* IP, size_t packetSize, size_t packetsToSend)
--
-- RETURN: void
--
-- NOTES:
-- Called by WndProc in WinMain whenever the send button is pressed. Sends generated data via UDP to the given IP
-- on the given port.
----------------------------------------------------------------------------------------------------------------------*/
void UDP::SendPacket(size_t port, char* IP, size_t packetSize, size_t packetsToSend) {
	SOCKET dataSocket;
	WSAOVERLAPPED overlapped;
	WSABUF dataBuff;
	char *buffer;
	struct sockaddr_in server, si_other;
	int sLen, recvLen;
	WSADATA wsa;
	sLen = sizeof(si_other);

	//Initialise winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		WSACleanup();
		exit(EXIT_FAILURE);
	}

	//Create WSA event
	if ((overlapped.hEvent = WSACreateEvent()) == WSA_INVALID_EVENT) {
		WSACleanup();
		return;
	}

	//Create a datagram socket
	if ((dataSocket = WSASocket(AF_INET, SOCK_DGRAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET) {
		DWORD errorCode = WSAGetLastError();
		perror("Can't create a socket\n");
		WSACleanup();
		exit(1);
	}

	// Initialize and set up the address structure
	memset((char *)&server, 0, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);

	if ((hp = gethostbyname(IP)) == NULL) {
		WSACleanup();
		return;
	}

	//Copy the server address
	memcpy((char*)&server.sin_addr, hp->h_addr, hp->h_length);

	buffer = (char*)malloc(packetSize * sizeof(char));

	//Send data
	for (size_t i = 0; i < packetsToSend; i++) {
		int k = 0;
		for (size_t j = 0; j < packetSize; j++) {
			k = (j < 26) ? j : j % 26;
			buffer[j] = 'a' + k;
		}
		if (i == packetsToSend - 1)
			buffer[packetsToSend - 1] = EOT;

		dataBuff.buf = buffer;
		dataBuff.len = packetSize;

		DWORD sb;

		if (WSASendTo(dataSocket, &dataBuff, 1, &sb, 0, (PSOCKADDR)&server, sizeof(server), &overlapped, NULL) == SOCKET_ERROR) {
			DWORD errorCode = WSAGetLastError();
			if (errorCode != ERROR_IO_PENDING) {
				WaitForMultipleObjects(1, &overlapped.hEvent, true, 1000);
			}
		}

		overlapped.hEvent = WSACreateEvent();
	}

	WSACleanup();
};

/*------------------------------------------------------------------------------------------------------------------
-- METHOD: CleanUp
--
-- DATE: February 11th, 2016
--
-- REVISIONS: February 11th, 2016: Created Cleanup
--			  February 15th, 2016: Commented
--
-- DESIGNER: Carson Roscoe
--
-- PROGRAMMER: Carson Roscoe
--
-- INTERFACE: void UDP::Cleanup()
--
-- RETURN: void
--
-- NOTES:
-- Called by WndProc when the socket needs to be closed.
----------------------------------------------------------------------------------------------------------------------*/
void UDP::Cleanup() {
	closesocket(sock);
}

/*------------------------------------------------------------------------------------------------------------------
-- METHOD: StartServer
--
-- DATE: February 11th, 2016
--
-- REVISIONS: February 11th, 2016: Created StartServer
--			  February 15th, 2016: Commented
--
-- DESIGNER: Carson Roscoe
--
-- PROGRAMMER: Carson Roscoe
--
-- INTERFACE: void UDP::StartServer(int port)
--
-- RETURN: void
--
-- NOTES:
-- Called by WndProc when a user changes the mode to UDP while in server mode. Utilizes WSAAsyncSelect allow
-- WndProc to receive events from the socket.
----------------------------------------------------------------------------------------------------------------------*/
void UDP::StartServer(int port) {
	WSADATA data;

	if (WSAStartup(MAKEWORD(2, 2), &data) != 0) 
		return;

	//Create initial UDP socket for WASAsyncSelect call
	if (!(sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))){
		WSACleanup();
	}

	//Now we'll set the sockaddr variables:
	sockAddr.sin_family = AF_INET; 
	sockAddr.sin_addr.s_addr = INADDR_ANY; 
	sockAddr.sin_port = htons(port);

	//Bind socket
	if ((bind(sock, (SOCKADDR *)&sockAddr, sizeof(SOCKADDR)))) {
		WSACleanup();
	}

	WSAAsyncSelect(sock, hwnd, WM_SOCKET, FD_READ);
}

/*------------------------------------------------------------------------------------------------------------------
-- METHOD: ReceivePacket
--
-- DATE: February 11th, 2016
--
-- REVISIONS: February 11th, 2016: Created ReceivePacket
--			  February 15th, 2016: Commented
--
-- DESIGNER: Carson Roscoe
--
-- PROGRAMMER: Carson Roscoe
--
-- INTERFACE: void UDP::ReceivePacket(size_t port, WPARAM wParam)
--
-- RETURN: void
--
-- NOTES:
-- Called by WndProc in WinMain whenever it receives an event from the user that data is ready to be read from
-- the socket. Reads via UDP until nothing more can be read.
----------------------------------------------------------------------------------------------------------------------*/
void UDP::ReceivePacket(size_t port, WPARAM wParam) {
	char buf[BUFFSIZE];
	WSABUF buffer;
	buffer.len = BUFFSIZE;
	buffer.buf = buf;
	DWORD recvBytes;
	DWORD flags = 0;
	long totalBytes = 0;
	char bytesReadString[64];
	SOCKADDR from;
	short timeout = 0;
	int len = sizeof(SOCKADDR);
	int packets = 0;

	//Get first time for timer
	SYSTEMTIME time;
	GetSystemTime(&time);
	WORD millis = (time.wSecond * 1000) + time.wMilliseconds;

	//Read
	do {
		recvBytes = 0;
		if (WSARecvFrom(sock, &buffer, 1, &recvBytes, &flags, &from, &len, NULL, NULL) == SOCKET_ERROR) {
			DWORD errorCode = WSAGetLastError();
			if (errorCode == WSAEWOULDBLOCK) {
				if (buffer.buf[recvBytes - 1] == EOT)
					break;
				if (timeout < 10000) {
					timeout++;
					continue;
				}
				else {
					break;
				}
				//break;
			}
		}
		else {
			timeout = 0;
			totalBytes += recvBytes;
			packets++;
		}
	} while (true);

	if (totalBytes == 0)
		return;

	//Get second time for timer
	GetSystemTime(&time);
	DWORD endMillis = (time.wSecond * 1000) + time.wMilliseconds;

	//Comepare times
	sprintf(bytesReadString, "Bytes Read:%d Transfer Time(millis):%d Packets Sent:%d", totalBytes, endMillis - millis, packets);

	//Update UI with new data
	SetWindowText(textBoxResult, bytesReadString);
};
