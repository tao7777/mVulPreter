FileBrowserHandlerCustomBindings::FileBrowserHandlerCustomBindings(
     ScriptContext* context)
     : ObjectBackedNativeHandler(context) {
   RouteFunction(
      "GetExternalFileEntry", "fileBrowserHandler",
      base::Bind(
          &FileBrowserHandlerCustomBindings::GetExternalFileEntryCallback,
          base::Unretained(this)));
 }
