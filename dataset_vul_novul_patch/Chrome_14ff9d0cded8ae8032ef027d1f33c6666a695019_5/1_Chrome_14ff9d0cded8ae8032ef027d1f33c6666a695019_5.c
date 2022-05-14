MediaGalleriesCustomBindings::MediaGalleriesCustomBindings(
     ScriptContext* context)
     : ObjectBackedNativeHandler(context) {
   RouteFunction(
      "GetMediaFileSystemObject",
       base::Bind(&MediaGalleriesCustomBindings::GetMediaFileSystemObject,
                  base::Unretained(this)));
 }
