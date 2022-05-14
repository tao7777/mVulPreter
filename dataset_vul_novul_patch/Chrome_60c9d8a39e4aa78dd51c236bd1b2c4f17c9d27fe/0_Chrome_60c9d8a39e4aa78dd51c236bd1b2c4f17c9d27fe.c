bool TextAutosizer::processSubtree(RenderObject* layoutRoot)
{
    if (!m_document->settings() || !m_document->settings()->textAutosizingEnabled() || layoutRoot->view()->printing() || !m_document->page())
        return false;

    Frame* mainFrame = m_document->page()->mainFrame();

    TextAutosizingWindowInfo windowInfo;

     windowInfo.windowSize = m_document->settings()->textAutosizingWindowSizeOverride();
     if (windowInfo.windowSize.isEmpty()) {
         bool includeScrollbars = !InspectorInstrumentation::shouldApplyScreenWidthOverride(mainFrame);
        windowInfo.windowSize = mainFrame->view()->visibleContentRect(includeScrollbars).size();
        if (!m_document->settings()->applyPageScaleFactorInCompositor())
            windowInfo.windowSize.scale(1 / m_document->page()->deviceScaleFactor());
     }
 
    windowInfo.minLayoutSize = mainFrame->view()->layoutSize();
    for (Frame* frame = m_document->frame(); frame; frame = frame->tree()->parent()) {
        if (!frame->view()->isInChildFrameWithFrameFlattening())
            windowInfo.minLayoutSize = windowInfo.minLayoutSize.shrunkTo(frame->view()->layoutSize());
    }

    RenderBlock* container = layoutRoot->isRenderBlock() ? toRenderBlock(layoutRoot) : layoutRoot->containingBlock();
    while (container && !isAutosizingContainer(container))
        container = container->containingBlock();

    RenderBlock* cluster = container;
    while (cluster && (!isAutosizingContainer(cluster) || !isAutosizingCluster(cluster)))
        cluster = cluster->containingBlock();

    processCluster(cluster, container, layoutRoot, windowInfo);
    return true;
}
