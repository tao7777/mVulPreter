void WebPage::touchPointAsMouseEvent(const Platform::TouchPoint& point, unsigned modifiers)
{
    if (d->m_page->defersLoading())
        return;

    if (d->m_fullScreenPluginView.get())
        return;
 
     d->m_lastUserEventTimestamp = currentTime();
 
    Platform::TouchPoint tPoint = point;
    tPoint.m_pos = d->mapFromTransformed(tPoint.m_pos);
    d->m_touchEventHandler->handleTouchPoint(tPoint, modifiers);
 }
