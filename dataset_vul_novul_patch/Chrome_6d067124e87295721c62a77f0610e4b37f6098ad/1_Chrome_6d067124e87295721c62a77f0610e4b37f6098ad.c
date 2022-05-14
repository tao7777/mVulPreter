v8::Handle<v8::Value> V8ThrowException::createDOMException(v8::Isolate* isolate, int ec, const String& sanitizedMessage, const String& unsanitizedMessage, const v8::Handle<v8::Object>& creationContext)
{
    if (ec <= 0 || v8::V8::IsExecutionTerminating())
        return v8Undefined();

    ASSERT(ec == SecurityError || unsanitizedMessage.isEmpty());

    if (ec == V8GeneralError)
        return V8ThrowException::createGeneralError(isolate, sanitizedMessage);
    if (ec == V8TypeError)
        return V8ThrowException::createTypeError(isolate, sanitizedMessage);
    if (ec == V8RangeError)
        return V8ThrowException::createRangeError(isolate, sanitizedMessage);
    if (ec == V8SyntaxError)
        return V8ThrowException::createSyntaxError(isolate, sanitizedMessage);
     if (ec == V8ReferenceError)
         return V8ThrowException::createReferenceError(isolate, sanitizedMessage);
 
     RefPtrWillBeRawPtr<DOMException> domException = DOMException::create(ec, sanitizedMessage, unsanitizedMessage);
    v8::Handle<v8::Value> exception = toV8(domException.get(), creationContext, isolate);
 
     if (exception.IsEmpty())
         return v8Undefined();

    v8::Handle<v8::Value> error = v8::Exception::Error(v8String(isolate, domException->message()));
    ASSERT(!error.IsEmpty());
    ASSERT(exception->IsObject());
    exception->ToObject(isolate)->SetAccessor(v8AtomicString(isolate, "stack"), domExceptionStackGetter, domExceptionStackSetter, error);
    V8HiddenValue::setHiddenValue(isolate, exception->ToObject(isolate), V8HiddenValue::error(isolate), error);

    return exception;
}
