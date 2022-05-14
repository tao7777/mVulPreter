v8::Handle<v8::Value> V8WebGLRenderingContext::getUniformCallback(const v8::Arguments& args)
{
     INC_STATS("DOM.WebGLRenderingContext.getUniform()");
 
     if (args.Length() != 2)
        return V8Proxy::throwNotEnoughArgumentsError(args.GetIsolate());
 
     ExceptionCode ec = 0;
     WebGLRenderingContext* context = V8WebGLRenderingContext::toNative(args.Holder());
    if (args.Length() > 0 && !isUndefinedOrNull(args[0]) && !V8WebGLProgram::HasInstance(args[0])) {
        V8Proxy::throwTypeError();
        return notHandledByInterceptor();
    }
    WebGLProgram* program = V8WebGLProgram::HasInstance(args[0]) ? V8WebGLProgram::toNative(v8::Handle<v8::Object>::Cast(args[0])) : 0;

    if (args.Length() > 1 && !isUndefinedOrNull(args[1]) && !V8WebGLUniformLocation::HasInstance(args[1])) {
        V8Proxy::throwTypeError();
        return notHandledByInterceptor();
    }
    bool ok = false;
    WebGLUniformLocation* location = toWebGLUniformLocation(args[1], ok);

    WebGLGetInfo info = context->getUniform(program, location, ec);
    if (ec) {
        V8Proxy::setDOMException(ec, args.GetIsolate());
        return v8::Undefined();
    }
    return toV8Object(info, args.GetIsolate());
}
