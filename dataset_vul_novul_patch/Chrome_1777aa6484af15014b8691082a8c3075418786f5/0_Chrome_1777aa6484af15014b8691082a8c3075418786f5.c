LayerTreeHostQt::LayerTreeHostQt(WebPage* webPage)
    : LayerTreeHost(webPage)
    , m_notifyAfterScheduledLayerFlush(false)
    , m_isValid(true)
#if USE(TILED_BACKING_STORE)
    , m_waitingForUIProcess(false)
    , m_isSuspended(false)
    , m_contentsScale(1)
#endif
    , m_shouldSyncFrame(false)
    , m_shouldSyncRootLayer(true)
    , m_layerFlushTimer(this, &LayerTreeHostQt::layerFlushTimerFired)
    , m_layerFlushSchedulingEnabled(true)
{
    m_rootLayer = GraphicsLayer::create(this);
    WebGraphicsLayer* webRootLayer = toWebGraphicsLayer(m_rootLayer.get());
    webRootLayer->setRootLayer(true);
#ifndef NDEBUG
    m_rootLayer->setName("LayerTreeHostQt root layer");
#endif
    m_rootLayer->setDrawsContent(false);
    m_rootLayer->setSize(m_webPage->size());
    m_layerTreeContext.webLayerID = toWebGraphicsLayer(webRootLayer)->id();

    m_nonCompositedContentLayer = GraphicsLayer::create(this);
#if USE(TILED_BACKING_STORE)
    toWebGraphicsLayer(m_rootLayer.get())->setWebGraphicsLayerClient(this);
#endif
#ifndef NDEBUG
     m_nonCompositedContentLayer->setName("LayerTreeHostQt non-composited content");
 #endif
     m_nonCompositedContentLayer->setDrawsContent(true);
     m_nonCompositedContentLayer->setSize(m_webPage->size());
 
     m_rootLayer->addChild(m_nonCompositedContentLayer.get());

    if (m_webPage->hasPageOverlay())
        createPageOverlayLayer();

    scheduleLayerFlush();
}
