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
     IntPoint pos(x, y);
    touch.m_pos = pos;
    touch.m_screenPos = pos;
    touch.m_state = BlackBerry::Platform::TouchPoint::TouchMoved;
 
     return JSValueMakeUndefined(context);
 }
