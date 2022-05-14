v8::Handle<v8::Value> V8WebGLRenderingContext::getProgramParameterCallback(const v8::Arguments& args)
{
     INC_STATS("DOM.WebGLRenderingContext.getProgramParameter()");
 
     if (args.Length() != 2)
        return V8Proxy::throwNotEnoughArgumentsError();
 
     ExceptionCode ec = 0;
     WebGLRenderingContext* context = V8WebGLRenderingContext::toNative(args.Holder());
    if (args.Length() > 0 && !isUndefinedOrNull(args[0]) && !V8WebGLProgram::HasInstance(args[0])) {
        V8Proxy::throwTypeError();
        return notHandledByInterceptor();
    }
    WebGLProgram* program = V8WebGLProgram::HasInstance(args[0]) ? V8WebGLProgram::toNative(v8::Handle<v8::Object>::Cast(args[0])) : 0;
    unsigned pname = toInt32(args[1]);
    WebGLGetInfo info = context->getProgramParameter(program, pname, ec);
    if (ec) {
        V8Proxy::setDOMException(ec, args.GetIsolate());
        return v8::Undefined();
    }
    return toV8Object(info, args.GetIsolate());
}
