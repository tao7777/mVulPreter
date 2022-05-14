RuntimeCustomBindings::RuntimeCustomBindings(ScriptContext* context)
    : ObjectBackedNativeHandler(context) {
   RouteFunction(
       "GetManifest",
       base::Bind(&RuntimeCustomBindings::GetManifest, base::Unretained(this)));
  RouteFunction("OpenChannelToExtension",
                 base::Bind(&RuntimeCustomBindings::OpenChannelToExtension,
                            base::Unretained(this)));
  RouteFunction("OpenChannelToNativeApp",
                 base::Bind(&RuntimeCustomBindings::OpenChannelToNativeApp,
                            base::Unretained(this)));
   RouteFunction("GetExtensionViews",
                base::Bind(&RuntimeCustomBindings::GetExtensionViews,
                           base::Unretained(this)));
}
