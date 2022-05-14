void getScriptableObjectProperty(PropertyType property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    HTMLPlugInElement* impl = ElementType::toImpl(info.Holder());
    RefPtr<SharedPersistent<v8::Object>> wrapper = impl->pluginWrapper();
    if (!wrapper)
        return;

    v8::Local<v8::Object> instance = wrapper->newLocal(info.GetIsolate());
     if (instance.IsEmpty())
         return;
 
    v8::Local<v8::Value> value;
    if (!instance->Get(info.GetIsolate()->GetCurrentContext(), property).ToLocal(&value))
         return;
 
    if (value->IsUndefined() && !v8CallBoolean(instance->Has(info.GetIsolate()->GetCurrentContext(), property)))
         return;
 
     v8SetReturnValue(info, value);
}
