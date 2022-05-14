v8::Handle<v8::Value> V8SVGLength::convertToSpecifiedUnitsCallback(const v8::Arguments& args)
{
    INC_STATS("DOM.SVGLength.convertToSpecifiedUnits");
    SVGPropertyTearOff<SVGLength>* wrapper = V8SVGLength::toNative(args.Holder());
    if (wrapper->role() == AnimValRole) {
        V8Proxy::setDOMException(NO_MODIFICATION_ALLOWED_ERR, args.GetIsolate());
        return v8::Handle<v8::Value>();
     }
 
     if (args.Length() < 1)
        return V8Proxy::throwNotEnoughArgumentsError();
 
     SVGLength& imp = wrapper->propertyReference();
     ExceptionCode ec = 0;
    EXCEPTION_BLOCK(int, unitType, toUInt32(args[0]));
    SVGLengthContext lengthContext(wrapper->contextElement());
    imp.convertToSpecifiedUnits(unitType, lengthContext, ec);
    if (UNLIKELY(ec))
        V8Proxy::setDOMException(ec, args.GetIsolate());
    else
        wrapper->commitChange();
    return v8::Handle<v8::Value>();
}
