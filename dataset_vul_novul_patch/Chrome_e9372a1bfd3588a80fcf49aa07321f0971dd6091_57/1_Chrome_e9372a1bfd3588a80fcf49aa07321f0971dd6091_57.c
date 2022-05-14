 static v8::Handle<v8::Value> getObjectParameter(const v8::Arguments& args, ObjectType objectType)
 {
     if (args.Length() != 2)
        return V8Proxy::throwNotEnoughArgumentsError();
 
     ExceptionCode ec = 0;
     WebGLRenderingContext* context = V8WebGLRenderingContext::toNative(args.Holder());
    unsigned target = toInt32(args[0]);
    unsigned pname = toInt32(args[1]);
    WebGLGetInfo info;
    switch (objectType) {
    case kBuffer:
        info = context->getBufferParameter(target, pname, ec);
        break;
    case kRenderbuffer:
        info = context->getRenderbufferParameter(target, pname, ec);
        break;
    case kTexture:
        info = context->getTexParameter(target, pname, ec);
        break;
    case kVertexAttrib:
        info = context->getVertexAttrib(target, pname, ec);
        break;
    default:
        notImplemented();
        break;
    }
    if (ec) {
        V8Proxy::setDOMException(ec, args.GetIsolate());
        return v8::Undefined();
    }
    return toV8Object(info, args.GetIsolate());
}
