void setScriptableObjectProperty(PropertyType property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    ASSERT(!value.IsEmpty());
    HTMLPlugInElement* impl = ElementType::toImpl(info.Holder());
    RefPtr<SharedPersistent<v8::Object>> wrapper = impl->pluginWrapper();
    if (!wrapper)
        return;

    v8::Local<v8::Object> instance = wrapper->newLocal(info.GetIsolate());
    if (instance.IsEmpty())
        return;

    v8::Maybe<bool> unused = instance->Set(info.GetIsolate()->GetCurrentContext(), property, value);
    ALLOW_UNUSED_LOCAL(unused);
 }
