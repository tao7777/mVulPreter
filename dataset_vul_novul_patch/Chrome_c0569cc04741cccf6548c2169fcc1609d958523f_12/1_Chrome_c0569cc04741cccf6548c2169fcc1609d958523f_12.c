V8ContextNativeHandler::V8ContextNativeHandler(ScriptContext* context)
    : ObjectBackedNativeHandler(context), context_(context) {
  RouteFunction("GetAvailability",
                base::Bind(&V8ContextNativeHandler::GetAvailability,
                           base::Unretained(this)));
   RouteFunction("GetModuleSystem",
                 base::Bind(&V8ContextNativeHandler::GetModuleSystem,
                            base::Unretained(this)));
  RouteFunction(
      "RunWithNativesEnabled",
      base::Bind(&V8ContextNativeHandler::RunWithNativesEnabled,
                 base::Unretained(this)));
 }
