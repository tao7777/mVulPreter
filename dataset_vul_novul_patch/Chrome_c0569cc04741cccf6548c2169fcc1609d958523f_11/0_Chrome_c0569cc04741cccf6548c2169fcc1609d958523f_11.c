 TestNativeHandler::TestNativeHandler(ScriptContext* context)
     : ObjectBackedNativeHandler(context) {
   RouteFunction(
      "GetWakeEventPage", "test",
       base::Bind(&TestNativeHandler::GetWakeEventPage, base::Unretained(this)));
 }
