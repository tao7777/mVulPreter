EncodedJSValue JSC_HOST_CALL jsFloat64ArrayPrototypeFunctionFoo(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(&JSFloat64Array::s_info))
        return throwVMTypeError(exec);
    JSFloat64Array* castedThis = jsCast<JSFloat64Array*>(asObject(thisValue));
     ASSERT_GC_OBJECT_INHERITS(castedThis, &JSFloat64Array::s_info);
     Float64Array* impl = static_cast<Float64Array*>(castedThis->impl());
     if (exec->argumentCount() < 1)
        return throwVMError(exec, createNotEnoughArgumentsError(exec));
     Float32Array* array(toFloat32Array(MAYBE_MISSING_PARAMETER(exec, 0, DefaultIsUndefined)));
     if (exec->hadException())
         return JSValue::encode(jsUndefined());

    JSC::JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(impl->foo(array)));
    return JSValue::encode(result);
}
