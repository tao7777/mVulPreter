v8::Handle<v8::Value> V8Proxy::throwNotEnoughArgumentsError()
 {
    return throwError(TypeError, "Not enough arguments");
 }
