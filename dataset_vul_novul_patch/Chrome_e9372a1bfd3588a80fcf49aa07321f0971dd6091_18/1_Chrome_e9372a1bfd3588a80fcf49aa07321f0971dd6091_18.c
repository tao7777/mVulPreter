static v8::Handle<v8::Value> idbKeyCallback(const v8::Arguments& args)
 {
     INC_STATS("DOM.TestObj.idbKey");
     if (args.Length() < 1)
        return V8Proxy::throwNotEnoughArgumentsError();
     TestObj* imp = V8TestObj::toNative(args.Holder());
     EXCEPTION_BLOCK(RefPtr<IDBKey>, key, createIDBKeyFromValue(MAYBE_MISSING_PARAMETER(args, 0, DefaultIsUndefined)));
     imp->idbKey(key.get());
    return v8::Handle<v8::Value>();
}
