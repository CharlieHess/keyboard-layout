#include "keyboard-layout-observer.h"
#include "input-language-changed-listener.h"

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

uv_loop_t *loop = uv_default_loop();
uv_async_t async;

InputLanguageChangedListener *listener;

static void asyncSendHandler(uv_async_t *handle) {
  (static_cast<KeyboardLayoutObserver *>(handle->data))->HandleKeyboardLayoutChanged();
}

static void onInputLanguageChanged(void *observer) {
  async.data = observer;
  uv_async_send(&async);
}

KeyboardLayoutObserver::KeyboardLayoutObserver(NanCallback *callback) : callback(callback) {
  uv_async_init(loop, &async, (uv_async_cb) asyncSendHandler);

  listener = new InputLanguageChangedListener(this, &onInputLanguageChanged);
}

KeyboardLayoutObserver::~KeyboardLayoutObserver() {
  delete callback;
  delete listener;
};

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
