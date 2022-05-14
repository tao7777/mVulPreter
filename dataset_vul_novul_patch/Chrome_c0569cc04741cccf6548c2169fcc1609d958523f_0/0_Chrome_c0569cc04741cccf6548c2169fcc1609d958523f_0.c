 TabsCustomBindings::TabsCustomBindings(ScriptContext* context)
     : ObjectBackedNativeHandler(context) {
  RouteFunction("OpenChannelToTab", "tabs",
                base::Bind(&TabsCustomBindings::OpenChannelToTab,
                           base::Unretained(this)));
 }
