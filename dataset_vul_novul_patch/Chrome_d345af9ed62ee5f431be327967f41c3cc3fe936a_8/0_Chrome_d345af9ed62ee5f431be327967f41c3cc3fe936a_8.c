static JSValueRef releaseTouchPointCallback(JSContextRef context, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    if (argumentCount < 1)
        return JSValueMakeUndefined(context);

    int index = static_cast<int>(JSValueToNumber(context, arguments[0], exception));
    ASSERT(!exception || !*exception);
     if (index < 0 || index >= (int)touches.size())
         return JSValueMakeUndefined(context);
 
    touches[index].updateState(BlackBerry::Platform::TouchPoint::TouchReleased);
     return JSValueMakeUndefined(context);
 }
