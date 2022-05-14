v8::Local<v8::Object> V8InjectedScriptHost::create(v8::Local<v8::Context> context, V8InspectorImpl* inspector)
 {
     v8::Isolate* isolate = inspector->isolate();
     v8::Local<v8::Object> injectedScriptHost = v8::Object::New(isolate);
     v8::Local<v8::External> debuggerExternal = v8::External::New(isolate, inspector);
     setFunctionProperty(context, injectedScriptHost, "internalConstructorName", V8InjectedScriptHost::internalConstructorNameCallback, debuggerExternal);
     setFunctionProperty(context, injectedScriptHost, "formatAccessorsAsProperties", V8InjectedScriptHost::formatAccessorsAsProperties, debuggerExternal);
    setFunctionProperty(context, injectedScriptHost, "subtype", V8InjectedScriptHost::subtypeCallback, debuggerExternal);
    setFunctionProperty(context, injectedScriptHost, "getInternalProperties", V8InjectedScriptHost::getInternalPropertiesCallback, debuggerExternal);
    setFunctionProperty(context, injectedScriptHost, "objectHasOwnProperty", V8InjectedScriptHost::objectHasOwnPropertyCallback, debuggerExternal);
    setFunctionProperty(context, injectedScriptHost, "bind", V8InjectedScriptHost::bindCallback, debuggerExternal);
    setFunctionProperty(context, injectedScriptHost, "proxyTargetValue", V8InjectedScriptHost::proxyTargetValueCallback, debuggerExternal);
    return injectedScriptHost;
}
