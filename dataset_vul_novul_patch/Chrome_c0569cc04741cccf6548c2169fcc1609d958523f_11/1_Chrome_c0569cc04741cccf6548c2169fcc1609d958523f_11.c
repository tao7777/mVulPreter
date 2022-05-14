 TestNativeHandler::TestNativeHandler(ScriptContext* context)
     : ObjectBackedNativeHandler(context) {
   RouteFunction(
      "GetWakeEventPage",
       base::Bind(&TestNativeHandler::GetWakeEventPage, base::Unretained(this)));
 }
