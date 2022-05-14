bool CCLayerTreeHost::initialize()
{
    TRACE_EVENT("CCLayerTreeHost::initialize", this, 0);
    if (m_settings.enableCompositorThread) {
        m_settings.acceleratePainting = false;
        m_settings.showFPSCounter = false;
        m_settings.showPlatformLayerTree = false;

        m_proxy = CCThreadProxy::create(this);
    } else
        m_proxy = CCSingleThreadProxy::create(this);
    m_proxy->start();

    if (!m_proxy->initializeLayerRenderer())
        return false;

    m_compositorIdentifier = m_proxy->compositorIdentifier();

     m_settings.acceleratePainting = m_proxy->layerRendererCapabilities().usingAcceleratedPainting;
 
     m_contentsTextureManager = TextureManager::create(TextureManager::highLimitBytes(), m_proxy->layerRendererCapabilities().maxTextureSize);
     return true;
 }
