 v8::MaybeLocal<v8::Array> V8Debugger::internalProperties(v8::Local<v8::Context> context, v8::Local<v8::Value> value)
{
    v8::Local<v8::Array> properties;
    if (!v8::Debug::GetInternalProperties(m_isolate, value).ToLocal(&properties))
        return v8::MaybeLocal<v8::Array>();
    if (value->IsFunction()) {
        v8::Local<v8::Function> function = value.As<v8::Function>();
        v8::Local<v8::Value> location = functionLocation(context, function);
        if (location->IsObject()) {
            properties->Set(properties->Length(), toV8StringInternalized(m_isolate, "[[FunctionLocation]]"));
            properties->Set(properties->Length(), location);
        }
        if (function->IsGeneratorFunction()) {
            properties->Set(properties->Length(), toV8StringInternalized(m_isolate, "[[IsGenerator]]"));
            properties->Set(properties->Length(), v8::True(m_isolate));
        }
    }
    if (!enabled())
        return properties;
    if (value->IsMap() || value->IsWeakMap() || value->IsSet() || value->IsWeakSet() || value->IsSetIterator() || value->IsMapIterator()) {
        v8::Local<v8::Value> entries = collectionEntries(context, v8::Local<v8::Object>::Cast(value));
        if (entries->IsArray()) {
            properties->Set(properties->Length(), toV8StringInternalized(m_isolate, "[[Entries]]"));
            properties->Set(properties->Length(), entries);
         }
     }
     if (value->IsGeneratorObject()) {
        v8::Local<v8::Value> location = generatorObjectLocation(context, v8::Local<v8::Object>::Cast(value));
         if (location->IsObject()) {
             properties->Set(properties->Length(), toV8StringInternalized(m_isolate, "[[GeneratorLocation]]"));
             properties->Set(properties->Length(), location);
        }
    }
    if (value->IsFunction()) {
         v8::Local<v8::Function> function = value.As<v8::Function>();
         v8::Local<v8::Value> boundFunction = function->GetBoundFunction();
         v8::Local<v8::Value> scopes;
        if (boundFunction->IsUndefined() && functionScopes(context, function).ToLocal(&scopes)) {
             properties->Set(properties->Length(), toV8StringInternalized(m_isolate, "[[Scopes]]"));
             properties->Set(properties->Length(), scopes);
         }
    }
    return properties;
}
