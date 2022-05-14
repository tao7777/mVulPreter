 static v8::Handle<v8::Value> getNamedProperty(HTMLDocument* htmlDocument, const AtomicString& key, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
 {
    if (!htmlDocument->hasNamedItem(key.impl()) && !htmlDocument->hasExtraNamedItem(key.impl()))
         return v8Undefined();
 
     RefPtr<HTMLCollection> items = htmlDocument->documentNamedItems(key);
    if (items->isEmpty())
        return v8Undefined();

    if (items->hasExactlyOneItem()) {
        Node* node = items->item(0);
        Frame* frame = 0;
        if (node->hasTagName(HTMLNames::iframeTag) && (frame = toHTMLIFrameElement(node)->contentFrame()))
            return toV8(frame->domWindow(), creationContext, isolate);
        return toV8(node, creationContext, isolate);
    }
    return toV8(items.release(), creationContext, isolate);
}
