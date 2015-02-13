#pragma once

#include <windows.h>
#include "notification-window.h"

typedef void (*ObserverCallback) (void *observer);

class InputLanguageChangedListener : public Win32MessageListener
{
public:
  InputLanguageChangedListener(void *observer, ObserverCallback cb);
  ~InputLanguageChangedListener();

protected:
  void *observer;
  ObserverCallback callback;

  void Win32MessageReceived(UINT uMsg, WPARAM wParam, LPARAM lParam);
};
