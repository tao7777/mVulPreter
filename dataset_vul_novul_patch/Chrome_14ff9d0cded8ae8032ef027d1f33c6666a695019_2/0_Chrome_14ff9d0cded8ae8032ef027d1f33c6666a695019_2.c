void FileBrowserHandlerCustomBindings::GetEntryURL(
void FileBrowserHandlerCustomBindings::GetExternalFileEntryCallback(
     const v8::FunctionCallbackInfo<v8::Value>& args) {
  GetExternalFileEntry(args, context());
 }
