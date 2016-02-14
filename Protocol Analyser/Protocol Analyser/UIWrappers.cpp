#include "UIWrappers.h"

#define ComboBox_AddStr(cmb,message) SendMessage((HWND)(cmb), CB_ADDSTRING, 0, (LPARAM)(message));

HWND CreateLabel(char *text, int x, int y, int w, int h) {
	return CreateWindow("static", text, WS_CHILD | WS_VISIBLE | WS_TABSTOP, x, y, w, h,
		hwnd, NULL, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
}

HWND CreateEditText(char *text, int x, int y, int w, int h) {
	return CreateWindow("EDIT", text, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT |
		ES_AUTOHSCROLL | ES_WANTRETURN, x, y, w, h, hwnd, NULL,
		(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
}

HWND CreateButton(char *text, int x, int y, int w, int h) {
	return CreateWindow("button", text, WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, x, y, w, h,
		hwnd, (HMENU)IDM_SEND, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
}

HWND CreateTextBox(int x, int y, int w, int h) {
	return CreateWindowEx(0, "EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT |
		ES_MULTILINE | ES_AUTOVSCROLL, x, y, w, h, hwnd, (HMENU)IDM_RES,
		(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
}

HWND CreateComboBox(int x, int y, int w, int h) {
	return CreateWindow(WC_COMBOBOX, NULL, CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED 
		| WS_VISIBLE, x, y, w, h, hwnd, NULL, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
}
