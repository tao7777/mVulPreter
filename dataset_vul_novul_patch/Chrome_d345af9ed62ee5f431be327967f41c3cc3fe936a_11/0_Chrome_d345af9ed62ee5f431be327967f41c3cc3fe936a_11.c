static JSValueRef updateTouchPointCallback(JSContextRef context, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    if (argumentCount < 3)
        return JSValueMakeUndefined(context);

    int index = static_cast<int>(JSValueToNumber(context, arguments[0], exception));
    ASSERT(!exception || !*exception);
    int x = static_cast<int>(JSValueToNumber(context, arguments[1], exception));
    ASSERT(!exception || !*exception);
    int y = static_cast<int>(JSValueToNumber(context, arguments[2], exception));
    ASSERT(!exception || !*exception);

    if (index < 0 || index >= (int)touches.size())
         return JSValueMakeUndefined(context);
 
     BlackBerry::Platform::TouchPoint& touch = touches[index];

    // pixelViewportPosition is unused in the WebKit layer
     IntPoint pos(x, y);

    // Unfortunately we don't know the scroll position at this point, so use pos for the content position too.
    // This assumes scroll position is 0,0
    touch.populateDocumentPosition(pos, pos);
    touch.setScreenPosition(pos);
    touch.updateState(BlackBerry::Platform::TouchPoint::TouchMoved);
 
     return JSValueMakeUndefined(context);
 }
