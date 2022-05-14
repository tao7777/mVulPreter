void WebPageProxy::gestureDidScroll(const IntSize& size)
bool WebPageProxy::gestureDidScroll(const IntSize& size)
 {
    bool atBeginningOrEndOfScrollableDocument = false;
    process()->sendSync(Messages::WebPage::GestureDidScroll(size), Messages::WebPage::GestureDidScroll::Reply(atBeginningOrEndOfScrollableDocument), m_pageID);
    return atBeginningOrEndOfScrollableDocument;
 }
