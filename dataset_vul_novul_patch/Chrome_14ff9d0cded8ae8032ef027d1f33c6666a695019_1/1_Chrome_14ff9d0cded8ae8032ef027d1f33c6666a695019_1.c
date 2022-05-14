FileBrowserHandlerCustomBindings::FileBrowserHandlerCustomBindings(
     ScriptContext* context)
     : ObjectBackedNativeHandler(context) {
   RouteFunction(
      "GetExternalFileEntry",
      base::Bind(&FileBrowserHandlerCustomBindings::GetExternalFileEntry,
                 base::Unretained(this)));
  RouteFunction("GetEntryURL",
                base::Bind(&FileBrowserHandlerCustomBindings::GetEntryURL,
                           base::Unretained(this)));
 }
