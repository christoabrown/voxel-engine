#pragma once
#include "stdafx.h"
#define MAX_LOADSTRING 100

struct WinInfo {
	int screenWidth;
	int screenHeight;
	HDC hdc;
	HWND hWnd;
	bool focused;
	bool resized = true;
	bool shuttingDown = false;
	WCHAR className[MAX_LOADSTRING];
	HINSTANCE hInstance;
};