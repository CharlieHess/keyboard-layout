#include "notification-window.h"

#include <iostream>
using namespace std;

static char classname[] = "notification msg window";

static Win32NotificationWindow *gInstance = NULL;

Win32NotificationWindow *Win32NotificationWindow::instance() {
	if (NULL == gInstance)
		gInstance = new Win32NotificationWindow();
	return gInstance;
}

Win32NotificationWindow::Win32NotificationWindow() : hWindow(NULL), callback(NULL) {
	HWND hWndParent = GetActiveWindow();
	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hWndParent, GWL_HINSTANCE);

	// Register the window class
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = Win32NotificationWindow::WndProc;
	wcex.cbClsExtra	= 0;
	wcex.cbWndExtra	= 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = 0;
	wcex.hCursor = 0;
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName = classname;
	wcex.hIconSm = 0;

	RegisterClassEx(&wcex);

	// Make the window
	hWindow = CreateWindowEx(0, classname, NULL, WS_OVERLAPPEDWINDOW,
	  CW_USEDEFAULT, CW_USEDEFAULT, 200, 200,
	  NULL, NULL, hInstance, this);

	ShowWindow(hWindow, SW_SHOW);
}

void Win32NotificationWindow::RunMessageLoop() {
	ShowWindow(hWindow, SW_SHOW);

	MSG message;
	int result;

	while ((result = GetMessage(&message, hWindow, 0, 0)) != 0) {
		if (-1 == result)
			break;

		TranslateMessage(&message);
		DispatchMessage(&message);
	}
}

void Win32NotificationWindow::Initialize(WmCallback cb) {
	callback = cb;
}

void Win32NotificationWindow::CleanUp() {
	callback = NULL;
}

LRESULT CALLBACK Win32NotificationWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (callback != NULL) {
		callback(uMsg, wParam, lParam);
	}

 	return DefWindowProc(hWindow, uMsg, wParam, lParam);
}

Win32NotificationWindow::~Win32NotificationWindow() {
	if (hWindow) {
		PostMessage(hWindow, WM_CLOSE, 0, 0);
	}
}

LRESULT CALLBACK Win32NotificationWindow::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	Win32NotificationWindow *self;
	if (uMsg == WM_NCCREATE) {
		LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
		self = reinterpret_cast<Win32NotificationWindow *>(lpcs->lpCreateParams);
		self->hWindow = hwnd;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(self));
	} else {
		self = reinterpret_cast<Win32NotificationWindow *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}
	if (self) {
		return self->HandleMessage(uMsg, wParam, lParam);
	} else {
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}