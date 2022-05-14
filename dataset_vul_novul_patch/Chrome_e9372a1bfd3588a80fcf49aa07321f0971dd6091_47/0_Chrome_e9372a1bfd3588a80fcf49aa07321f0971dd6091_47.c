v8::Handle<v8::Value> V8DataView::getUint8Callback(const v8::Arguments& args)
 {
     INC_STATS("DOM.DataView.getUint8");
     if (args.Length() < 1)
        return V8Proxy::throwNotEnoughArgumentsError(args.GetIsolate());
 
     DataView* imp = V8DataView::toNative(args.Holder());
     ExceptionCode ec = 0;
    EXCEPTION_BLOCK(unsigned, byteOffset, toUInt32(args[0]));
    uint8_t result = imp->getUint8(byteOffset, ec);
    if (UNLIKELY(ec)) {
        V8Proxy::setDOMException(ec, args.GetIsolate());
        return v8::Handle<v8::Value>();
    }
    return v8::Integer::New(result);
}
