#include "WinMain.h"

#define _CRT_SECURE_NO_WARNINGS

#pragma warning (disable: 4096)

HWND hwnd;
HDC hdc;
TCP tcpConnection;
UDP udpConnection;
Protocol &protocol;


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hprevInstance, LPSTR lspszCmdParam, int nCmdShow)
{
	MSG Msg;
	
	protocol = tcpConnection;

	ShowWindow(hwnd, nCmdShow);

	UpdateWindow(hwnd);


	while (GetMessage(&Msg, NULL, 0, 0))
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	return Msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	HMENU hMenu;
	PAINTSTRUCT paintstruct;

	switch (Message)
	{
	case WM_CREATE:
		hdc = GetDC(hwnd);

		ReleaseDC(hwnd, hdc);
		break;

		break;
	default:
		return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}

void InstantiateWindow(HINSTANCE hInst)
{
	WNDCLASSEX Wcl;
	char Name[] = "Windows Network Resolver";

	Wcl.cbSize = sizeof(WNDCLASSEX);
	Wcl.style = CS_HREDRAW | CS_VREDRAW;
	Wcl.hIcon = LoadIcon(NULL, IDI_APPLICATION);	// large icon 
	Wcl.hIconSm = NULL;								// use small version of large icon
	Wcl.hCursor = LoadCursor(NULL, IDC_ARROW);		// cursor style

	Wcl.lpfnWndProc = WndProc;
	Wcl.hInstance = hInst;
	Wcl.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH); //white background
	Wcl.lpszClassName = Name;

	Wcl.lpszMenuName = "MYMENU";	// The menu Class
	Wcl.cbClsExtra = 0;				// no extra memory needed
	Wcl.cbWndExtra = 0;

	if (!RegisterClassEx(&Wcl))
		return;

	hwnd = CreateWindow(Name, Name, WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU,
		50, 50, 700, 500, NULL, NULL, hInst, NULL);

	//memset(buffer, 0, sizeof(buffer));
}