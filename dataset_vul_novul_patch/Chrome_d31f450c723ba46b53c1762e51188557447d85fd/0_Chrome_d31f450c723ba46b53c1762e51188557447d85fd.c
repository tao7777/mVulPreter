LayerTreeCoordinator::LayerTreeCoordinator(WebPage* webPage)
    : LayerTreeHost(webPage)
    , m_notifyAfterScheduledLayerFlush(false)
    , m_isValid(true)
    , m_waitingForUIProcess(true)
    , m_isSuspended(false)
    , m_contentsScale(1)
    , m_shouldSendScrollPositionUpdate(true)
     , m_shouldSyncFrame(false)
     , m_shouldSyncRootLayer(true)
     , m_layerFlushTimer(this, &LayerTreeCoordinator::layerFlushTimerFired)
    , m_releaseInactiveAtlasesTimer(this, &LayerTreeCoordinator::releaseInactiveAtlasesTimerFired)
     , m_layerFlushSchedulingEnabled(true)
     , m_forceRepaintAsyncCallbackID(0)
 {
    m_rootLayer = GraphicsLayer::create(this);
    CoordinatedGraphicsLayer* webRootLayer = toCoordinatedGraphicsLayer(m_rootLayer.get());
    webRootLayer->setRootLayer(true);
#ifndef NDEBUG
    m_rootLayer->setName("LayerTreeCoordinator root layer");
#endif
    m_rootLayer->setDrawsContent(false);
    m_rootLayer->setSize(m_webPage->size());
    m_layerTreeContext.webLayerID = toCoordinatedGraphicsLayer(webRootLayer)->id();

    m_nonCompositedContentLayer = GraphicsLayer::create(this);
    toCoordinatedGraphicsLayer(m_rootLayer.get())->setCoordinatedGraphicsLayerClient(this);
#ifndef NDEBUG
    m_nonCompositedContentLayer->setName("LayerTreeCoordinator non-composited content");
#endif
    m_nonCompositedContentLayer->setDrawsContent(true);
    m_nonCompositedContentLayer->setSize(m_webPage->size());

    m_rootLayer->addChild(m_nonCompositedContentLayer.get());

    if (m_webPage->hasPageOverlay())
        createPageOverlayLayer();

    scheduleLayerFlush();
}
