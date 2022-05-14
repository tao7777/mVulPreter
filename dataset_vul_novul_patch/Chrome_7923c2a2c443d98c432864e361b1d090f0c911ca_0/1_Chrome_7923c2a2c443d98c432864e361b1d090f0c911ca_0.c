V8ContextNativeHandler::V8ContextNativeHandler(ScriptContext* context,
                                               Dispatcher* dispatcher)
    : ObjectBackedNativeHandler(context),
      context_(context),
      dispatcher_(dispatcher) {
  RouteFunction("GetAvailability",
                base::Bind(&V8ContextNativeHandler::GetAvailability,
                           base::Unretained(this)));
  RouteFunction("GetModuleSystem",
                 base::Bind(&V8ContextNativeHandler::GetModuleSystem,
                            base::Unretained(this)));
   RouteFunction(
      "RunWithNativesEnabledModuleSystem",
      base::Bind(&V8ContextNativeHandler::RunWithNativesEnabledModuleSystem,
                  base::Unretained(this)));
 }
