#pragma once
#include "stdafx.h"
#include "camera.h"
#include "engine_win32.h"

struct MouseInput {
	bool left = false;
	bool right = false;
	bool middle = false;
	bool button4 = false;
	bool button5 = false;
	USHORT wheelDelta = 0;
	bool visible = true;
	bool updateVisibility = false;
};

void HandleInput(WinInfo *winInfo);
void NewInput(RAWINPUT* input);
bool getVisiblity();
bool shouldUpdateVisibility();
void updateVisibility();
