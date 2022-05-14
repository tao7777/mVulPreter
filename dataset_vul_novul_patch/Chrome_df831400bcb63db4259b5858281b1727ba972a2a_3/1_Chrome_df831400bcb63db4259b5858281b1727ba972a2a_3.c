void WebPage::gestureDidScroll(const WebCore::IntSize& size)
 {
     if (!m_gestureTargetNode || !m_gestureTargetNode->renderer() || !m_gestureTargetNode->renderer()->enclosingLayer())
         return;
 
     m_gestureTargetNode->renderer()->enclosingLayer()->scrollByRecursively(size.width(), size.height());
 }
