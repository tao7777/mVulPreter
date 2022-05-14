InRegionScrollableArea::InRegionScrollableArea(WebPagePrivate* webPage, RenderLayer* layer)
    : m_webPage(webPage)
    , m_layer(layer)
{
    ASSERT(webPage);
    ASSERT(layer);
    m_isNull = false;


    RenderObject* layerRenderer = layer->renderer();
    ASSERT(layerRenderer);

    if (layerRenderer->isRenderView()) { // #document case

        FrameView* view = toRenderView(layerRenderer)->frameView();
        ASSERT(view);

        Frame* frame = view->frame();
        ASSERT_UNUSED(frame, frame);

        m_scrollPosition = m_webPage->mapToTransformed(view->scrollPosition());
        m_contentsSize = m_webPage->mapToTransformed(view->contentsSize());
        m_viewportSize = m_webPage->mapToTransformed(view->visibleContentRect(false /*includeScrollbars*/)).size();

        m_visibleWindowRect = m_webPage->mapToTransformed(m_webPage->getRecursiveVisibleWindowRect(view));
        IntRect transformedWindowRect = IntRect(IntPoint::zero(), m_webPage->transformedViewportSize());
        m_visibleWindowRect.intersect(transformedWindowRect);

         m_scrollsHorizontally = view->contentsWidth() > view->visibleWidth();
         m_scrollsVertically = view->contentsHeight() > view->visibleHeight();
 
        m_minimumScrollPosition = m_webPage->mapToTransformed(calculateMinimumScrollPosition(
            view->visibleContentRect().size(),
            0.0 /*overscrollLimit*/));
        m_maximumScrollPosition = m_webPage->mapToTransformed(calculateMaximumScrollPosition(
            view->visibleContentRect().size(),
            view->contentsSize(),
            0.0 /*overscrollLimit*/));
     } else { // RenderBox-based elements case (scrollable boxes (div's, p's, textarea's, etc)).
 
         RenderBox* box = m_layer->renderBox();
        ASSERT(box);
        ASSERT(box->canBeScrolledAndHasScrollableArea());

        ScrollableArea* scrollableArea = static_cast<ScrollableArea*>(m_layer);
        m_scrollPosition = m_webPage->mapToTransformed(scrollableArea->scrollPosition());
        m_contentsSize = m_webPage->mapToTransformed(scrollableArea->contentsSize());
        m_viewportSize = m_webPage->mapToTransformed(scrollableArea->visibleContentRect(false /*includeScrollbars*/)).size();

        m_visibleWindowRect = m_layer->renderer()->absoluteClippedOverflowRect();
        m_visibleWindowRect = m_layer->renderer()->frame()->view()->contentsToWindow(m_visibleWindowRect);
        IntRect visibleFrameWindowRect = m_webPage->getRecursiveVisibleWindowRect(m_layer->renderer()->frame()->view());
        m_visibleWindowRect.intersect(visibleFrameWindowRect);
        m_visibleWindowRect = m_webPage->mapToTransformed(m_visibleWindowRect);
        IntRect transformedWindowRect = IntRect(IntPoint::zero(), m_webPage->transformedViewportSize());
        m_visibleWindowRect.intersect(transformedWindowRect);

         m_scrollsHorizontally = box->scrollWidth() != box->clientWidth() && box->scrollsOverflowX();
         m_scrollsVertically = box->scrollHeight() != box->clientHeight() && box->scrollsOverflowY();
 
        m_minimumScrollPosition = m_webPage->mapToTransformed(calculateMinimumScrollPosition(
            Platform::IntSize(box->clientWidth(), box->clientHeight()),
            0.0 /*overscrollLimit*/));
        m_maximumScrollPosition = m_webPage->mapToTransformed(calculateMaximumScrollPosition(
            Platform::IntSize(box->clientWidth(), box->clientHeight()),
            Platform::IntSize(box->scrollWidth(), box->scrollHeight()),
            0.0 /*overscrollLimit*/));
     }
 }
