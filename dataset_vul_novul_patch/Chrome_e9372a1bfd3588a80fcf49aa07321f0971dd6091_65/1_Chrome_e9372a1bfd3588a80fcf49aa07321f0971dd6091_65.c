v8::Handle<v8::Value> V8WebKitMutationObserver::constructorCallback(const v8::Arguments& args)
{
    INC_STATS("DOM.WebKitMutationObserver.Constructor");

    if (!args.IsConstructCall())
        return V8Proxy::throwTypeError("DOM object constructor cannot be called as a function.");

    if (ConstructorMode::current() == ConstructorMode::WrapExistingObject)
         return args.Holder();
 
     if (args.Length() < 1)
        return V8Proxy::throwNotEnoughArgumentsError();
 
     v8::Local<v8::Value> arg = args[0];
     if (!arg->IsObject())
        return throwError(TYPE_MISMATCH_ERR, args.GetIsolate());

    ScriptExecutionContext* context = getScriptExecutionContext();
    if (!context)
        return V8Proxy::throwError(V8Proxy::ReferenceError, "WebKitMutationObserver constructor's associated frame unavailable", args.GetIsolate());

    RefPtr<MutationCallback> callback = V8MutationCallback::create(arg, context);
    RefPtr<WebKitMutationObserver> observer = WebKitMutationObserver::create(callback.release());

    V8DOMWrapper::setDOMWrapper(args.Holder(), &info, observer.get());
    V8DOMWrapper::setJSWrapperForDOMObject(observer.release(), v8::Persistent<v8::Object>::New(args.Holder()));
    return args.Holder();
}
