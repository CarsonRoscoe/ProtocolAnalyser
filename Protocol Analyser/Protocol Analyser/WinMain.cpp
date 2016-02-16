/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: WinMain.cpp - Starting point of the entire applications & host of WndProc
--
-- PROGRAM: ProtocolAnalyzer
--
-- FUNCTIONS:
-- int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR pCmdLine, int nCmdShow)
-- LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
-- int InitWindow(HINSTANCE hInstance);
-- void ChangeMode(WPARAM wParam);
-- void Update(int mode);
--
-- DATE: February 9th, 2016
--
-- REVISIONS: February 9th, 2016: Started the program and created the base skeleton
--			  February 10th, 2016: Setup TCP
--			  February 11th, 2016: Setup UDP
--			  February 14th, 2016: Bug fixing
--			  February 15th, 2016: Commented
--
-- DESIGNER: Carson Roscoe
--
-- PROGRAMMER: Carson Roscoe
--
-- NOTES:
-- This program is used to analyze the differences in efficiency between TCP and UDP. It does this by allowing a user
-- to specify the data (packet size, amount, protocol, etc) that a user would like to send, and then also can act
-- as a server to receive this data. After both sending and receiving it, the application can tell the user the specifics
-- of how long it took to send/receive the data.
----------------------------------------------------------------------------------------------------------------------*/
#include "WinMain.h"

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: WinMain
--
-- DATE: February 9th, 2016
--
-- REVISIONS: February 9th, 2016: Created WinMain
--			  February 15th, 2016: Commented
--
-- DESIGNER: Carson Roscoe
--
-- PROGRAMMER: Carson Roscoe
--
-- INTERFACE: int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR pCmdLine, int nCmdShow)
--
-- RETURN: int regarding the exit code
--
-- NOTES:
-- Starting point for our application which calls the InitWindow function before going into the default
-- WIN32 GetMessage loop.
----------------------------------------------------------------------------------------------------------------------*/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR pCmdLine, int nCmdShow) {
	if (InitWindow(hInstance) == -1) {
		//Error, handled inside InitWindow
		return 1;
	};
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	// Run the message loop.
	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: WndProc
--
-- DATE: February 9th, 2016
--
-- REVISIONS: February 9th, 2016: Created WndProc
--			  February 10th, 2016: Added WSAAsyncSelect socket calls and hooked up UI events
--			  February 15th, 2016: Commented
--
-- DESIGNER: Carson Roscoe
--
-- PROGRAMMER: Carson Roscoe
--
-- INTERFACE: LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
--
-- RETURN: long pointer result regarding exit code
--
-- NOTES:
-- Invoked whenever either a menu item has triggered an event, or whenever our socket has triggered a Accept or Read
-- event.
----------------------------------------------------------------------------------------------------------------------*/
LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	char ip[64];
	char portStr[64];
	char sizeStr[64];
	char timesStr[64];
	int port = DEFAULTPORT;
	int size = 2000;
	int times = 3;
	LPOPENFILENAME fileName;

	switch (Message) {
	//On destroy window, cleanup
	case WM_DESTROY:
		tcpConnection.Cleanup();
		udpConnection.Cleanup();
		break;
	//On create window
	case WM_CREATE:
		hdc = GetDC(hwnd);

		ReleaseDC(hwnd, hdc);
		break;
	//On UI command...
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		//On button pressed to send data
		case IDM_SEND:
			//determine if client.
			if (mode == Client) {
				GetWindowText(editTextIP, ip, sizeof(ip));
				GetWindowText(editTextPort, portStr, sizeof(portStr));
				ComboBox_GetLBText(comboBox1, ComboBox_GetCurSel(comboBox1), sizeStr);
				ComboBox_GetLBText(comboBox2, ComboBox_GetCurSel(comboBox2), timesStr);

				port = atoi(portStr);
				size = atoi(sizeStr);
				times = atoi(timesStr);

				switch (protocol) {
				case TCPmode:
					tcpConnection.SendPacket(port, ip, size, times);
					break;
				case UDPmode:
					udpConnection.SendPacket(port, ip, size, times);
					break;
				}
			} else { //Else we are teh server
				tcpConnection.Cleanup();
				udpConnection.Cleanup();
				GetWindowText(editTextPort, portStr, sizeof(portStr));
				switch (protocol) {
				case TCPmode:
					tcpConnection.StartServer(atoi(portStr));
					break;
				case UDPmode:
					udpConnection.StartServer(atoi(portStr));
					break;
				}
			}
			break;
		//If we are changing to either the client or the server
		case ID_MODE_CLIENT:
		case ID_MODE_SERVER:
			ChangeMode(wParam);
			break;
		//By default, update UDP/TCP modes
		default:
			switch (ComboBox_GetCurSel(comboBox3)) {
			case 0:
				protocol = TCPmode;
				break;
			case 1:
				protocol = UDPmode;
				break;
			}
		}
		break;
	//On receiving socket commands
	case WM_SOCKET:
		if (mode != Server)
			return 0;
		if (WSAGETSELECTERROR(lParam)) {
			int errorCode = WSAGETSELECTERROR(lParam);
			perror("Socket failed with error " + errorCode);
		//If it isnt an error
		} else {
			switch (WSAGETSELECTEVENT(lParam)) {
			//On accept call, we are TCP that means so call our Accept method
			case FD_ACCEPT:
				tcpConnection.Accept(wParam);
				break;
			//On read call determine which protocol and call appropriate read
			case FD_READ:
				GetWindowText(editTextPort, portStr, sizeof(portStr));
				switch (protocol) {
				case TCPmode:
					tcpConnection.ReceivePacket(atoi(portStr), wParam);
					break;
				case UDPmode:
					udpConnection.ReceivePacket(atoi(portStr), wParam);
					break;
				}
				return 0;
			}
		}
		return 0;
	default:
		return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: InitWindow
--
-- DATE: February 9th, 2016
--
-- REVISIONS: February 9th, 2016: Created InitWindow
--			  February 15th, 2016: Commented
--
-- DESIGNER: Carson Roscoe
--
-- PROGRAMMER: Carson Roscoe
--
-- int InitWindow(HINSTANCE hInstance)
--
-- RETURN: int regarding success or fail
--
-- NOTES:
-- Creates the window, assigns UI  and gives them default values.
----------------------------------------------------------------------------------------------------------------------*/
int InitWindow(HINSTANCE hInstance) {
	WNDCLASSEX wcex;

	////Setup Window////
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	wcex.lpszClassName = PROGRAM_TITLE;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

	if (!RegisterClassEx(&wcex)) {
		DWORD errorCode = GetLastError();
		perror("RegisterClassEx Failed with error " + errorCode);
		return -1;
	}

	hwnd = CreateWindow(PROGRAM_TITLE, PROGRAM_TITLE, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 700, 500, NULL, NULL, hInstance, NULL);

	if (!hwnd) {
		DWORD errorCode = GetLastError();
		perror("CreateWindow Failed with error " + errorCode);
		return -1;
	}

	////Assign UI////
	labelIP = CreateLabel("IP ", MARGIN, MARGIN, LABELWIDTH, LABELHEIGHT);
	editTextIP = CreateEditText("127.0.0.1", EDITSTART + MARGIN, MARGIN, EDITWIDTH, EDITHEIGHT);
	labelPort = CreateLabel("Port ", MARGIN + LABELWIDTH + SPACING + EDITWIDTH, MARGIN, LABELWIDTH, LABELHEIGHT);
	editTextPort = CreateEditText("7000", LABELWIDTH * 2 + SPACING * 2 + EDITWIDTH, MARGIN, EDITWIDTH, EDITHEIGHT);

	labelPacketSize = CreateLabel("Size ", MARGIN, MARGIN + LABELHEIGHT + SPACING, LABELWIDTH, LABELHEIGHT);
	comboBox1 = CreateComboBox(EDITSTART + MARGIN, MARGIN + LABELHEIGHT + SPACING, EDITWIDTH, COMBOHEIGHT);

	labelQuantity = CreateLabel("Quantity ", MARGIN + LABELWIDTH + SPACING + EDITWIDTH, MARGIN + LABELHEIGHT + SPACING, LABELWIDTH, LABELHEIGHT);
	comboBox2 = CreateComboBox(LABELWIDTH * 2 + SPACING * 2 + EDITWIDTH, MARGIN + LABELHEIGHT + SPACING, EDITWIDTH, COMBOHEIGHT);

	labelProtocol = CreateLabel("Protocol ", MARGIN, MARGIN + LABELHEIGHT * 2 + SPACING * 2, LABELWIDTH, LABELHEIGHT);
	comboBox3 = CreateComboBox(EDITSTART + MARGIN, MARGIN + LABELHEIGHT*2 + SPACING*2, EDITWIDTH, COMBOHEIGHT);

	buttonSend = CreateButton("Send ", MARGIN + LABELWIDTH + SPACING + EDITWIDTH, MARGIN + LABELHEIGHT * 2 + SPACING * 2, LABELWIDTH + EDITWIDTH + SPACING, LABELHEIGHT);

	textBoxResult = CreateTextBox(MARGIN, MARGIN + LABELHEIGHT * 3 + SPACING * 3, MARGIN + LABELWIDTH + SPACING + EDITWIDTH + LABELWIDTH + EDITWIDTH, LABELHEIGHT * 5);

	////Give UI default values////
	ComboBox_AddString(comboBox1, "1024");
	ComboBox_AddString(comboBox1, "2048");
	ComboBox_AddString(comboBox1, "4096");
	ComboBox_AddString(comboBox1, "8192");
	ComboBox_AddString(comboBox1, "16384");
	ComboBox_AddString(comboBox1, "20000");
	ComboBox_AddString(comboBox1, "32768");
	ComboBox_AddString(comboBox1, "60000");
	ComboBox_AddString(comboBox1, "65536");
	ComboBox_AddString(comboBox2, "1");
	ComboBox_AddString(comboBox2, "10");
	ComboBox_AddString(comboBox2, "25");
	ComboBox_AddString(comboBox2, "50");
	ComboBox_AddString(comboBox2, "100");
	ComboBox_AddString(comboBox2, "200");
	ComboBox_AddString(comboBox2, "500");
	ComboBox_AddString(comboBox2, "1000");
	ComboBox_AddString(comboBox3, "TCP");
	ComboBox_AddString(comboBox3, "UDP");

	ComboBox_SetCurSel(comboBox1, 0);
	ComboBox_SetCurSel(comboBox2, 0);
	ComboBox_SetCurSel(comboBox3, 0);

	return 0;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: ChangeMode
--
-- DATE: February 14th, 2016
--
-- REVISIONS: February 14th, 2016: Created ChangeMode
--
-- DESIGNER: Carson Roscoe
--
-- PROGRAMMER: Carson Roscoe
--
-- void ChangeMode(WPARAM wParam)
--
-- RETURN: void
--
-- NOTES:
-- Used when the user changes between Client and Server mode in the UI
----------------------------------------------------------------------------------------------------------------------*/
void ChangeMode(WPARAM wParam) {
	char portStr[32];
	GetWindowText(editTextPort, portStr, sizeof(portStr));
	switch (LOWORD(wParam)) {
	case ID_MODE_CLIENT:
		mode = Client;
		Update(ID_MODE_CLIENT);
		break;
	case ID_MODE_SERVER:
		switch (protocol) {
		case TCPmode:
			tcpConnection.StartServer(atoi(portStr));
			break;
		case UDPmode:
			udpConnection.StartServer(atoi(portStr));
			break;
		}
		mode = Server;
		Update(ID_MODE_SERVER);
		break;
	}
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: Update
--
-- DATE: February 14th, 2016
--
-- REVISIONS: February 14th, 2016: Created Update
--
-- DESIGNER: Carson Roscoe
--
-- PROGRAMMER: Carson Roscoe
--
-- void ChangeMode(WPARAM wParam)
--
-- RETURN: void
--
-- NOTES:
-- Used when the user changes between Client and Server mode in the UI. Changes what UI elements are visible
-- and which elements are not. Also changes the name of the Send/Refresh button to the other.
----------------------------------------------------------------------------------------------------------------------*/
void Update(int mode) {
	switch (mode) {
	//Design UI for client
	case ID_MODE_CLIENT:
		ShowWindow(labelIP, TRUE);
		ShowWindow(labelPort, TRUE);
		ShowWindow(labelPacketSize, TRUE);
		ShowWindow(labelQuantity, TRUE);
		ShowWindow(labelProtocol, TRUE);
		ShowWindow(editTextPort, TRUE);
		ShowWindow(editTextIP, TRUE);
		ShowWindow(comboBox1, TRUE);
		ShowWindow(comboBox2, TRUE);
		ShowWindow(comboBox3, TRUE);
		SendMessage(buttonSend, WM_SETTEXT, 0, (LPARAM)"Send");
		break;
	//Design UI for server
	case ID_MODE_SERVER:
		ShowWindow(labelIP, FALSE);
		ShowWindow(labelPacketSize, FALSE);
		ShowWindow(labelQuantity, FALSE);
		ShowWindow(editTextIP, FALSE);
		ShowWindow(comboBox1, FALSE);
		ShowWindow(comboBox2, FALSE);
		SendMessage(buttonSend, WM_SETTEXT, 0, (LPARAM)"Refresh");
		break;
	}
}

