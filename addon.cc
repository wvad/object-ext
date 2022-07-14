#include <node.h>

using namespace v8;

typedef const FunctionCallbackInfo<Value> &FunctionArgumnents;

inline Local<String> ToV8String(Isolate *isolate, const char *str) {
  return String::NewFromUtf8(isolate, str, NewStringType::kNormal).ToLocalChecked();
}

inline void SetObjectProperty(Local<Object> obj, const char *name, Local<Value> value) {
  Isolate *isolate = obj->GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  obj->Set(context, ToV8String(isolate, name), value).Check();
}

inline void SetObjectProperty(Local<Object> obj, uint32_t index, Local<Value> value) {
  obj->Set(obj->GetIsolate()->GetCurrentContext(), index, value).Check();
}

void GetInfo(FunctionArgumnents args) {
  if (!args[0]->IsObject()) return;
  Local<Object> obj = args[0].As<Object>();
  Isolate *isolate = obj->GetIsolate();
  Local<Object> info = Object::New(isolate);

  bool isKeyValue;
  MaybeLocal<Array> maybeEntries = obj->PreviewEntries(&isKeyValue);
  if (maybeEntries.IsEmpty()) {
    SetObjectProperty(info, "entries", Undefined(isolate));
  } else {
    Local<Context> context = isolate->GetCurrentContext();
    if (isKeyValue) {
      Local<Array> checked = maybeEntries.ToLocalChecked();
      uint32_t length = checked->Length() / 2;
      Local<Array> entries = Array::New(isolate, length);
      SetObjectProperty(entries, "isKeyValue", Boolean::New(isolate, true));
      for (uint32_t i = 0; i < length; i++) {
        Local<Value> key = checked->Get(context, i * 2).ToLocalChecked();
        Local<Value> value = checked->Get(context, i * 2 + 1).ToLocalChecked();
        Local<Object> entry = Array::New(isolate, 2);
        SetObjectProperty(entry, (uint32_t)0, key);
        SetObjectProperty(entry, (uint32_t)1, value);
        SetObjectProperty(entries, i, entry);
      }
      SetObjectProperty(info, "entries", entries);
    } else {
      Local<Array> entries = maybeEntries.ToLocalChecked();
      SetObjectProperty(entries, "isKeyValue", Boolean::New(isolate, false));
      SetObjectProperty(info, "entries", maybeEntries.ToLocalChecked());
    }
  }

  int internalFieldCount = obj->InternalFieldCount();
  Local<Array> internalFields = Array::New(isolate, internalFieldCount);
  for (int i = 0; i < internalFieldCount; i++) SetObjectProperty(internalFields, i, obj->GetInternalField(i));
  SetObjectProperty(info, "internalFields", internalFields);

  Local<String> className = obj->GetConstructorName();
  if (!className.IsEmpty()) SetObjectProperty(info, "className", className);

  int id = obj->GetIdentityHash();
  SetObjectProperty(info, "id", Number::New(isolate, id));

  if (args[0]->IsExternal()) {
    void *address = args[0].As<External>()->Value();
    std::string addressStr = std::to_string(reinterpret_cast<uintptr_t>(address));
    SetObjectProperty(info, "external", ToV8String(isolate, addressStr.data()));
  } else {
    SetObjectProperty(info, "external", Undefined(isolate));
  }

  bool isCallable = obj->IsCallable();
  SetObjectProperty(info, "isCallable", Boolean::New(isolate, isCallable));

  bool isConstructor = obj->IsConstructor();
  SetObjectProperty(info, "isConstructor", Boolean::New(isolate, isConstructor));

  bool isApiWrapper = obj->IsApiWrapper();
  SetObjectProperty(info, "isApiWrapper", Boolean::New(isolate, isApiWrapper));

  args.GetReturnValue().Set(info);
}

void LazyDataPropertyCallback(Local<Name>, const PropertyCallbackInfo<Value> &info) {
  Isolate *isolate = info.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  Local<Function> callback = info.Data().As<Function>();
  Local<Value> argv[] = { Undefined(isolate) };
  info.GetReturnValue().Set(callback->Call(context, Undefined(isolate), 0, argv).ToLocalChecked());
}

void SetLazyAccessor(FunctionArgumnents args) {
  if (!args[0]->IsObject() || !args[1]->IsName() || !args[2]->IsFunction()) return;
  Local<Context> context = args.GetIsolate()->GetCurrentContext();
  Local<Object> obj = args[0].As<Object>();
  Local<Name> name = args[1].As<Name>();
  Local<Function> callback = args[2].As<Function>();
  obj->SetLazyDataProperty(context, name, LazyDataPropertyCallback, callback);
}

void NativeDataPropertyGetter(Local<Name>, const PropertyCallbackInfo<Value> &info) {
  Isolate *isolate = info.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  Local<Function> getter = info.Data().As<Object>()->Get(context, 0).ToLocalChecked().As<Function>();
  Local<Value> argv[] = { Undefined(isolate) };
  info.GetReturnValue().Set(getter->Call(context, info.This(), 0, argv).ToLocalChecked());
}

