 FileManagerPrivateCustomBindings::FileManagerPrivateCustomBindings(
     ScriptContext* context)
     : ObjectBackedNativeHandler(context) {
   RouteFunction(
      "GetFileSystem",
       base::Bind(&FileManagerPrivateCustomBindings::GetFileSystem,
                  base::Unretained(this)));
 }
