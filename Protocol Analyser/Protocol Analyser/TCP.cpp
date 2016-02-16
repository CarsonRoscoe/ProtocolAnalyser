/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: TCP.cpp
--
-- PROGRAM: ProtocolAnalyzer
--
-- METHODS:
-- void TCP::SendPacket(size_t port, char* IP, size_t packetSize, size_t packetsToSend)
-- void TCP::Cleanup()
-- void TCP::StartServer(int port)
-- void TCP::ReceivePacket(size_t port, WPARAM wParam)
--
-- DATE: February 10th, 2016
--
-- REVISIONS: February 10th, 2016: Created TCP setup
--			  February 13th, 2016: Finished TCP
--
-- DESIGNER: Carson Roscoe
--
-- PROGRAMMER: Carson Roscoe
--
-- NOTES:
-- Method definitions for the TCP class. The TCP class is used to communicate via TCP. 
----------------------------------------------------------------------------------------------------------------------*/
#include "TCP.h"

#define EOT (char)17

extern HWND hwnd;
extern HWND textBoxResult;
struct	hostent	*hp;
struct	sockaddr_in server, client;
SOCKET connection;

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
-- INTERFACE: void TCP::StartServer(int port)
--
-- RETURN: void
--
-- NOTES:
-- Called by WndProc when a user changes the mode to UDP while in server mode. Utilizes WSAAsyncSelect allow
-- WndProc to receive events from the socket.
----------------------------------------------------------------------------------------------------------------------*/
void TCP::StartServer(int port) {
	DWORD red;
	SOCKET listenSocket;
	SOCKADDR_IN internetAddr;
	WSADATA wsaData;

	if ((red = WSAStartup(0x0202, &wsaData)) != 0) {
		perror("WSAStartup failed with error " + red);
		return;
	}

	if ((listenSocket = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		perror("socket() failed with error " + WSAGetLastError());
		return;
	}

	WSAAsyncSelect(listenSocket, hwnd, WM_SOCKET, FD_ACCEPT | FD_CLOSE);

	internetAddr.sin_family = AF_INET;
	internetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	internetAddr.sin_port = htons(port);

	if (bind(listenSocket, (PSOCKADDR)&internetAddr, sizeof(internetAddr)) == SOCKET_ERROR) {
		perror("bind() failed with error " + WSAGetLastError());
		return;
	}

	if (listen(listenSocket, 5)) {
		perror("listen() failed with error " + WSAGetLastError());
		return;
	}
}

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
-- INTERFACE: void TCP::SendPacket(size_t port, char* IP, size_t packetSize, size_t packetsToSend)
--
-- RETURN: void
--
-- NOTES:
-- Called by WndProc in WinMain whenever the send button is pressed. Sends generated data via TCP to the given IP
-- on the given port.
----------------------------------------------------------------------------------------------------------------------*/
void TCP::SendPacket(size_t port, char* IP, size_t packetSize, size_t packetsToSend) {
	DWORD sb;
	WSAOVERLAPPED overlapped;
	WSABUF dataBuff;
	char *buffer;
	INT err;
	WSADATA wsaData;
	WORD wVersionRequested;

	wVersionRequested = MAKEWORD(2, 2);

	//Setup WSA
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		WSACleanup();
		return;
	}

	//Create socket
	if ((connection = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET) {
		perror("Cannot create socket");
		WSACleanup();
		return;
	}

	// Initialize and set up the address structure
	memset((char *)&server, 0, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);

	//Resolve host
	if ((hp = gethostbyname(IP)) == NULL) {
		perror("Unknown server address");
		WSACleanup();
		return;
	}

	//Copy the server address
	memcpy((char *)&server.sin_addr, hp->h_addr, hp->h_length);

	//Connecting to the server
	if (connect(connection, (struct sockaddr *)&server, sizeof(server)) == -1) {
		perror("Can't connect to server");
		WSACleanup();
		return;
	}

	buffer = (char*)malloc(packetSize * sizeof(char));

	//Create WSA event
	if ((overlapped.hEvent = WSACreateEvent()) == WSA_INVALID_EVENT) {
		perror("WSACreateEvent failed");
		WSACleanup();
		return;
	}

	//Send data
	for (size_t i = 0; i < packetsToSend; i++) {
		int k = 0;
		for (size_t j = 0; j < packetSize; j++) {
			k = (j < 26) ? j : j % 26;
			buffer[j] = 'a' + k;
		}

		dataBuff.buf = buffer;
		dataBuff.len = packetSize;

		WSASend(connection, &dataBuff, 1, &sb, 0, &overlapped, NULL);
		WaitForSingleObject(&overlapped.hEvent, 1000);
		overlapped.hEvent = WSACreateEvent();
	}

	SleepEx(5, TRUE);
	closesocket(connection);
	WSACleanup();
};

/*------------------------------------------------------------------------------------------------------------------
-- METHOD: Accept
--
-- DATE: February 11th, 2016
--
-- REVISIONS: February 11th, 2016: Created Accept
--			  February 15th, 2016: Commented
--
-- DESIGNER: Carson Roscoe
--
-- PROGRAMMER: Carson Roscoe
--
-- INTERFACE: void TCP::Accept(WPARAM wParam)
--
-- RETURN: void
--
-- NOTES:
-- Called by WndProc when the WM_SOCKET's RD_ACCEPT call fires off
----------------------------------------------------------------------------------------------------------------------*/
void TCP::Accept(WPARAM wParam) {
	closesocket(connection);
	if ((connection = accept(wParam, NULL, NULL)) == INVALID_SOCKET) {
		perror("accept() failed with error " + WSAGetLastError());
		return;
	}

	WSAAsyncSelect(connection, hwnd, WM_SOCKET, FD_READ | FD_WRITE | FD_CLOSE);
}

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
-- INTERFACE: void TCP::Cleanup()
--
-- RETURN: void
--
-- NOTES:
-- Called by WndProc when the socket needs to be closed.
----------------------------------------------------------------------------------------------------------------------*/
void TCP::Cleanup() {
	closesocket(connection);
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
-- INTERFACE: void TCP::ReceivePacket(size_t port, WPARAM wParam)
--
-- RETURN: void
--
-- NOTES:
-- Called by WndProc in WinMain whenever it receives an event from the user that data is ready to be read from
-- the socket. Reads via TCP until nothing more can be read.
----------------------------------------------------------------------------------------------------------------------*/
void TCP::ReceivePacket(size_t port, WPARAM wParam) {
	char buf[BUFFSIZE];
	WSABUF buffer;
	buffer.len = BUFFSIZE;
	buffer.buf = buf;
	DWORD RecvBytes;
	DWORD Flags;
	long totalBytes = 0;
	char bytesReadString[64];
	int packetNum = 0;
	int timeout = 0;
	Flags = 0;

	//Get start time for timer
	SYSTEMTIME time;
	GetSystemTime(&time);
	WORD millis = (time.wSecond * 1000) + time.wMilliseconds;

	//Reading loop
	do {
		RecvBytes = 0;
		if (WSARecv(connection, &buffer, 1, &RecvBytes, &Flags, NULL, NULL) == SOCKET_ERROR) {
			if (WSAGetLastError() != WSAEWOULDBLOCK) {
				break;
			}
		}
		else {
			if (RecvBytes == 0) {
				if (timeout < 5) {
					timeout++;
					continue;
				}
				break;
			}
			timeout = 0;
			totalBytes += RecvBytes;
		}
	} while (true);

	if (totalBytes == 0)
		return;

	//Get end time for timer
	GetSystemTime(&time);
	DWORD endMillis = (time.wSecond * 1000) + time.wMilliseconds;

	//Calculate difference and store into bytesReadString
	sprintf(bytesReadString, "Bytes Read:%d Transfer Time(millis):%d", totalBytes, endMillis - millis);

	//Update UI with statistics
	SetWindowText(textBoxResult, bytesReadString);

	//Wait for server to finish
	SleepEx(100, false);

	//Close connection
	closesocket(connection);
};
