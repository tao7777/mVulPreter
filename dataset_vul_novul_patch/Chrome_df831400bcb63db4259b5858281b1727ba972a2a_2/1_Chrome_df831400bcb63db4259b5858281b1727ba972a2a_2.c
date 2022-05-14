LRESULT WebView::onGesture(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, bool& handled)
 {
     ASSERT(GetGestureInfoPtr());
     ASSERT(CloseGestureInfoHandlePtr());
 
    if (!GetGestureInfoPtr() || !CloseGestureInfoHandlePtr()) {
         handled = false;
         return 0;
     }

    HGESTUREINFO gestureHandle = reinterpret_cast<HGESTUREINFO>(lParam);
    GESTUREINFO gi = {0};
    gi.cbSize = sizeof(GESTUREINFO);

    if (!GetGestureInfoPtr()(gestureHandle, &gi)) {
        handled = false;
        return 0;
    }

    switch (gi.dwID) {
    case GID_BEGIN:
        m_lastPanX = gi.ptsLocation.x;
        m_lastPanY = gi.ptsLocation.y;
        break;
    case GID_END:
        m_page->gestureDidEnd();
        break;
    case GID_PAN: {
        int currentX = gi.ptsLocation.x;
        int currentY = gi.ptsLocation.y;

        int deltaX = m_lastPanX - currentX;
        int deltaY = m_lastPanY - currentY;

         m_lastPanX = currentX;
         m_lastPanY = currentY;
 
        m_page->gestureDidScroll(IntSize(deltaX, deltaY));
 
 
         CloseGestureInfoHandlePtr()(gestureHandle);
 
        handled = true;
        return 0;
    }
    default:
        break;
    }

    handled = false;
    return 0;
}
