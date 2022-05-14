v8::Handle<v8::Value> V8DataView::setInt8Callback(const v8::Arguments& args)
 {
     INC_STATS("DOM.DataView.setInt8");
     if (args.Length() < 2)
        return V8Proxy::throwNotEnoughArgumentsError(args.GetIsolate());
 
     DataView* imp = V8DataView::toNative(args.Holder());
     ExceptionCode ec = 0;
    EXCEPTION_BLOCK(unsigned, byteOffset, toUInt32(args[0]));
    EXCEPTION_BLOCK(int, value, toInt32(args[1]));
    imp->setInt8(byteOffset, static_cast<int8_t>(value), ec);
    if (UNLIKELY(ec))
        V8Proxy::setDOMException(ec, args.GetIsolate());
    return v8::Handle<v8::Value>();
}
