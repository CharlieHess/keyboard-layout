#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class Win32MessageListener
{
protected:
	virtual void Win32MessageReceived(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

	friend class Win32NotificationWindow;
};

class Win32NotificationWindow
{
public:
	static Win32NotificationWindow *instance();
	virtual ~Win32NotificationWindow();

	HWND handle()
	{
		return hWindow;
	}

	void setListener(Win32MessageListener *wml);
	void clearListener();

protected:
	Win32NotificationWindow();
	LRESULT CALLBACK HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	static DWORD WINAPI WndThread(LPVOID context);
	static LRESULT CALLBACK WndProc(HWND hwnd,
  	UINT uMsg,
  	WPARAM wParam,
  	LPARAM lParam);

	HWND hWindow;
	HANDLE hThread;
	Win32MessageListener *listener;
};
