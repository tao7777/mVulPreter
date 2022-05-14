v8::Handle<v8::Value> V8WebSocket::constructorCallback(const v8::Arguments& args)
{
    INC_STATS("DOM.WebSocket.Constructor");

    if (!args.IsConstructCall())
        return V8Proxy::throwTypeError("DOM object constructor cannot be called as a function.");

    if (ConstructorMode::current() == ConstructorMode::WrapExistingObject)
         return args.Holder();
 
     if (args.Length() == 0)
        return V8Proxy::throwNotEnoughArgumentsError();
 
     v8::TryCatch tryCatch;
     v8::Handle<v8::String> urlstring = args[0]->ToString();
    if (tryCatch.HasCaught())
        return throwError(tryCatch.Exception(), args.GetIsolate());
    if (urlstring.IsEmpty())
        return V8Proxy::throwError(V8Proxy::SyntaxError, "Empty URL", args.GetIsolate());

    ScriptExecutionContext* context = getScriptExecutionContext();
    if (!context)
        return V8Proxy::throwError(V8Proxy::ReferenceError, "WebSocket constructor's associated frame is not available", args.GetIsolate());

    const KURL& url = context->completeURL(toWebCoreString(urlstring));

    RefPtr<WebSocket> webSocket = WebSocket::create(context);
    ExceptionCode ec = 0;

    if (args.Length() < 2)
        webSocket->connect(url, ec);
    else {
        v8::Local<v8::Value> protocolsValue = args[1];
        if (protocolsValue->IsArray()) {
            Vector<String> protocols;
            v8::Local<v8::Array> protocolsArray = v8::Local<v8::Array>::Cast(protocolsValue);
            for (uint32_t i = 0; i < protocolsArray->Length(); ++i) {
                v8::TryCatch tryCatchProtocol;
                v8::Handle<v8::String> protocol = protocolsArray->Get(v8::Int32::New(i))->ToString();
                if (tryCatchProtocol.HasCaught())
                    return throwError(tryCatchProtocol.Exception(), args.GetIsolate());
                protocols.append(toWebCoreString(protocol));
            }
            webSocket->connect(url, protocols, ec);
        } else {
            v8::TryCatch tryCatchProtocol;
            v8::Handle<v8::String> protocol = protocolsValue->ToString();
            if (tryCatchProtocol.HasCaught())
                return throwError(tryCatchProtocol.Exception(), args.GetIsolate());
            webSocket->connect(url, toWebCoreString(protocol), ec);
        }
    }
    if (ec)
        return throwError(ec, args.GetIsolate());

    V8DOMWrapper::setDOMWrapper(args.Holder(), &info, webSocket.get());
    V8DOMWrapper::setJSWrapperForActiveDOMObject(webSocket.release(), v8::Persistent<v8::Object>::New(args.Holder()));
    return args.Holder();
}
