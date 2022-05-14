void FileBrowserHandlerCustomBindings::GetEntryURL(
     const v8::FunctionCallbackInfo<v8::Value>& args) {
  CHECK(args.Length() == 1);
  CHECK(args[0]->IsObject());
  const blink::WebURL& url =
      blink::WebDOMFileSystem::createFileSystemURL(args[0]);
  args.GetReturnValue().Set(v8_helpers::ToV8StringUnsafe(
      args.GetIsolate(), url.string().utf8().c_str()));
 }
