 WebstoreBindings::WebstoreBindings(ScriptContext* context)
     : ObjectBackedNativeHandler(context) {
  RouteFunction("Install",
                 base::Bind(&WebstoreBindings::Install, base::Unretained(this)));
 }
