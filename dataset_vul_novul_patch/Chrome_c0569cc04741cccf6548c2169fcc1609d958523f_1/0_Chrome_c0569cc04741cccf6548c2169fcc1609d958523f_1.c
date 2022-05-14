 WebstoreBindings::WebstoreBindings(ScriptContext* context)
     : ObjectBackedNativeHandler(context) {
  RouteFunction("Install", "webstore",
                 base::Bind(&WebstoreBindings::Install, base::Unretained(this)));
 }
