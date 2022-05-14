v8::Handle<v8::Value> V8Proxy::throwNotEnoughArgumentsError()
v8::Handle<v8::Value> V8Proxy::throwNotEnoughArgumentsError(v8::Isolate* isolate)
 {
    return throwError(TypeError, "Not enough arguments", isolate);
 }
