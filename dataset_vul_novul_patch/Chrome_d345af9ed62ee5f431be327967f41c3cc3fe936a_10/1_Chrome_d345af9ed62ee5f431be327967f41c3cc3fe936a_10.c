 static JSValueRef touchEndCallback(JSContextRef context, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
 {
     for (unsigned i = 0; i < touches.size(); ++i)
        if (touches[i].m_state != BlackBerry::Platform::TouchPoint::TouchReleased) {
             sendTouchEvent(BlackBerry::Platform::TouchEvent::TouchMove);
             return JSValueMakeUndefined(context);
         }
    sendTouchEvent(BlackBerry::Platform::TouchEvent::TouchEnd);
    touchActive = false;
    return JSValueMakeUndefined(context);
}
