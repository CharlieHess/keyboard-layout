#include "notification-window.h"

static char classname[] = "notification msg window";

static Win32NotificationWindow *gInstance = NULL;

Win32NotificationWindow *Win32NotificationWindow::instance() {
	if (NULL == gInstance)
		gInstance = new Win32NotificationWindow();
	return gInstance;
}

Win32NotificationWindow::Win32NotificationWindow() : hWindow(NULL) {
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
	  CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
	  HWND_MESSAGE, NULL, hInstance, this);
}

void Win32NotificationWindow::RunMessageLoop() {
	MSG message;
	int result;

	while ((result = GetMessage(&message, hWindow, 0, 0)) != 0) {
		if (-1 == result)
			break;

		TranslateMessage(&message);
		DispatchMessage(&message);
	}
}

void Win32NotificationWindow::SetObserver(KeyboardLayoutObserver *obs) {
	observer = obs;
}

void Win32NotificationWindow::ClearObserver() {
	observer = NULL;
}

Win32NotificationWindow::~Win32NotificationWindow() {
	if (hWindow) {
		PostMessage(hWindow, WM_CLOSE, 0, 0);
	}
}

LRESULT CALLBACK Win32NotificationWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (observer != NULL) {
		uv_async_t async = observer->getAsyncHandle();
		async.data = observer;
		uv_async_send(&async);
	}

 	return DefWindowProc(hWindow, uMsg, wParam, lParam);
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
