void InspectorOverlay::update()
{
    if (isEmpty()) {
        m_client->hideHighlight();
        return;
    }

    FrameView* view = m_page->mainFrame()->view();
    if (!view)
        return;
    IntRect viewRect = view->visibleContentRect();
    FrameView* overlayView = overlayPage()->mainFrame()->view();

    IntSize frameViewFullSize = view->visibleContentRect(ScrollableArea::IncludeScrollbars).size();
    IntSize size = m_size.isEmpty() ? frameViewFullSize : m_size;
    size.scale(m_page->pageScaleFactor());
    overlayView->resize(size);

    reset(size, m_size.isEmpty() ? IntSize() : frameViewFullSize, viewRect.x(), viewRect.y());

    drawGutter();
    drawNodeHighlight();
    drawQuadHighlight();
     if (!m_inspectModeEnabled)
         drawPausedInDebuggerMessage();
     drawViewSize();
    drawOverridesMessage();
 
     overlayPage()->mainFrame()->document()->recalcStyle(Force);
    if (overlayView->needsLayout())
        overlayView->layout();

    m_client->highlight();
}
