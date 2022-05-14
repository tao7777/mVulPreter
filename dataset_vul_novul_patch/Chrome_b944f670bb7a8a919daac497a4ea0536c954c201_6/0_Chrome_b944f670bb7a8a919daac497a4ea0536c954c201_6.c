 JSValue JSWebKitMutationObserver::observe(ExecState* exec)
 {
     if (exec->argumentCount() < 2)
        return throwError(exec, createNotEnoughArgumentsError(exec));
     Node* target = toNode(exec->argument(0));
     if (exec->hadException())
         return jsUndefined();

    JSObject* optionsObject = exec->argument(1).getObject();
    if (!optionsObject) {
        setDOMException(exec, TYPE_MISMATCH_ERR);
        return jsUndefined();
    }

    JSDictionary dictionary(exec, optionsObject);
    MutationObserverOptions options = 0;
    for (unsigned i = 0; i < numBooleanOptions; ++i) {
        bool option = false;
        if (!dictionary.tryGetProperty(booleanOptions[i].name, option))
            return jsUndefined();
        if (option)
            options |= booleanOptions[i].value;
    }

    HashSet<AtomicString> attributeFilter;
    if (!dictionary.tryGetProperty("attributeFilter", attributeFilter))
        return jsUndefined();
    if (!attributeFilter.isEmpty())
        options |= WebKitMutationObserver::AttributeFilter;

    ExceptionCode ec = 0;
    impl()->observe(target, options, attributeFilter, ec);
    if (ec)
        setDOMException(exec, ec);
    return jsUndefined();
}
