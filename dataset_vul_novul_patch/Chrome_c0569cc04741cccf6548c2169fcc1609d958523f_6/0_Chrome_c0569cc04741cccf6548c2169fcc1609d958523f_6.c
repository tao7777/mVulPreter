LazyBackgroundPageNativeHandler::LazyBackgroundPageNativeHandler(
     ScriptContext* context)
     : ObjectBackedNativeHandler(context) {
   RouteFunction(
      "IncrementKeepaliveCount", "tts",
       base::Bind(&LazyBackgroundPageNativeHandler::IncrementKeepaliveCount,
                  base::Unretained(this)));
   RouteFunction(
      "DecrementKeepaliveCount", "tts",
       base::Bind(&LazyBackgroundPageNativeHandler::DecrementKeepaliveCount,
                  base::Unretained(this)));
 }
