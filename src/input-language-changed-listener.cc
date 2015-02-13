#include "input-language-changed-listener.h"

InputLanguageChangedListener::InputLanguageChangedListener(void *obs, ObserverCallback cb)
{
  observer = obs;
  callback = cb;

  Win32NotificationWindow::instance()->setListener(this);
}

InputLanguageChangedListener::~InputLanguageChangedListener()
{
  Win32NotificationWindow::instance()->clearListener();
}

void InputLanguageChangedListener::Win32MessageReceived(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if (uMsg == WM_INPUTLANGCHANGE)
  {
    callback(observer);
  }
}
