#include "WinMain.h"

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

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	char ip[64];
	char portStr[64];
	char sizeStr[64];
	char timesStr[64];
	int port = DEFAULTPORT;
	int size = 2000;
	int times = 3;

	switch (Message)
	{
	case WM_DESTROY:
		tcpConnection.Cleanup();
		break;
	case WM_CREATE:
		hdc = GetDC(hwnd);

		ReleaseDC(hwnd, hdc);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDM_SEND:
			GetWindowText(editTextIP, ip, sizeof(ip));
			GetWindowText(editTextPort, portStr, sizeof(portStr));
			ComboBox_GetLBText(comboBox1, ComboBox_GetCurSel(comboBox1), sizeStr);
			ComboBox_GetLBText(comboBox2, ComboBox_GetCurSel(comboBox2), timesStr);

			port = atoi(portStr);
			size = atoi(sizeStr);
			times = atoi(timesStr);

			udpConnection.SendPacket(port, ip, size, times);
			break;
		case ID_MODE_CLIENT:
		case ID_MODE_SERVER:
			ChangeMode(wParam);
			break;
		}
		break;
	case WM_SOCKET:
		if (mode != Server)
			return 0;
		
		if (WSAGETSELECTERROR(lParam)) {
			int errorCode = WSAGETSELECTERROR(lParam);
			perror("Socket failed with error " + errorCode);
			FreeSocketInformation(wParam);
		} else {
			switch (WSAGETSELECTEVENT(lParam)) {
			case FD_ACCEPT:
				tcpConnection.Accept(wParam);
				break;
			case FD_READ:
				udpConnection.ReceivePacket(DEFAULTPORT, wParam);
				return 0;
			}
		}
		return 0;
	default:
		return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}

int InitWindow(HINSTANCE hInstance) {

	WNDCLASSEX wcex;

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

	int labelWidth = 100, labelHeight = 25;
	int comboHeight = 100;
	int editStart = 110, editWidth = 140, editHeight = 30;
	int spacing = 60;
	int margin = 40;
	
	labelIP = CreateLabel("IP Address ", margin, margin, labelWidth, labelHeight);
	editTextIP = CreateEditText("127.0.0.1", editStart + margin, margin, editWidth, editHeight);
	labelPort = CreateLabel("Port ", margin + labelWidth + spacing + editWidth, margin, labelWidth, labelHeight);
	editTextPort = CreateEditText("7575", labelWidth*2 + spacing*2 + editWidth, margin, editWidth, editHeight);
	//TCPUDP drop down

	labelPacketSize = CreateLabel("Size (Bytes) ", margin, margin + labelHeight + spacing, labelWidth, labelHeight);
	comboBox1 = CreateComboBox(editStart + margin, margin + labelHeight + spacing, editWidth, comboHeight);

	labelQuantity = CreateLabel("Times to send", margin + labelWidth + spacing + editWidth, margin + labelHeight + spacing, labelWidth, labelHeight);
	comboBox2 = CreateComboBox(labelWidth * 2 + spacing * 2 + editWidth, margin + labelHeight + spacing, editWidth, comboHeight);

	labelProtocol = CreateLabel("Protocol", margin, margin + labelHeight * 2 + spacing * 2, labelWidth, labelHeight);
	comboBox3 = CreateComboBox(editStart + margin, margin + labelHeight*2 + spacing*2, editWidth, comboHeight);

	buttonSend = CreateButton("Send ", margin + labelWidth + spacing + editWidth, margin + labelHeight * 2 + spacing * 2, labelWidth + editWidth + spacing, labelHeight);

	textBoxResult = CreateTextBox(margin, margin + labelHeight * 3 + spacing * 3, margin + labelWidth + spacing + editWidth + labelWidth + editWidth, labelHeight * 5);

	ComboBox_AddString(comboBox1, "2000");
	ComboBox_AddString(comboBox1, "4000");
	ComboBox_AddString(comboBox1, "8000");
	ComboBox_AddString(comboBox1, "16000");
	ComboBox_AddString(comboBox1, "32000");
	ComboBox_AddString(comboBox1, "60000");
	ComboBox_AddString(comboBox1, "100000");
	ComboBox_AddString(comboBox2, "1");
	ComboBox_AddString(comboBox2, "10");
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

void ChangeMode(WPARAM wParam) {
	switch (LOWORD(wParam))
	{
	case ID_MODE_CLIENT:
		mode = Client;
		Update(ID_MODE_CLIENT);
		break;
	case ID_MODE_SERVER:
		udpConnection.StartServer();
		mode = Server;
		Update(ID_MODE_SERVER);
		break;
	}
}

void Update(int mode) {
	HMENU hMenu = GetMenu(hwnd);
	switch (mode) {
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
		ShowWindow(buttonSend, TRUE);
		break;
	case ID_MODE_SERVER:
		ShowWindow(labelIP, FALSE);
		ShowWindow(labelPort, FALSE);
		ShowWindow(labelPacketSize, FALSE);
		ShowWindow(labelQuantity, FALSE);
		ShowWindow(labelProtocol, FALSE);
		ShowWindow(editTextPort, FALSE);
		ShowWindow(editTextIP, FALSE);
		ShowWindow(comboBox1, FALSE);
		ShowWindow(comboBox2, FALSE);
		ShowWindow(comboBox3, FALSE);
		ShowWindow(buttonSend, FALSE);
		break;
	}
}

