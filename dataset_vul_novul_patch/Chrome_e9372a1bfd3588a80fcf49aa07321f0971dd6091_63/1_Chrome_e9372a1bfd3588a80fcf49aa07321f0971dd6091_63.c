static v8::Handle<v8::Value> uniformMatrixHelper(const v8::Arguments& args,
                                                 int matrixSize)
{
     if (args.Length() != 3)
        return V8Proxy::throwNotEnoughArgumentsError();
 
     WebGLRenderingContext* context = V8WebGLRenderingContext::toNative(args.Holder());
 
    if (args.Length() > 0 && !isUndefinedOrNull(args[0]) && !V8WebGLUniformLocation::HasInstance(args[0])) {
        V8Proxy::throwTypeError();
        return notHandledByInterceptor();
    }
    bool ok = false;
    WebGLUniformLocation* location = toWebGLUniformLocation(args[0], ok);
    
    bool transpose = args[1]->BooleanValue();
    if (V8Float32Array::HasInstance(args[2])) {
        Float32Array* array = V8Float32Array::toNative(args[2]->ToObject());
        ASSERT(array != NULL);
        ExceptionCode ec = 0;
        switch (matrixSize) {
            case 2: context->uniformMatrix2fv(location, transpose, array, ec); break;
            case 3: context->uniformMatrix3fv(location, transpose, array, ec); break;
            case 4: context->uniformMatrix4fv(location, transpose, array, ec); break;
            default: ASSERT_NOT_REACHED(); break;
        }
        if (ec)
            V8Proxy::setDOMException(ec, args.GetIsolate());
        return v8::Undefined();
    }

    if (args[2].IsEmpty() || !args[2]->IsArray()) {
        V8Proxy::throwTypeError();
        return notHandledByInterceptor();
    }
    v8::Handle<v8::Array> array =
      v8::Local<v8::Array>::Cast(args[2]);
    uint32_t len = array->Length();
    float* data = jsArrayToFloatArray(array, len);
    if (!data) {
        V8Proxy::setDOMException(SYNTAX_ERR, args.GetIsolate());
        return notHandledByInterceptor();
    }
    ExceptionCode ec = 0;
    switch (matrixSize) {
        case 2: context->uniformMatrix2fv(location, transpose, data, len, ec); break;
        case 3: context->uniformMatrix3fv(location, transpose, data, len, ec); break;
        case 4: context->uniformMatrix4fv(location, transpose, data, len, ec); break;
        default: ASSERT_NOT_REACHED(); break;
    }
    fastFree(data);
    if (ec)
        V8Proxy::setDOMException(ec, args.GetIsolate()); 
    return v8::Undefined();
}
