#pragma once

#define LABELWIDTH 100
#define LABELHEIGHT 25
#define COMBOHEIGHT 100
#define EDITSTART 110
#define EDITWIDTH 140
#define EDITHEIGHT 30
#define SPACING 60
#define MARGIN 40

#include <Windows.h>
#include <CommCtrl.h>
#include "resource.h"

HWND CreateEditText(char *text, int x, int y, int w, int h);
HWND CreateComboBox(int x, int y, int w, int h);
HWND CreateTextBox(int x, int y, int w, int h);
HWND CreateLabel(char*, int, int, int, int);
HWND CreateButton(char *text, int x, int y, int w, int h);

extern HWND hwnd;