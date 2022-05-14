void V8LazyEventListener::prepareListenerObject(ExecutionContext* executionContext)
{
    if (!executionContext)
        return;

    v8::HandleScope handleScope(toIsolate(executionContext));
    v8::Local<v8::Context> v8Context = toV8Context(executionContext, world());
    if (v8Context.IsEmpty())
        return;
    ScriptState* scriptState = ScriptState::from(v8Context);
     if (!scriptState->contextIsValid())
         return;
 
    if (!executionContext->isDocument())
        return;

    if (!toDocument(executionContext)->allowInlineEventHandlers(m_node, this, m_sourceURL, m_position.m_line)) {
         clearListenerObject();
         return;
     }
 
     if (hasExistingListenerObject())
         return;
 
     ScriptState::Scope scope(scriptState);
     String listenerSource =  InspectorInstrumentation::preprocessEventListener(toDocument(executionContext)->frame(), m_code, m_sourceURL, m_functionName);
 
    String code = "(function() {"
        "with (this[2]) {"
        "with (this[1]) {"
        "with (this[0]) {"
            "return function(" + m_eventParameterName + ") {" +
                listenerSource + "\n" // Insert '\n' otherwise //-style comments could break the handler.
            "};"
        "}}}})";

    v8::Handle<v8::String> codeExternalString = v8String(isolate(), code);

    v8::Local<v8::Value> result = V8ScriptRunner::compileAndRunInternalScript(codeExternalString, isolate(), m_sourceURL, m_position);
    if (result.IsEmpty())
         return;
 
    if (!result->IsFunction())
        return;
     v8::Local<v8::Function> intermediateFunction = result.As<v8::Function>();
 
     HTMLFormElement* formElement = 0;
    if (m_node && m_node->isHTMLElement())
        formElement = toHTMLElement(m_node)->formOwner();

    v8::Handle<v8::Object> nodeWrapper = toObjectWrapper<Node>(m_node, scriptState);
    v8::Handle<v8::Object> formWrapper = toObjectWrapper<HTMLFormElement>(formElement, scriptState);
    v8::Handle<v8::Object> documentWrapper = toObjectWrapper<Document>(m_node ? m_node->ownerDocument() : 0, scriptState);

    v8::Local<v8::Object> thisObject = v8::Object::New(isolate());
    if (thisObject.IsEmpty())
        return;
    if (!thisObject->ForceSet(v8::Integer::New(isolate(), 0), nodeWrapper))
        return;
    if (!thisObject->ForceSet(v8::Integer::New(isolate(), 1), formWrapper))
        return;
    if (!thisObject->ForceSet(v8::Integer::New(isolate(), 2), documentWrapper))
        return;

    v8::Local<v8::Value> innerValue = V8ScriptRunner::callInternalFunction(intermediateFunction, thisObject, 0, 0, isolate());
    if (innerValue.IsEmpty() || !innerValue->IsFunction())
        return;

    v8::Local<v8::Function> wrappedFunction = innerValue.As<v8::Function>();

    v8::Local<v8::Function> toStringFunction = v8::Function::New(isolate(), V8LazyEventListenerToString);
    ASSERT(!toStringFunction.IsEmpty());
    String toStringString = "function " + m_functionName + "(" + m_eventParameterName + ") {\n  " + m_code + "\n}";
    V8HiddenValue::setHiddenValue(isolate(), wrappedFunction, V8HiddenValue::toStringString(isolate()), v8String(isolate(), toStringString));
    wrappedFunction->Set(v8AtomicString(isolate(), "toString"), toStringFunction);
    wrappedFunction->SetName(v8String(isolate(), m_functionName));


    setListenerObject(wrappedFunction);
}
