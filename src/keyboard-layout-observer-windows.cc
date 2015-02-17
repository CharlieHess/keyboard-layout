#include "keyboard-layout-observer.h"
#include "notification-window.h"

using namespace v8;

void KeyboardLayoutObserver::Init(Handle<Object> target) {
  NanScope();
  Local<FunctionTemplate> newTemplate = NanNew<FunctionTemplate>(KeyboardLayoutObserver::New);
  newTemplate->SetClassName(NanNew<String>("KeyboardLayoutObserver"));
  newTemplate->InstanceTemplate()->SetInternalFieldCount(1);
  Local<ObjectTemplate> proto = newTemplate->PrototypeTemplate();
  NODE_SET_METHOD(proto, "getCurrentKeyboardLayout", KeyboardLayoutObserver::GetCurrentKeyboardLayout);
  target->Set(NanNew<String>("KeyboardLayoutObserver"), newTemplate->GetFunction());
}

NODE_MODULE(keyboard_layout_observer, KeyboardLayoutObserver::Init)

NAN_METHOD(KeyboardLayoutObserver::New) {
  NanScope();

  Local<Function> callbackHandle = args[0].As<Function>();
  NanCallback *callback = new NanCallback(callbackHandle);

  KeyboardLayoutObserver *observer = new KeyboardLayoutObserver(callback);
  observer->Wrap(args.This());
  NanReturnUndefined();
}

uv_thread_t window_thread_id;

static void windowThread(void *arg) {
  KeyboardLayoutObserver *observer = static_cast<KeyboardLayoutObserver *>(arg);

  Win32NotificationWindow::instance()->Initialize(observer);
  Win32NotificationWindow::instance()->RunMessageLoop();
}

KeyboardLayoutObserver::KeyboardLayoutObserver(NanCallback *callback) : callback(callback) {
  uv_thread_create(&window_thread_id, windowThread, this);
}

KeyboardLayoutObserver::~KeyboardLayoutObserver() {
  Win32NotificationWindow::instance()->CleanUp();
  delete callback;
}

void KeyboardLayoutObserver::HandleKeyboardLayoutChanged() {
  callback->Call(0, NULL);
}

NAN_METHOD(KeyboardLayoutObserver::GetCurrentKeyboardLayout) {
  NanScope();

  char layoutName[KL_NAMELENGTH];
  if (::GetKeyboardLayoutName(layoutName))
    NanReturnValue(NanNew(layoutName));
  else
    NanReturnValue(NanUndefined());
}
