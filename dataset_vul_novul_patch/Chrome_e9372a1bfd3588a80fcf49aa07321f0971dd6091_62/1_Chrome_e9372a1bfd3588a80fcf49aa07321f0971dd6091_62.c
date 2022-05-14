static v8::Handle<v8::Value> uniformHelperi(const v8::Arguments& args,
                                            FunctionToCall functionToCall) {
 
     if (args.Length() != 2)
        return V8Proxy::throwNotEnoughArgumentsError();
 
     WebGLRenderingContext* context = V8WebGLRenderingContext::toNative(args.Holder());
     if (args.Length() > 0 && !isUndefinedOrNull(args[0]) && !V8WebGLUniformLocation::HasInstance(args[0])) {
        V8Proxy::throwTypeError();
        return notHandledByInterceptor();
    }
    bool ok = false;
    WebGLUniformLocation* location = toWebGLUniformLocation(args[0], ok);

    if (V8Int32Array::HasInstance(args[1])) {
        Int32Array* array = V8Int32Array::toNative(args[1]->ToObject());
        ASSERT(array != NULL);
        ExceptionCode ec = 0;
        switch (functionToCall) {
            case kUniform1v: context->uniform1iv(location, array, ec); break;
            case kUniform2v: context->uniform2iv(location, array, ec); break;
            case kUniform3v: context->uniform3iv(location, array, ec); break;
            case kUniform4v: context->uniform4iv(location, array, ec); break;
            default: ASSERT_NOT_REACHED(); break;
        }
        if (ec)
            V8Proxy::setDOMException(ec, args.GetIsolate());
        return v8::Undefined();
    }

    if (args[1].IsEmpty() || !args[1]->IsArray()) {
        V8Proxy::throwTypeError();
        return notHandledByInterceptor();
    }
    v8::Handle<v8::Array> array =
      v8::Local<v8::Array>::Cast(args[1]);
    uint32_t len = array->Length();
    int* data = jsArrayToIntArray(array, len);
    if (!data) {
        V8Proxy::setDOMException(SYNTAX_ERR, args.GetIsolate());
        return notHandledByInterceptor();
    }
    ExceptionCode ec = 0;
    switch (functionToCall) {
        case kUniform1v: context->uniform1iv(location, data, len, ec); break;
        case kUniform2v: context->uniform2iv(location, data, len, ec); break;
        case kUniform3v: context->uniform3iv(location, data, len, ec); break;
        case kUniform4v: context->uniform4iv(location, data, len, ec); break;
        default: ASSERT_NOT_REACHED(); break;
    }
    fastFree(data);
    if (ec)
        V8Proxy::setDOMException(ec, args.GetIsolate());
    return v8::Undefined();
}
