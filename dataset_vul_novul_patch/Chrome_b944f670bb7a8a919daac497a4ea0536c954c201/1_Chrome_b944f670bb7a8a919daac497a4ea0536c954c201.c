 static JSValue getDataViewMember(ExecState* exec, DataView* imp, DataViewAccessType type)
 {
     if (exec->argumentCount() < 1)
        return throwError(exec, createTypeError(exec, "Not enough arguments"));
     ExceptionCode ec = 0;
     unsigned byteOffset = exec->argument(0).toUInt32(exec);
     if (exec->hadException())
        return jsUndefined();

    bool littleEndian = false;
    if (exec->argumentCount() > 1 && (type == AccessDataViewMemberAsFloat32 || type == AccessDataViewMemberAsFloat64)) {
        littleEndian = exec->argument(1).toBoolean(exec);
        if (exec->hadException())
            return jsUndefined();
    }

    JSC::JSValue result;
    switch (type) {
    case AccessDataViewMemberAsInt8:
        result = jsNumber(imp->getInt8(byteOffset, ec));
        break;
    case AccessDataViewMemberAsUint8:
        result = jsNumber(imp->getUint8(byteOffset, ec));
        break;
    case AccessDataViewMemberAsFloat32:
    case AccessDataViewMemberAsFloat64: {
        double value = (type == AccessDataViewMemberAsFloat32) ? imp->getFloat32(byteOffset, littleEndian, ec) : imp->getFloat64(byteOffset, littleEndian, ec);
        result = isnan(value) ? jsNaN() : jsNumber(value);
        break;
    } default:
        ASSERT_NOT_REACHED();
        break;
    }
    setDOMException(exec, ec);
    return result;
}
