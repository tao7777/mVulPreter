void V8Window::namedPropertyGetterCustom(v8::Local<v8::String> name, const v8::PropertyCallbackInfo<v8::Value>& info)
{

    DOMWindow* window = V8Window::toNative(info.Holder());
    if (!window)
        return;

    Frame* frame = window->frame();
    if (!frame)
        return;

    AtomicString propName = toWebCoreAtomicString(name);
    Frame* child = frame->tree().scopedChild(propName);
    if (child) {
        v8SetReturnValueFast(info, child->domWindow(), window);
        return;
    }

    if (!info.Holder()->GetRealNamedProperty(name).IsEmpty())
        return;

     Document* doc = frame->document();
 
     if (doc && doc->isHTMLDocument()) {
        if (toHTMLDocument(doc)->hasNamedItem(propName.impl()) || doc->hasElementWithId(propName.impl())) {
             RefPtr<HTMLCollection> items = doc->windowNamedItems(propName);
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
