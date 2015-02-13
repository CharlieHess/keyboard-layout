#include "notification-window.h"

static char classname[] = "notification msg window";

static Win32NotificationWindow *gInstance = NULL;

Win32NotificationWindow *Win32NotificationWindow::instance()
{
	if (NULL == gInstance)
		gInstance = new Win32NotificationWindow();
	return gInstance;
}

Win32NotificationWindow::Win32NotificationWindow() :
	hThread(NULL),
	hWindow(NULL),
  listener(NULL)
{
	// register the window class
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = Win32NotificationWindow::WndProc;
	wcex.cbClsExtra	= 0;
	wcex.cbWndExtra	= 0;
	wcex.hInstance = GetModuleHandle(NULL);
	wcex.hIcon = 0;
	wcex.hCursor = 0;
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName = classname;
	wcex.hIconSm = 0;

	RegisterClassEx(&wcex);

	// make the window
	hWindow = CreateWindowEx(0, classname, NULL, WS_OVERLAPPEDWINDOW,
	  CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
	  HWND_MESSAGE, NULL, GetModuleHandle(NULL), this);

	if (hWindow == NULL)
		return;

	// start the thread for the window messsage loop
	DWORD threadid;

	hThread = CreateThread(NULL, 0,
    &Win32NotificationWindow::WndThread,
    this, 0, &threadid);
}

Win32NotificationWindow::~Win32NotificationWindow()
{
	if (hWindow)
	{
		PostMessage(hWindow, WM_CLOSE, 0, 0);

		if (hThread)
		{
			WaitForSingleObject(hThread, 2000);
			CloseHandle(hThread);
		}
	}
}

LRESULT CALLBACK Win32NotificationWindow::HandleMessage
(
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam
)
{
	if (listener != NULL)
	{
    listener->Win32MessageReceived(uMsg, wParam, lParam);
	}

 	return DefWindowProc(hWindow, uMsg, wParam, lParam);
}

LRESULT CALLBACK Win32NotificationWindow::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
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

DWORD WINAPI Win32NotificationWindow::WndThread(LPVOID context)
{
	MSG Msg;
	Win32NotificationWindow *that;
	int rval;

	that = (Win32NotificationWindow *) context;

	// listen to the message loop
	while ((rval = GetMessage(&Msg, that->hWindow, 0, 0)) != 0)
	{
		if (-1 == rval)
			return 0;

		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	return (DWORD) Msg.wParam;
}

void Win32NotificationWindow::setListener(Win32MessageListener *wml)
{
  listener = wml;
}

void Win32NotificationWindow::clearListener()
{
	listener = NULL;
}
