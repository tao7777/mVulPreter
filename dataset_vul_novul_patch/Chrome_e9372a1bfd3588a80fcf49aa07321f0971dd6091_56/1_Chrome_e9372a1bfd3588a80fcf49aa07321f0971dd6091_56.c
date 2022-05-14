v8::Handle<v8::Value> V8WebGLRenderingContext::getFramebufferAttachmentParameterCallback(const v8::Arguments& args)
{
     INC_STATS("DOM.WebGLRenderingContext.getFramebufferAttachmentParameter()");
 
     if (args.Length() != 3)
        return V8Proxy::throwNotEnoughArgumentsError();
 
     ExceptionCode ec = 0;
     WebGLRenderingContext* context = V8WebGLRenderingContext::toNative(args.Holder());
    unsigned target = toInt32(args[0]);
    unsigned attachment = toInt32(args[1]);
    unsigned pname = toInt32(args[2]);
    WebGLGetInfo info = context->getFramebufferAttachmentParameter(target, attachment, pname, ec);
    if (ec) {
        V8Proxy::setDOMException(ec, args.GetIsolate());
        return v8::Undefined();
    }
    return toV8Object(info, args.GetIsolate());
}
