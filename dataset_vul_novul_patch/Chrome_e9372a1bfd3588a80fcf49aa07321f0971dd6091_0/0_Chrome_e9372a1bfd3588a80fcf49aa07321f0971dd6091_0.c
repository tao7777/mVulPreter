static v8::Handle<v8::Value> excitingFunctionCallback(const v8::Arguments& args)
 {
     INC_STATS("DOM.TestActiveDOMObject.excitingFunction");
     if (args.Length() < 1)
        return V8Proxy::throwNotEnoughArgumentsError(args.GetIsolate());
     TestActiveDOMObject* imp = V8TestActiveDOMObject::toNative(args.Holder());
     if (!V8BindingSecurity::canAccessFrame(V8BindingState::Only(), imp->frame(), true))
         return v8::Handle<v8::Value>();
    EXCEPTION_BLOCK(Node*, nextChild, V8Node::HasInstance(MAYBE_MISSING_PARAMETER(args, 0, DefaultIsUndefined)) ? V8Node::toNative(v8::Handle<v8::Object>::Cast(MAYBE_MISSING_PARAMETER(args, 0, DefaultIsUndefined))) : 0);
    imp->excitingFunction(nextChild);
    return v8::Handle<v8::Value>();
}
