v8::Handle<v8::Value> V8WebKitMutationObserver::observeCallback(const v8::Arguments& args)
 {
     INC_STATS("DOM.WebKitMutationObserver.observe");
     if (args.Length() < 2)
        return V8Proxy::throwNotEnoughArgumentsError(args.GetIsolate());
     WebKitMutationObserver* imp = V8WebKitMutationObserver::toNative(args.Holder());
     EXCEPTION_BLOCK(Node*, target, V8Node::HasInstance(args[0]) ? V8Node::toNative(v8::Handle<v8::Object>::Cast(args[0])) : 0);
 
    if (!args[1]->IsObject())
        return throwError(TYPE_MISMATCH_ERR, args.GetIsolate());

    Dictionary optionsObject(args[1]);
    unsigned options = 0;
    HashSet<AtomicString> attributeFilter;
    bool option;
    if (optionsObject.get("childList", option) && option)
        options |= WebKitMutationObserver::ChildList;
    if (optionsObject.get("attributes", option) && option)
        options |= WebKitMutationObserver::Attributes;
    if (optionsObject.get("attributeFilter", attributeFilter))
        options |= WebKitMutationObserver::AttributeFilter;
    if (optionsObject.get("characterData", option) && option)
        options |= WebKitMutationObserver::CharacterData;
    if (optionsObject.get("subtree", option) && option)
        options |= WebKitMutationObserver::Subtree;
    if (optionsObject.get("attributeOldValue", option) && option)
        options |= WebKitMutationObserver::AttributeOldValue;
    if (optionsObject.get("characterDataOldValue", option) && option)
        options |= WebKitMutationObserver::CharacterDataOldValue;

    ExceptionCode ec = 0;
    imp->observe(target, options, attributeFilter, ec);
    if (ec)
        V8Proxy::setDOMException(ec, args.GetIsolate());
    return v8::Handle<v8::Value>();
}
