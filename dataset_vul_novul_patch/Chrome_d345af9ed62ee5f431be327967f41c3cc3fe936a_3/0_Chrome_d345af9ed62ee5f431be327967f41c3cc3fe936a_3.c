void WebPage::touchPointAsMouseEvent(const Platform::TouchPoint& point, unsigned modifiers)
{
    if (d->m_page->defersLoading())
        return;

    if (d->m_fullScreenPluginView.get())
        return;
 
     d->m_lastUserEventTimestamp = currentTime();
 
    d->m_touchEventHandler->handleTouchPoint(point, modifiers);
 }
