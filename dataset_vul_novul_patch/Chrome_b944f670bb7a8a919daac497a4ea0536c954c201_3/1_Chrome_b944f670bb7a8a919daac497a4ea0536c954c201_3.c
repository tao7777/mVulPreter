 JSValue JSDirectoryEntry::getFile(ExecState* exec)
 {
     if (exec->argumentCount() < 1)
        return throwError(exec, createTypeError(exec, "Not enough arguments"));
 
     DirectoryEntry* imp = static_cast<DirectoryEntry*>(impl());
     const String& path = valueToStringWithUndefinedOrNullCheck(exec, exec->argument(0));
    if (exec->hadException())
        return jsUndefined();

    int argsCount = exec->argumentCount();
    if (argsCount <= 1) {
        imp->getFile(path);
        return jsUndefined();
    }

    RefPtr<WebKitFlags> flags;
    if (!exec->argument(1).isNull() && !exec->argument(1).isUndefined() && exec->argument(1).isObject()) {
        JSObject* object = exec->argument(1).getObject();
        flags = WebKitFlags::create();
        JSValue jsCreate = object->get(exec, Identifier(exec, "create"));
        flags->setCreate(jsCreate.toBoolean(exec));
        JSValue jsExclusive = object->get(exec, Identifier(exec, "exclusive"));
        flags->setExclusive(jsExclusive.toBoolean(exec));
    }
    if (exec->hadException())
        return jsUndefined();
    RefPtr<EntryCallback> successCallback;
    if (exec->argumentCount() > 2 && !exec->argument(2).isNull() && !exec->argument(2).isUndefined()) {
        if (!exec->argument(2).isObject()) {
            setDOMException(exec, TYPE_MISMATCH_ERR);
            return jsUndefined();
        }
        successCallback = JSEntryCallback::create(asObject(exec->argument(2)), globalObject());
    }
    RefPtr<ErrorCallback> errorCallback;
    if (exec->argumentCount() > 3 && !exec->argument(3).isNull() && !exec->argument(3).isUndefined()) {
        if (!exec->argument(3).isObject()) {
            setDOMException(exec, TYPE_MISMATCH_ERR);
            return jsUndefined();
        }
        errorCallback = JSErrorCallback::create(asObject(exec->argument(3)), globalObject());
    }

    imp->getFile(path, flags, successCallback, errorCallback);
    return jsUndefined();
}
