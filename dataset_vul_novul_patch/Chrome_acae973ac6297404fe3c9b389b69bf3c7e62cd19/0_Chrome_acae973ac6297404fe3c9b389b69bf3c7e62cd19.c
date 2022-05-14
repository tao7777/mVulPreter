v8::Handle<v8::Value> V8DOMWrapper::convertEventToV8Object(Event* event)
{
    if (!event)
        return v8::Null();

    v8::Handle<v8::Object> wrapper = getDOMObjectMap().get(event);
    if (!wrapper.IsEmpty())
        return wrapper;

    V8ClassIndex::V8WrapperType type = V8ClassIndex::EVENT;

    if (event->isUIEvent()) {
        if (event->isKeyboardEvent())
            type = V8ClassIndex::KEYBOARDEVENT;
        else if (event->isTextEvent())
            type = V8ClassIndex::TEXTEVENT;
        else if (event->isMouseEvent())
            type = V8ClassIndex::MOUSEEVENT;
        else if (event->isWheelEvent())
            type = V8ClassIndex::WHEELEVENT;
#if ENABLE(SVG)
        else if (event->isSVGZoomEvent())
            type = V8ClassIndex::SVGZOOMEVENT;
#endif
        else
            type = V8ClassIndex::UIEVENT;
    } else if (event->isMutationEvent())
        type = V8ClassIndex::MUTATIONEVENT;
    else if (event->isOverflowEvent())
        type = V8ClassIndex::OVERFLOWEVENT;
    else if (event->isMessageEvent())
        type = V8ClassIndex::MESSAGEEVENT;
    else if (event->isProgressEvent()) {
        if (event->isXMLHttpRequestProgressEvent())
            type = V8ClassIndex::XMLHTTPREQUESTPROGRESSEVENT;
        else
            type = V8ClassIndex::PROGRESSEVENT;
    } else if (event->isWebKitAnimationEvent())
         type = V8ClassIndex::WEBKITANIMATIONEVENT;
     else if (event->isWebKitTransitionEvent())
         type = V8ClassIndex::WEBKITTRANSITIONEVENT;
#if ENABLE(WORKERS)
    else if (event->isErrorEvent())
        type = V8ClassIndex::ERROREVENT;
#endif
 
 
     v8::Handle<v8::Object> result = instantiateV8Object(type, V8ClassIndex::EVENT, event);
    if (result.IsEmpty()) {
        return v8::Null();
    }

    event->ref(); // fast ref
    setJSWrapperForDOMObject(event, v8::Persistent<v8::Object>::New(result));

    return result;
}
