void RenderLayerCompositor::frameViewDidScroll()
{
    FrameView* frameView = m_renderView->frameView();
    IntPoint scrollPosition = frameView->scrollPosition();

    if (!m_scrollLayer)
        return;

    bool scrollingCoordinatorHandlesOffset = false;
    if (ScrollingCoordinator* scrollingCoordinator = this->scrollingCoordinator()) {
        if (Settings* settings = m_renderView->document().settings()) {
            if (isMainFrame() || settings->compositedScrollingForFramesEnabled())
                scrollingCoordinatorHandlesOffset = scrollingCoordinator->scrollableAreaScrollLayerDidChange(frameView);
        }
    }

    if (scrollingCoordinatorHandlesOffset)
        m_scrollLayer->setPosition(-frameView->minimumScrollPosition());
    else
        m_scrollLayer->setPosition(-scrollPosition);


    blink::Platform::current()->histogramEnumeration("Renderer.AcceleratedFixedRootBackground",
        ScrolledMainFrameBucket,
        AcceleratedFixedRootBackgroundHistogramMax);

     if (!m_renderView->rootBackgroundIsEntirelyFixed())
         return;
 
     blink::Platform::current()->histogramEnumeration("Renderer.AcceleratedFixedRootBackground",
         !!fixedRootBackgroundLayer()
             ? ScrolledMainFrameWithAcceleratedFixedRootBackground
            : ScrolledMainFrameWithUnacceleratedFixedRootBackground,
        AcceleratedFixedRootBackgroundHistogramMax);
}
