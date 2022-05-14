FrameView::FrameView(LocalFrame* frame)
    : m_frame(frame)
    , m_canHaveScrollbars(true)
    , m_slowRepaintObjectCount(0)
    , m_hasPendingLayout(false)
    , m_layoutSubtreeRoot(0)
    , m_inSynchronousPostLayout(false)
    , m_postLayoutTasksTimer(this, &FrameView::postLayoutTimerFired)
    , m_updateWidgetsTimer(this, &FrameView::updateWidgetsTimerFired)
    , m_isTransparent(false)
    , m_baseBackgroundColor(Color::white)
    , m_mediaType("screen")
    , m_overflowStatusDirty(true)
    , m_viewportRenderer(0)
    , m_wasScrolledByUser(false)
    , m_inProgrammaticScroll(false)
    , m_safeToPropagateScrollToParent(true)
    , m_isTrackingPaintInvalidations(false)
    , m_scrollCorner(nullptr)
    , m_hasSoftwareFilters(false)
    , m_visibleContentScaleFactor(1)
     , m_inputEventsScaleFactorForEmulation(1)
     , m_layoutSizeFixedToFrameSize(true)
     , m_didScrollTimer(this, &FrameView::didScrollTimerFired)
 {
     ASSERT(m_frame);
     init();

    if (!m_frame->isMainFrame())
        return;

    ScrollableArea::setVerticalScrollElasticity(ScrollElasticityAllowed);
    ScrollableArea::setHorizontalScrollElasticity(ScrollElasticityAllowed);
}
