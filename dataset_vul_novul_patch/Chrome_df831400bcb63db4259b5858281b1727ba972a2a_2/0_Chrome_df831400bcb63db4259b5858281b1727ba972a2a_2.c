LRESULT WebView::onGesture(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, bool& handled)
 {
     ASSERT(GetGestureInfoPtr());
     ASSERT(CloseGestureInfoHandlePtr());
    ASSERT(UpdatePanningFeedbackPtr());
    ASSERT(BeginPanningFeedbackPtr());
    ASSERT(EndPanningFeedbackPtr());
 
    if (!GetGestureInfoPtr() || !CloseGestureInfoHandlePtr() || !UpdatePanningFeedbackPtr() || !BeginPanningFeedbackPtr() || !EndPanningFeedbackPtr()) {
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
 
        // Calculate the overpan for window bounce.
        m_overPanY -= deltaY;

        bool shouldBounceWindow = m_page->gestureDidScroll(IntSize(deltaX, deltaY));

        if (gi.dwFlags & GF_BEGIN) {
            BeginPanningFeedbackPtr()(m_window);
            m_overPanY = 0;
        } else if (gi.dwFlags & GF_END) {
            EndPanningFeedbackPtr()(m_window, true);
            m_overPanY = 0;
        }

        // FIXME: Support horizontal window bounce - <http://webkit.org/b/58068>.
        // FIXME: Window Bounce doesn't undo until user releases their finger - <http://webkit.org/b/58069>.
 
        if (shouldBounceWindow)
            UpdatePanningFeedbackPtr()(m_window, 0, m_overPanY, gi.dwFlags & GF_INERTIA);
 
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
