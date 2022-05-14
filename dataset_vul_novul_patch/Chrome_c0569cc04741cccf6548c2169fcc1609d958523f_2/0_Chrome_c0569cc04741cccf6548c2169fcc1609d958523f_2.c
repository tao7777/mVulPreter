ApiDefinitionsNatives::ApiDefinitionsNatives(Dispatcher* dispatcher,
                                              ScriptContext* context)
     : ObjectBackedNativeHandler(context), dispatcher_(dispatcher) {
   RouteFunction(
      "GetExtensionAPIDefinitionsForTest", "test",
       base::Bind(&ApiDefinitionsNatives::GetExtensionAPIDefinitionsForTest,
                  base::Unretained(this)));
 }
