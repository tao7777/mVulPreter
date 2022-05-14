LazyBackgroundPageNativeHandler::LazyBackgroundPageNativeHandler(
     ScriptContext* context)
     : ObjectBackedNativeHandler(context) {
   RouteFunction(
      "IncrementKeepaliveCount",
       base::Bind(&LazyBackgroundPageNativeHandler::IncrementKeepaliveCount,
                  base::Unretained(this)));
   RouteFunction(
      "DecrementKeepaliveCount",
       base::Bind(&LazyBackgroundPageNativeHandler::DecrementKeepaliveCount,
                  base::Unretained(this)));
 }
