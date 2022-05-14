void WebPage::gestureDidScroll(const WebCore::IntSize& size)
void WebPage::gestureDidScroll(const IntSize& size, bool& atBeginningOrEndOfScrollableDocument)
 {
    atBeginningOrEndOfScrollableDocument = false;

     if (!m_gestureTargetNode || !m_gestureTargetNode->renderer() || !m_gestureTargetNode->renderer()->enclosingLayer())
         return;
 
     m_gestureTargetNode->renderer()->enclosingLayer()->scrollByRecursively(size.width(), size.height());

    Frame* frame = m_page->mainFrame();
    if (!frame)
        return;

    ScrollView* view = frame->view();
    if (!view)
        return;

    Scrollbar* verticalScrollbar = view->verticalScrollbar();
    if (!verticalScrollbar)
        return;

    atBeginningOrEndOfScrollableDocument = !verticalScrollbar->currentPos() || verticalScrollbar->currentPos() >= verticalScrollbar->maximum();
 }
