 FileManagerPrivateCustomBindings::FileManagerPrivateCustomBindings(
     ScriptContext* context)
     : ObjectBackedNativeHandler(context) {
  RouteFunction("GetFileSystem", "fileManagerPrivate",
                base::Bind(&FileManagerPrivateCustomBindings::GetFileSystem,
                           base::Unretained(this)));
   RouteFunction(
      "GetExternalFileEntry", "fileManagerPrivate",
      base::Bind(&FileManagerPrivateCustomBindings::GetExternalFileEntry,
                 base::Unretained(this)));
  RouteFunction("GetEntryURL", "fileManagerPrivate",
                base::Bind(&FileManagerPrivateCustomBindings::GetEntryURL,
                           base::Unretained(this)));
 }
