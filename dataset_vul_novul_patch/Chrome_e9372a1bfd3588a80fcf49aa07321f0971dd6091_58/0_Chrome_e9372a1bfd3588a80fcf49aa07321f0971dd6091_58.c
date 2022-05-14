v8::Handle<v8::Value> V8WebGLRenderingContext::getParameterCallback(const v8::Arguments& args)
{
     INC_STATS("DOM.WebGLRenderingContext.getParameter()");
 
     if (args.Length() != 1)
        return V8Proxy::throwNotEnoughArgumentsError(args.GetIsolate());
 
     ExceptionCode ec = 0;
     WebGLRenderingContext* context = V8WebGLRenderingContext::toNative(args.Holder());
    unsigned pname = toInt32(args[0]);
    WebGLGetInfo info = context->getParameter(pname, ec);
    if (ec) {
        V8Proxy::setDOMException(ec, args.GetIsolate());
        return v8::Undefined();
    }
    return toV8Object(info, args.GetIsolate());
}
