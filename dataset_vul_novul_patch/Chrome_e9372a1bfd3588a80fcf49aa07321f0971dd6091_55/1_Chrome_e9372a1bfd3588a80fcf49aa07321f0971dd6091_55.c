v8::Handle<v8::Value> V8WebGLRenderingContext::getExtensionCallback(const v8::Arguments& args)
{
     INC_STATS("DOM.WebGLRenderingContext.getExtensionCallback()");
     WebGLRenderingContext* imp = V8WebGLRenderingContext::toNative(args.Holder());
     if (args.Length() < 1)
        return V8Proxy::throwNotEnoughArgumentsError();
     STRING_TO_V8PARAMETER_EXCEPTION_BLOCK(V8Parameter<>, name, args[0]);
     WebGLExtension* extension = imp->getExtension(name);
     return toV8Object(extension, args.Holder(), args.GetIsolate());
}
