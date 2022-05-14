void V8Window::namedPropertyGetterCustom(v8::Local<v8::Name> name, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    if (!name->IsString())
        return;

    auto nameString = name.As<v8::String>();
    LocalDOMWindow* window = toLocalDOMWindow(V8Window::toImpl(info.Holder()));
    if (!window)
        return;

    LocalFrame* frame = window->frame();
    if (!frame)
        return;

    AtomicString propName = toCoreAtomicString(nameString);
    Frame* child = frame->tree().scopedChild(propName);
    if (child) {
        v8SetReturnValueFast(info, child->domWindow(), window);
        return;
    }

     if (!info.Holder()->GetRealNamedProperty(nameString).IsEmpty())
         return;
 
     Document* doc = frame->document();
 
    if (doc && doc->isHTMLDocument()) {
        if (toHTMLDocument(doc)->hasNamedItem(propName) || doc->hasElementWithId(propName)) {
            RefPtrWillBeRawPtr<HTMLCollection> items = doc->windowNamedItems(propName);
            if (!items->isEmpty()) {
                if (items->hasExactlyOneItem()) {
                    v8SetReturnValueFast(info, items->item(0), window);
                    return;
                }
                v8SetReturnValueFast(info, items.release(), window);
                return;
            }
        }
    }
}
