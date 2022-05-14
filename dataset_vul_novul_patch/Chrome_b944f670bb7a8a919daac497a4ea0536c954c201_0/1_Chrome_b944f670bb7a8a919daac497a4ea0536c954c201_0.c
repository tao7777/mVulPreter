 static JSValue setDataViewMember(ExecState* exec, DataView* imp, DataViewAccessType type)
 {
     if (exec->argumentCount() < 2)
        return throwError(exec, createTypeError(exec, "Not enough arguments"));
     ExceptionCode ec = 0;
     unsigned byteOffset = exec->argument(0).toUInt32(exec);
     if (exec->hadException())
        return jsUndefined();
    int value = exec->argument(1).toInt32(exec);
    if (exec->hadException())
        return jsUndefined();
        
    switch (type) {
    case AccessDataViewMemberAsInt8:
        imp->setInt8(byteOffset, static_cast<int8_t>(value), ec);
        break;
    case AccessDataViewMemberAsUint8:
        imp->setUint8(byteOffset, static_cast<uint8_t>(value), ec);
        break;
    default:
        ASSERT_NOT_REACHED();
        break;
    }
    setDOMException(exec, ec);
    return jsUndefined();
}
