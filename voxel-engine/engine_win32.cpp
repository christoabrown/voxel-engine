// opengl-learning.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "engine_win32.h"
#include "renderer.h"
#include "input.h"



// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING] = L"voxel-engine";                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING] = L"VOXELENGINE";            // the main window class name
WinInfo winInfo;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
					 _In_opt_ HINSTANCE hPrevInstance,
					 _In_ LPWSTR    lpCmdLine,
					 _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	MSG msg;
	winInfo.shuttingDown = false;

	std::thread renderThread(setupGL, &winInfo);
	// Main message loop:
	while (!winInfo.shuttingDown)
	{
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (shouldUpdateVisibility()) 
			updateVisibility();
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	if(renderThread.joinable())
		renderThread.join();
	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style          = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcex.lpfnWndProc    = WndProc;
	wcex.cbClsExtra     = 0;
	wcex.cbWndExtra     = 0;
	wcex.hInstance      = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = NULL;
	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName  = szWindowClass;
	wcex.hIconSm = NULL;

	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	  CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
	  return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   winInfo.hWnd = hWnd;
   winInfo.hdc = GetDC(hWnd);
   memcpy(winInfo.className, szWindowClass, MAX_LOADSTRING);
   winInfo.hInstance = hInstance;
   //winInfo.className = szWindowClass;

   RAWINPUTDEVICE rid[2];
   rid[0].usUsagePage = 0x01;
   rid[0].usUsage = 0x02;
   rid[0].dwFlags = 0; // HID Mouse
   rid[0].hwndTarget = hWnd;

   rid[1].usUsagePage = 0x01;
   rid[1].usUsage = 0x06;
   rid[1].dwFlags = 0; // HID Keyboard
   rid[1].hwndTarget = hWnd;

   if (RegisterRawInputDevices(rid, 2, sizeof(rid[0])) == FALSE)
   {
	   //handle register error with GetLastError
   }

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
		{
			int wmId = LOWORD(wParam);
			// Parse the menu selections:
			switch (wmId)
			{
			//case IDM_ABOUT:
			//	DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			//	break;
			//case IDM_EXIT:
			//	DestroyWindow(hWnd);
			//	break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
		break;
	case WM_SIZE:
		{
			if (LOWORD(lParam) != 0 && HIWORD(lParam) != 0)
			{
				winInfo.screenWidth = LOWORD(lParam);
				winInfo.screenHeight = HIWORD(lParam);
			}
			winInfo.resized = true;
		}
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_INPUT:
		{
			UINT bufferSize;
			
			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &bufferSize, sizeof(RAWINPUTHEADER));
			LPBYTE lpb = new BYTE[bufferSize];
			if (lpb == NULL)
			{
				return 0;
			}
			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, (LPVOID)lpb, &bufferSize, sizeof(RAWINPUTHEADER));
			RAWINPUT* rawInput = (RAWINPUT*)lpb;

			NewInput(rawInput);
			delete[] lpb;
		}
		break;
	case WM_SETFOCUS:
		winInfo.focused = true;
		break;
	case WM_KILLFOCUS:
		winInfo.focused = false;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		if(hWnd == winInfo.hWnd)
			winInfo.shuttingDown = true;
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
