#include "stdafx.h"
#include "input.h"
std::array<bool, 1024> keyboard;
const GLfloat sensitivity = 0.10f;
GLfloat mouseXOffset, mouseYOffset;
MouseInput mouse = {};

void setVisibility(bool visible)
{
	mouse.visible = visible;
	mouse.updateVisibility = true;
}

void updateVisibility()
{
	if(mouse.visible)
		while (ShowCursor(TRUE)<0);
	else
		while (ShowCursor(FALSE) >= 0);
	mouse.updateVisibility = false;
}

void HandleInput(WinInfo *winInfo)
{
	//KEYBOARD
	if (keyboard['W'])
		TranslateCamera(FORWARD);
	if (keyboard['S'])
		TranslateCamera(BACKWARD);
	if (keyboard['A'])
		TranslateCamera(LEFT);
	if (keyboard['D'])
		TranslateCamera(RIGHT);
	if (keyboard[VK_SPACE])
		TranslateCamera(UP);
	if (keyboard[VK_SHIFT])
		TranslateCamera(DOWN);
	if (keyboard[VK_ESCAPE])
		setVisibility(true);

	//MOUSE
	if (!mouse.visible && (mouseXOffset != 0.0f || mouseYOffset != 0.0f))
	{
		RotateCamera(mouseXOffset, mouseYOffset);
		mouseXOffset = mouseYOffset = 0.0f;

		RECT rect;
		GetWindowRect(winInfo->hWnd, &rect);
		SetCursorPos(rect.left + winInfo->screenWidth / 2, rect.top + winInfo->screenHeight / 2);
	}
	if (mouse.left && winInfo->focused)
	{
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(winInfo->hWnd, &pt);

		if (pt.y >= 0 && pt.y <= winInfo->screenHeight && pt.x >= 0 && pt.x <= winInfo->screenWidth)
			setVisibility(false);
	}
}

void NewInput(RAWINPUT* input)
{
	if (input->header.dwType == RIM_TYPEKEYBOARD)
	{
		UINT msg = input->data.keyboard.Message;
		USHORT key = input->data.keyboard.VKey;

		if (msg == WM_KEYDOWN)
		{
			keyboard[key] = true;
		}
		else if (msg == WM_KEYUP)
		{
			keyboard[key] = false;
		}
	}
	else if (input->header.dwType == RIM_TYPEMOUSE)
	{
		mouseXOffset += input->data.mouse.lLastX * sensitivity;
		mouseYOffset += input->data.mouse.lLastY * sensitivity * -1.0f;

		USHORT button = input->data.mouse.usButtonFlags;
		mouse.left = (button == RI_MOUSE_LEFT_BUTTON_DOWN) ? true : (button == RI_MOUSE_LEFT_BUTTON_UP) ? false : mouse.left;
		mouse.right = (button == RI_MOUSE_RIGHT_BUTTON_DOWN) ? true : (button == RI_MOUSE_RIGHT_BUTTON_UP) ? false : mouse.right;
		mouse.middle = (button == RI_MOUSE_MIDDLE_BUTTON_DOWN) ? true : (button == RI_MOUSE_MIDDLE_BUTTON_UP) ? false : mouse.middle;
		mouse.button4 = (button == RI_MOUSE_BUTTON_4_DOWN) ? true : (button == RI_MOUSE_BUTTON_4_UP) ? false : mouse.button4;
		mouse.button5 = (button == RI_MOUSE_BUTTON_5_DOWN) ? true : (button == RI_MOUSE_BUTTON_5_UP) ? false : mouse.button5;
		if (button == RI_MOUSE_WHEEL)
			mouse.wheelDelta = input->data.mouse.usButtonData;
	}
}

bool shouldUpdateVisibility()
{
	return(mouse.updateVisibility);
}

bool getVisiblity()
{
	return(mouse.visible);
}