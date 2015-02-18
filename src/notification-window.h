#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "keyboard-layout-observer.h"

typedef void (*WmCallback) (UINT msg, WPARAM wParam, LPARAM lParam);

class Win32NotificationWindow {
public:
	static Win32NotificationWindow *instance();
	virtual ~Win32NotificationWindow();

	HWND handle() {
		return hWindow;
	}

	void Initialize(WmCallback cb);
	void RunMessageLoop();
	void CleanUp();

protected:
	Win32NotificationWindow();
	LRESULT CALLBACK HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	static LRESULT CALLBACK WndProc(HWND hwnd,
  	UINT uMsg,
  	WPARAM wParam,
  	LPARAM lParam);

	HWND hWindow;
	WmCallback callback;
};
