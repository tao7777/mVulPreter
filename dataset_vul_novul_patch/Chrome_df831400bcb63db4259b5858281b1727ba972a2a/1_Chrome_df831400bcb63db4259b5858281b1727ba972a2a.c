void WebPageProxy::gestureDidScroll(const IntSize& size)
 {
    process()->send(Messages::WebPage::GestureDidScroll(size), m_pageID);
 }
