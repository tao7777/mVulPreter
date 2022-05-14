WebView::WebView(RECT rect, WebContext* context, WebPageGroup* pageGroup, HWND parentWindow)
    : m_topLevelParentWindow(0)
    , m_toolTipWindow(0)
    , m_lastCursorSet(0)
    , m_webCoreCursor(0)
    , m_overrideCursor(0)
    , m_trackingMouseLeave(false)
    , m_isInWindow(false)
    , m_isVisible(false)
    , m_wasActivatedByMouseEvent(false)
    , m_isBeingDestroyed(false)
    , m_inIMEComposition(0)
    , m_findIndicatorCallback(0)
     , m_findIndicatorCallbackContext(0)
     , m_lastPanX(0)
     , m_lastPanY(0)
    , m_overPanY(0)
 {
     registerWebViewWindowClass();
 
    m_window = ::CreateWindowExW(0, kWebKit2WebViewWindowClassName, 0, WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE,
        rect.top, rect.left, rect.right - rect.left, rect.bottom - rect.top, parentWindow ? parentWindow : HWND_MESSAGE, 0, instanceHandle(), this);
    ASSERT(::IsWindow(m_window));
    ASSERT(m_isVisible == static_cast<bool>(::GetWindowLong(m_window, GWL_STYLE) & WS_VISIBLE));

    m_page = context->createWebPage(this, pageGroup);
    m_page->initializeWebPage();

    CoCreateInstance(CLSID_DragDropHelper, 0, CLSCTX_INPROC_SERVER, IID_IDropTargetHelper, (void**)&m_dropTargetHelper);

    initializeToolTipWindow();

    windowAncestryDidChange();
}
