v8::Handle<v8::Value> V8DirectoryEntry::getDirectoryCallback(const v8::Arguments& args)
{
    INC_STATS("DOM.DirectoryEntry.getDirectory");
     DirectoryEntry* imp = V8DirectoryEntry::toNative(args.Holder());
 
     if (args.Length() < 1)
        return V8Proxy::throwNotEnoughArgumentsError();
 
     STRING_TO_V8PARAMETER_EXCEPTION_BLOCK(V8Parameter<WithUndefinedOrNullCheck>, path, args[0]);
     if (args.Length() <= 1) {
        imp->getDirectory(path);
        return v8::Handle<v8::Value>();
    }
    RefPtr<WebKitFlags> flags;
    if (!isUndefinedOrNull(args[1]) && args[1]->IsObject()) {
        EXCEPTION_BLOCK(v8::Handle<v8::Object>, object, v8::Handle<v8::Object>::Cast(args[1]));
        flags = WebKitFlags::create();
        v8::Local<v8::Value> v8Create = object->Get(v8::String::New("create"));
        if (!v8Create.IsEmpty() && !isUndefinedOrNull(v8Create)) {
            EXCEPTION_BLOCK(bool, isCreate, v8Create->BooleanValue());
            flags->setCreate(isCreate);
        }
        v8::Local<v8::Value> v8Exclusive = object->Get(v8::String::New("exclusive"));
        if (!v8Exclusive.IsEmpty() && !isUndefinedOrNull(v8Exclusive)) {
            EXCEPTION_BLOCK(bool, isExclusive, v8Exclusive->BooleanValue());
            flags->setExclusive(isExclusive);
        }
    }
    RefPtr<EntryCallback> successCallback;
    if (args.Length() > 2 && !args[2]->IsNull() && !args[2]->IsUndefined()) {
        if (!args[2]->IsObject())
            return throwError(TYPE_MISMATCH_ERR, args.GetIsolate());
        successCallback = V8EntryCallback::create(args[2], getScriptExecutionContext());
    }
    RefPtr<ErrorCallback> errorCallback;
    if (args.Length() > 3 && !args[3]->IsNull() && !args[3]->IsUndefined()) {
        if (!args[3]->IsObject())
            return throwError(TYPE_MISMATCH_ERR, args.GetIsolate());
        errorCallback = V8ErrorCallback::create(args[3], getScriptExecutionContext());
    }
    imp->getDirectory(path, flags, successCallback, errorCallback);
    return v8::Handle<v8::Value>();
}
