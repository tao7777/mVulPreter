bool WebPagePrivate::dispatchTouchEventToFullScreenPlugin(PluginView* plugin, const Platform::TouchEvent& event)
{
    if (!event.neverHadMultiTouch())
        return false;

    if (event.isDoubleTap() || event.isTouchHold() || event.m_type == Platform::TouchEvent::TouchCancel) {
        NPTouchEvent npTouchEvent;

        if (event.isDoubleTap())
            npTouchEvent.type = TOUCH_EVENT_DOUBLETAP;
        else if (event.isTouchHold())
            npTouchEvent.type = TOUCH_EVENT_TOUCHHOLD;
        else if (event.m_type == Platform::TouchEvent::TouchCancel)
            npTouchEvent.type = TOUCH_EVENT_CANCEL;

        npTouchEvent.points = 0;
        npTouchEvent.size = event.m_points.size();
         if (npTouchEvent.size) {
             npTouchEvent.points = new NPTouchPoint[npTouchEvent.size];
             for (int i = 0; i < npTouchEvent.size; i++) {
                npTouchEvent.points[i].touchId = event.m_points[i].id();
                npTouchEvent.points[i].clientX = event.m_points[i].screenPosition().x();
                npTouchEvent.points[i].clientY = event.m_points[i].screenPosition().y();
                npTouchEvent.points[i].screenX = event.m_points[i].screenPosition().x();
                npTouchEvent.points[i].screenY = event.m_points[i].screenPosition().y();
                npTouchEvent.points[i].pageX = event.m_points[i].pixelViewportPosition().x();
                npTouchEvent.points[i].pageY = event.m_points[i].pixelViewportPosition().y();
             }
         }
 
        NPEvent npEvent;
        npEvent.type = NP_TouchEvent;
        npEvent.data = &npTouchEvent;

        plugin->dispatchFullScreenNPEvent(npEvent);
        delete[] npTouchEvent.points;
        return true;
    }

    dispatchTouchPointAsMouseEventToFullScreenPlugin(plugin, event.m_points[0]);
    return true;
}
