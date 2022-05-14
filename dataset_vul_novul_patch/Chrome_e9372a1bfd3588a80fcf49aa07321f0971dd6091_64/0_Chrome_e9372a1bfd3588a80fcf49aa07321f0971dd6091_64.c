static v8::Handle<v8::Value> vertexAttribAndUniformHelperf(const v8::Arguments& args,
                                                           FunctionToCall functionToCall) {
 
     if (args.Length() != 2)
        return V8Proxy::throwNotEnoughArgumentsError(args.GetIsolate());
 
     bool ok = false;
     int index = -1;
    WebGLUniformLocation* location = 0;

    if (isFunctionToCallForAttribute(functionToCall))
        index = toInt32(args[0]);
    else {
        if (args.Length() > 0 && !isUndefinedOrNull(args[0]) && !V8WebGLUniformLocation::HasInstance(args[0])) {
            V8Proxy::throwTypeError();
            return notHandledByInterceptor();
        }
        location = toWebGLUniformLocation(args[0], ok);
    }

    WebGLRenderingContext* context = V8WebGLRenderingContext::toNative(args.Holder());

    if (V8Float32Array::HasInstance(args[1])) {
        Float32Array* array = V8Float32Array::toNative(args[1]->ToObject());
        ASSERT(array != NULL);
        ExceptionCode ec = 0;
        switch (functionToCall) {
            case kUniform1v: context->uniform1fv(location, array, ec); break;
            case kUniform2v: context->uniform2fv(location, array, ec); break;
            case kUniform3v: context->uniform3fv(location, array, ec); break;
            case kUniform4v: context->uniform4fv(location, array, ec); break;
            case kVertexAttrib1v: context->vertexAttrib1fv(index, array); break;
            case kVertexAttrib2v: context->vertexAttrib2fv(index, array); break;
            case kVertexAttrib3v: context->vertexAttrib3fv(index, array); break;
            case kVertexAttrib4v: context->vertexAttrib4fv(index, array); break;
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
    float* data = jsArrayToFloatArray(array, len);
    if (!data) {
        V8Proxy::setDOMException(SYNTAX_ERR, args.GetIsolate());
        return notHandledByInterceptor();
    }
    ExceptionCode ec = 0;
    switch (functionToCall) {
        case kUniform1v: context->uniform1fv(location, data, len, ec); break;
        case kUniform2v: context->uniform2fv(location, data, len, ec); break;
        case kUniform3v: context->uniform3fv(location, data, len, ec); break;
        case kUniform4v: context->uniform4fv(location, data, len, ec); break;
        case kVertexAttrib1v: context->vertexAttrib1fv(index, data, len); break;
        case kVertexAttrib2v: context->vertexAttrib2fv(index, data, len); break;
        case kVertexAttrib3v: context->vertexAttrib3fv(index, data, len); break;
        case kVertexAttrib4v: context->vertexAttrib4fv(index, data, len); break;
        default: ASSERT_NOT_REACHED(); break;
    }
    fastFree(data);
    if (ec)
        V8Proxy::setDOMException(ec, args.GetIsolate());
    return v8::Undefined();
}
