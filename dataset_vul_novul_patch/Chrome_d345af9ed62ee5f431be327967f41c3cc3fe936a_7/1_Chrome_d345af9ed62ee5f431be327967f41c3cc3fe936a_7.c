static JSValueRef addTouchPointCallback(JSContextRef context, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    if (argumentCount < 2)
        return JSValueMakeUndefined(context);

    int x = static_cast<int>(JSValueToNumber(context, arguments[0], exception));
    ASSERT(!exception || !*exception);
     int y = static_cast<int>(JSValueToNumber(context, arguments[1], exception));
     ASSERT(!exception || !*exception);
 
    BlackBerry::Platform::TouchPoint touch;
    touch.m_id = touches.isEmpty() ? 0 : touches.last().m_id + 1;
     IntPoint pos(x, y);
    touch.m_pos = pos;
    touch.m_screenPos = pos;
    touch.m_state = BlackBerry::Platform::TouchPoint::TouchPressed;
 
     touches.append(touch);
 
    return JSValueMakeUndefined(context);
}
