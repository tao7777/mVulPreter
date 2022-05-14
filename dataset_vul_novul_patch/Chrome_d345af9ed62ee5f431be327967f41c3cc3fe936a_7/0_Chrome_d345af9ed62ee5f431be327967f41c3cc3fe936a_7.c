static JSValueRef addTouchPointCallback(JSContextRef context, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    if (argumentCount < 2)
        return JSValueMakeUndefined(context);

    int x = static_cast<int>(JSValueToNumber(context, arguments[0], exception));
    ASSERT(!exception || !*exception);
     int y = static_cast<int>(JSValueToNumber(context, arguments[1], exception));
     ASSERT(!exception || !*exception);
 
    int id = touches.isEmpty() ? 0 : touches.last().id() + 1;

    // pixelViewportPosition is unused in the WebKit layer, so use this for screen position
     IntPoint pos(x, y);

    BlackBerry::Platform::TouchPoint touch(id, BlackBerry::Platform::TouchPoint::TouchPressed, pos, pos, 0);

    // Unfortunately we don't know the scroll position at this point, so use pos for the content position too.
    // This assumes scroll position is 0,0
    touch.populateDocumentPosition(pos, pos);
 
     touches.append(touch);
 
    return JSValueMakeUndefined(context);
}
