/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: UIWrappers.cpp
--
-- PROGRAM: ProtocolAnalyzer
--
-- FUNCTIONS:
-- HWND CreateLabel(char *text, int x, int y, int width, int height)
-- HWND CreateEditText(char *text, int x, int y, int width, int height)
-- HWND CreateButton(char *text, int x, int y, int width, int height)
-- HWND CreateTextBox(int x, int y, int width, int height)
-- HWND CreateComboBox(int x, int y, int width, int height)
--
-- DATE: February 10th, 2016
--
-- REVISIONS: February 10th, 2016: Created Wrapper functions
--
-- DESIGNER: Carson Roscoe
--
-- PROGRAMMER: Carson Roscoe
--
-- NOTES:
-- Win32 UI wrapper functions to make creating the GUI easier
----------------------------------------------------------------------------------------------------------------------*/
#include "UIWrappers.h"

//Wrapper define to add a stirng to a combo box easier
#define ComboBox_AddStr(cmb,message) SendMessage((HWND)(cmb), CB_ADDSTRING, 0, (LPARAM)(message));

//Creates a label with a given string at postion x/y on the screen with the given width and height
HWND CreateLabel(char *text, int x, int y, int width, int height) {
	return CreateWindow("static", text, WS_CHILD | WS_VISIBLE, x, y, width, height,
		hwnd, NULL, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
}

//Creates a editible text area with a given string at postion x/y on the screen with the given width and height
HWND CreateEditText(char *text, int x, int y, int width, int height) {
	return CreateWindow("EDIT", text, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT, x, y, width, height, hwnd, NULL,
		(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
}

//Creates a button with a given string at postion x/y on the screen with the given width and height
HWND CreateButton(char *text, int x, int y, int width, int height) {
	return CreateWindow("button", text, WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, x, y, width, height,
		hwnd, (HMENU)IDM_SEND, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
}

//Creates a large editble text area with a given string at postion x/y on the screen with the given width and height
HWND CreateTextBox(int x, int y, int width, int height) {
	return CreateWindowEx(0, "EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT |
		ES_MULTILINE | ES_AUTOVSCROLL, x, y, width, height, hwnd, (HMENU)IDM_RES,
		(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
}

//Creates a combo box with a given string at postion x/y on the screen with the given width and height
HWND CreateComboBox(int x, int y, int width, int height) {
	return CreateWindow(WC_COMBOBOX, NULL, CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED 
		| WS_VISIBLE, x, y, width, height, hwnd, NULL, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
}