void NativeDataPropertySetter(Local<Name>, Local<Value> value, const PropertyCallbackInfo<void> &info) {
  Isolate *isolate = info.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  Local<Function> setter = info.Data().As<Object>()->Get(context, 1).ToLocalChecked().As<Function>();
  Local<Value> argv[] = { value };
  setter->Call(context, info.This(), 1, argv);
}

void SetAccessor(FunctionArgumnents args) {
  if (!args[0]->IsObject() || !args[1]->IsName() || !args[2]->IsFunction() || !args[3]->IsFunction()) return;
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  Local<Object> obj = args[0].As<Object>();
  Local<Name> name = args[1].As<Name>();
  Local<Function> getter = args[2].As<Function>();
  Local<Function> setter = args[3].As<Function>();
  Local<Object> callbacks = Object::New(isolate);
  callbacks->Set(context, 0, getter);
  callbacks->Set(context, 1, setter);
  obj->SetNativeDataProperty(context, name, NativeDataPropertyGetter, NativeDataPropertySetter, callbacks);
}

void HasRealProperty(FunctionArgumnents args) {
  if (!args[0]->IsObject()) return;
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  Local<Object> obj = args[0].As<Object>();
  auto maybeIndex = args[1]->ToUint32(context);
  if (!maybeIndex.IsEmpty()) {
    uint32_t index = maybeIndex.ToLocalChecked()->Value();
    auto result = obj->HasRealIndexedProperty(context, index);
    if (result.IsJust() ? result.ToChecked() : false) {
      args.GetReturnValue().Set(ToV8String(isolate, "indexed"));
      return;
    }
  }
  Local<Value> name = args[1];
  if (!name->IsSymbol()) {
    auto maybeName = name->ToString(context);
    if (maybeName.IsEmpty()) return;
    name = maybeName.ToLocalChecked();
  }
  auto result = obj->HasRealNamedProperty(context, name.As<Name>());
  if (result.IsJust() ? result.ToChecked() : false) {
    args.GetReturnValue().Set(ToV8String(isolate, "named"));
    return;
  }
  result = obj->HasRealNamedCallbackProperty(context, name.As<Name>());
  if (result.IsJust() ? result.ToChecked() : false) {
    args.GetReturnValue().Set(ToV8String(isolate, "namedCallback"));
    return;
  }
  args.GetReturnValue().Set(false);
}

void structuredClone(FunctionArgumnents args) {
  if (!args[0]->IsObject()) return;
  Isolate *isolate = args.GetIsolate();
#if 15 < NODE_MAJOR_VERSION
  MaybeLocal<Context> maybeContext = args[0].As<Object>()->GetCreationContext();
  if (maybeContext.IsEmpty()) return;
  Local<Context> context = maybeContext.ToLocalChecked();
#else
  Local<Context> context = args[0].As<Object>()->CreationContext();
  if (context.IsEmpty()) return;
#endif
  ValueSerializer serializer(isolate);
  Maybe<bool> succeeded = serializer.WriteValue(context, args[0]);
  auto data = serializer.Release();
  if (succeeded.IsNothing() || !succeeded.ToChecked()) return;  
  ValueDeserializer deserializer(isolate, data.first, data.second);
  MaybeLocal<Value> maybeResult = deserializer.ReadValue(context);
  if (maybeResult.IsEmpty()) return;
  args.GetReturnValue().Set(maybeResult.ToLocalChecked());
}

Local<Function> toJSFunction(Isolate *isolate, FunctionCallback func, const char *funcName, int paramsLength) {
  Local<Context> context = isolate->GetCurrentContext();
  Local<Function> function = FunctionTemplate::New(
    isolate,
    func,
    Local<Value>(),
    Local<Signature>(),
    paramsLength,
    ConstructorBehavior::kThrow
  )
  ->GetFunction(context)
  .ToLocalChecked();
  function->SetName(ToV8String(isolate, funcName));
  return function;
}

void addMethodToObject(Local<Object> object, const char *funcName,  FunctionCallback function, int paramsLength) {
  Isolate *isolate = object->GetIsolate();
  Local<Function> func = toJSFunction(isolate, function, funcName, paramsLength);
  SetObjectProperty(object, funcName, func);
}

void init(Local<Object> exports) {
  addMethodToObject(exports, "getInfo", GetInfo, 1);
  addMethodToObject(exports, "setLazyAccessor", SetLazyAccessor, 3);
  addMethodToObject(exports, "setAccessor", SetAccessor, 4);
  addMethodToObject(exports, "hasRealProperty", HasRealProperty, 2);
  addMethodToObject(exports, "sameContextStructuredClone", structuredClone, 1);
}

NODE_MODULE(addon, init)
