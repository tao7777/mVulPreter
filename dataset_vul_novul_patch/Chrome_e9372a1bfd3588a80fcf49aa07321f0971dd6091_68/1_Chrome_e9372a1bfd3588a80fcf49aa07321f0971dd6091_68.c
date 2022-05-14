v8::Handle<v8::Value> V8WebSocket::sendCallback(const v8::Arguments& args)
{
     INC_STATS("DOM.WebSocket.send()");
 
     if (!args.Length())
        return V8Proxy::throwNotEnoughArgumentsError();
 
     WebSocket* webSocket = V8WebSocket::toNative(args.Holder());
     v8::Handle<v8::Value> message = args[0];
    ExceptionCode ec = 0;
    bool result;
    if (V8ArrayBuffer::HasInstance(message)) {
        ArrayBuffer* arrayBuffer = V8ArrayBuffer::toNative(v8::Handle<v8::Object>::Cast(message));
        ASSERT(arrayBuffer);
        result = webSocket->send(arrayBuffer, ec);
    } else if (V8Blob::HasInstance(message)) {
        Blob* blob = V8Blob::toNative(v8::Handle<v8::Object>::Cast(message));
        ASSERT(blob);
        result = webSocket->send(blob, ec);
    } else {
        v8::TryCatch tryCatch;
        v8::Handle<v8::String> stringMessage = message->ToString();
        if (tryCatch.HasCaught())
            return throwError(tryCatch.Exception(), args.GetIsolate());
        result = webSocket->send(toWebCoreString(stringMessage), ec);
    }
    if (ec)
        return throwError(ec, args.GetIsolate());

    return v8Boolean(result);
}
