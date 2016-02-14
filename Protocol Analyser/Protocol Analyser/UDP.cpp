#include "UDP.h"

extern HWND hwnd;
extern HWND textBoxResult;
extern struct hostent *hp;
extern struct sockaddr_in server, client;

#define EOT (char)3

UDP::UDP(Mode mode) {

}

UDP::~UDP() {

}

void UDP::SendPacket(size_t port, char* IP, size_t packetSize, size_t packetsToSend) {
	char temp[BUFFSIZE];
	DWORD dwWritten;
	DWORD Index;
	DWORD BytesTransferred = 0;
	DWORD Flags;
	SOCKET DataSocket;
	WSAOVERLAPPED ol;
	WSABUF DataBuff;
	WSABUF ControlBuff;
	char *Buffer;
	DWORD TotalBytesSent;
	DWORD BytesRecv;
	SOCKET s;
	struct sockaddr_in server, si_other;
	int slen, recv_len;
	char buf[BUFFSIZE];
	WSADATA wsa;

	slen = sizeof(si_other);

	//Initialise winsock
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	printf("Initialised.\n");

	//Create a socket
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}

	if ((ol.hEvent = WSACreateEvent()) == WSA_INVALID_EVENT) {
		printf("WSACreateEvent failed with error %d\n", WSAGetLastError());
		return;
	}

	// Create a datagram socket
	if ((DataSocket = WSASocket(AF_INET, SOCK_DGRAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET) {
		DWORD errorCode = WSAGetLastError();
		perror("Can't create a socket\n");
		exit(1);
	}

	// Initialize and set up the address structure
	memset((char *)&server, 0, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);

	if ((hp = gethostbyname(IP)) == NULL) {
		fprintf(stderr, "Unknown server address\n");
		WSACleanup();
		return;
	}

	// Copy the server address
	memcpy((char*)&server.sin_addr, hp->h_addr, hp->h_length);

	Buffer = (char*)malloc(packetSize * sizeof(char));

	/* Send the data */
	for (size_t i = 0; i < packetsToSend; i++) {
		int k = 0;
		for (size_t j = 0; j < packetSize; j++) {
			k = (j < 26) ? j : j % 26;
			Buffer[j] = 'a' + k;
		}
		if (i == packetsToSend - 1)
			Buffer[packetSize - 1] = EOT;

		DataBuff.buf = Buffer;
		DataBuff.len = packetSize;

		DWORD sb;
		char temp[64];

		if (WSASendTo(DataSocket, &DataBuff, 1, &sb, 0, (PSOCKADDR)&server, sizeof(server), &ol, NULL) == SOCKET_ERROR) {
			if (WSAGetLastError() != ERROR_IO_PENDING) {
				sprintf(temp, "WSASend() failed with error %d", WSAGetLastError());
				MessageBox(NULL, temp, "", MB_OK);
				WSACleanup();
				return;
			}
		}
	}

	WSACleanup();
};

// Here is a structure contains the port we'll use,
// the protocol type and the IP address we'll communicate with.
SOCKADDR_IN sockaddr;

// This is our socket, it is the handle to the IO address to read/write packets
SOCKET sock;

void UDP::StartServer() {
	WSADATA data;

	if (WSAStartup(MAKEWORD(2, 2), &data) != 0) 
		return;

	// Here we create our socket, which will be a UDP socket (SOCK_DGRAM).
	if (!(sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))){
		// Creation failed!
	}

	//Now we'll set the sockaddr variables:
	sockaddr.sin_family = AF_INET; // Must be AF_INET
								   // If this is the Server:
	sockaddr.sin_addr.s_addr = INADDR_ANY; // Means we will "answer" to all addresses.

	sockaddr.sin_port = htons(DEFAULTPORT);

	// A server need to bind the socket to itself in order to receive all the packets
	// it gets from a port

	if ((bind(sock, (SOCKADDR *)&sockaddr, sizeof(SOCKADDR)))) {
		// Bind failed!
	}

	WSAAsyncSelect(sock, hwnd, WM_SOCKET, FD_READ);
}

void UDP::ReceivePacket(size_t port, WPARAM wParam) {
	char buf[BUFFSIZE];
	WSABUF buffer;
	buffer.len = BUFFSIZE;
	buffer.buf = buf;
	DWORD RecvBytes;
	DWORD Flags = 0;
	long totalBytes = 0;
	char bytesReadString[64];
	DWORD errorCode = 0;
	SOCKADDR from;
	short timeout = 0;
	int len = sizeof(SOCKADDR);
	do {
		RecvBytes = 0;
		if (WSARecvFrom(sock, &buffer, 1, &RecvBytes, &Flags, &from, &len, NULL, NULL) == SOCKET_ERROR) {
			errorCode = WSAGetLastError();
			if (errorCode == WSAEWOULDBLOCK) {
				if (timeout < 5) {
					timeout++;
					Sleep(1);
					continue;
				}
				break;
			}
		}
		else {
			timeout = 0;
			totalBytes += RecvBytes;
		}
	} while (true);

	if (totalBytes == 0)
		return;
	sprintf(bytesReadString, "%d", totalBytes);
	SetWindowText(textBoxResult, bytesReadString);
};
