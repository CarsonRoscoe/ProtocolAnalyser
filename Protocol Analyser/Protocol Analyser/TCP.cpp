#include "TCP.h"

#define BUFFER 64
#define IPBUFFER 16
#define PROTOCOL "tcp"
#define EOT (char)3

extern HWND hwnd;
extern HWND textBoxResult;
struct hostent *hp;
struct sockaddr_in server, client;

#define CreateThread(func,id) CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)(func),(LPVOID)hwnd,0,(LPDWORD)(id))

size_t size;
u_short port;
size_t quantity;
char IP[IPBUFFER];
SOCKET connection;
DWORD threadId;
HANDLE thread;

TCP::TCP(Mode mode) {
}

TCP::~TCP() {
	TerminateThread(thread, 1);
}

void TCP::StartServer() {
	DWORD Ret;
	SOCKET Listen;
	SOCKADDR_IN InternetAddr;
	WSADATA wsaData;

	if ((Ret = WSAStartup(0x0202, &wsaData)) != 0) {
		printf("WSAStartup failed with error %d\n", Ret);
		return;
	}

	if ((Listen = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		printf("socket() failed with error %d\n", WSAGetLastError());
		return;
	}

	WSAAsyncSelect(Listen, hwnd, WM_SOCKET, FD_ACCEPT | FD_CLOSE);

	InternetAddr.sin_family = AF_INET;
	InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	InternetAddr.sin_port = htons(DEFAULTPORT);

	if (bind(Listen, (PSOCKADDR)&InternetAddr, sizeof(InternetAddr)) == SOCKET_ERROR) {
		printf("bind() failed with error %d\n", WSAGetLastError());
		return;
	}

	if (listen(Listen, 5)) {
		perror("listen() failed with error " + WSAGetLastError());
		return;
	}
}

void TCP::SendPacket(size_t port, char* IP, size_t size, size_t quantity) {
	DWORD sb;
	WSAOVERLAPPED ol;
	WSABUF DataBuff;
	char *Buffer;
	INT err;
	WSADATA WSAData;
	WORD wVersionRequested;

	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &WSAData);
	if (err != 0) {
		exit(1);
	}

	if ((connection = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET) {
		perror("Cannot create socket");
		exit(1);
	}

	// Initialize and set up the address structure
	memset((char *)&server, 0, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);

	if ((hp = gethostbyname(IP)) == NULL) {
		perror("Unknown server address");
		exit(1);
	}

	// Copy the server address
	memcpy((char *)&server.sin_addr, hp->h_addr, hp->h_length);

	// Connecting to the server
	if (connect(connection, (struct sockaddr *)&server, sizeof(server)) == -1) {
		perror("Can't connect to server");
		exit(1);
	}

	Buffer = (char*)malloc(size * sizeof(char));

	if ((ol.hEvent = WSACreateEvent()) == WSA_INVALID_EVENT) {
		perror("WSACreateEvent failed");
		exit(1);
	}

	/* Send the data */
	for (size_t i = 0; i < quantity; i++) {
		int k = 0;
		for (size_t j = 0; j < size; j++) {
			k = (j < 26) ? j : j % 26;
			Buffer[j] = 'a' + k;
		}

		if (i == quantity - 1)
			Buffer[size - 1] = EOT;

		DataBuff.buf = Buffer;
		DataBuff.len = size;
		if (WSASend(connection, &DataBuff, 1, &sb, 0, &ol, NULL) == SOCKET_ERROR) {
			int error = WSAGetLastError();
			perror("WSASend failed");
			exit(1);
		};
		Sleep(50);
	}

	closesocket(connection);
	WSACleanup();
};

void TCP::Accept(WPARAM wParam) {
	closesocket(connection);
	if ((connection = accept(wParam, NULL, NULL)) == INVALID_SOCKET)
	{
		perror("accept() failed with error " + WSAGetLastError());
		return;
	}

	CreateSocketInformation(connection);


	WSAAsyncSelect(connection, hwnd, WM_SOCKET, FD_READ | FD_WRITE | FD_CLOSE);
}

void TCP::Cleanup() {
	TerminateThread(thread, 1);
	closesocket(connection);
}

void TCP::ReceivePacket(size_t port, WPARAM wParam) {
	LPSOCKET_INFORMATION SocketInfo;
	DWORD RecvBytes;
	DWORD Flags;
	bool EOTRead = false;
	long totalBytes = 0;
	char bytesReadString[32];
	int packetNum = 1;
	do {
		SocketInfo = GetSocketInformation(wParam);

		SocketInfo->DataBuf.buf = SocketInfo->Buffer;
		SocketInfo->DataBuf.len = BUFFSIZE;

		Flags = 0;
		if (WSARecv(SocketInfo->Socket, &(SocketInfo->DataBuf), 1, &RecvBytes, &Flags, NULL, NULL) == SOCKET_ERROR) {
			if (WSAGetLastError() != WSAEWOULDBLOCK) {
				printf("WSARecv() failed with error %d\n", WSAGetLastError());
				FreeSocketInformation(wParam);
				return;
			}
		}
		else {
			SocketInfo->BytesRECV = RecvBytes;
			totalBytes += RecvBytes;
			if (RecvBytes == 0)
				continue;
			sprintf(bytesReadString, "%d", totalBytes);
			SetWindowText(textBoxResult, bytesReadString);
			if (SocketInfo->DataBuf.buf[RecvBytes - 1] == EOT) {
				EOTRead = true;
				printf("Found EOT\n");
			}
		}
	} while (!EOTRead);

	closesocket(connection);
};

LPSOCKET_INFORMATION SocketInfoList;

void CreateSocketInformation(SOCKET s)
{
	LPSOCKET_INFORMATION SI;

	if ((SI = (LPSOCKET_INFORMATION)GlobalAlloc(GPTR, sizeof(SOCKET_INFORMATION))) == NULL)
	{
		printf("GlobalAlloc() failed with error %d\n", GetLastError());
		return;
	}

	// Prepare SocketInfo structure for use.

	SI->Socket = s;
	SI->RecvPosted = FALSE;
	SI->BytesSEND = 0;
	SI->BytesRECV = 0;

	SI->Next = SocketInfoList;

	SocketInfoList = SI;
}

LPSOCKET_INFORMATION GetSocketInformation(SOCKET s)
{
	SOCKET_INFORMATION *SI = SocketInfoList;

	while (SI)
	{
		if (SI->Socket == s)
			return SI;

		SI = SI->Next;
	}

	return NULL;
}

void FreeSocketInformation(SOCKET s)
{
	SOCKET_INFORMATION *SI = SocketInfoList;
	SOCKET_INFORMATION *PrevSI = NULL;

	while (SI)
	{
		if (SI->Socket == s)
		{
			if (PrevSI)
				PrevSI->Next = SI->Next;
			else
				SocketInfoList = SI->Next;

			closesocket(SI->Socket);
			GlobalFree(SI);
			return;
		}

		PrevSI = SI;
		SI = SI->Next;
	}
}