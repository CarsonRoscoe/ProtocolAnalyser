#pragma once

#include <Windowsx.h>
#include "TCP.h"
#include "UDP.h"
#include "resource.h"
#include "UIWrappers.h"

#define _CRT_SECURE_NO_WARNINGS
#define PROGRAM_TITLE "Protocol Analyzer"

#pragma warning (disable: 4096)

enum ProtocolMode {
	TCPmode, UDPmode
};

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
int InitWindow(HINSTANCE hInstance);
void ChangeMode(WPARAM wParam);
void Update(int mode);

HWND hwnd;
HWND textBoxResult;
HWND labelIP;
HWND editTextIP;
HWND labelPort;
HWND labelProtocol;
HWND editTextPort;
HWND labelPacketSize;
HWND labelQuantity;
HWND comboBox1;
HWND comboBox2;
HWND comboBox3;
HWND buttonSend;
HDC hdc;
TCP tcpConnection(Client);
UDP udpConnection(Client);
Mode mode = Client;
ProtocolMode protocol = TCPmode;