 static v8::Handle<v8::Value> setValueAndClosePopupCallback(const v8::Arguments& args)
 {
     if (args.Length() < 2)
        return V8Proxy::throwNotEnoughArgumentsError();
     DOMWindow* imp = V8DOMWindow::toNative(args.Data()->ToObject());
     EXCEPTION_BLOCK(int, intValue, toInt32(MAYBE_MISSING_PARAMETER(args, 0, DefaultIsUndefined)));
     STRING_TO_V8PARAMETER_EXCEPTION_BLOCK(V8Parameter<>, stringValue, MAYBE_MISSING_PARAMETER(args, 1, DefaultIsUndefined));
    DOMWindowPagePopup::setValueAndClosePopup(imp, intValue, stringValue);
    return v8::Undefined();
}
