bool V8DOMWindow::namedSecurityCheckCustom(v8::Local<v8::Object> host, v8::Local<v8::Value> key, v8::AccessType type, v8::Local<v8::Value>)
{
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::Handle<v8::Object> window = host->FindInstanceInPrototypeChain(V8DOMWindow::GetTemplate(isolate, worldTypeInMainThread(isolate)));
    if (window.IsEmpty())
        return false;  // the frame is gone.

    DOMWindow* targetWindow = V8DOMWindow::toNative(window);

    ASSERT(targetWindow);

    Frame* target = targetWindow->frame();
    if (!target)
        return false;

    if (target->loader()->stateMachine()->isDisplayingInitialEmptyDocument())
        target->loader()->didAccessInitialDocument();

    if (key->IsString()) {
        DEFINE_STATIC_LOCAL(AtomicString, nameOfProtoProperty, ("__proto__", AtomicString::ConstructFromLiteral));

        String name = toWebCoreString(key);
        Frame* childFrame = target->tree()->scopedChild(name);
        if (type == v8::ACCESS_HAS && childFrame)
            return true;
        v8::Handle<v8::String> keyString = key->ToString();
        if (type == v8::ACCESS_GET
            && childFrame
            && !host->HasRealNamedProperty(keyString)
            && !window->HasRealNamedProperty(keyString)
            && name != nameOfProtoProperty)
             return true;
     }
 
    return BindingSecurity::shouldAllowAccessToFrame(target, DoNotReportSecurityError);
}
