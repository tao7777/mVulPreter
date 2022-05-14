 void CCLayerTreeHostTest::doBeginTest()
 {
     ASSERT(isMainThread());
    ASSERT(!m_running);
    m_running = true;
     m_client = MockLayerTreeHostClient::create(this);
 
     RefPtr<LayerChromium> rootLayer = LayerChromium::create(0);
    m_layerTreeHost = MockLayerTreeHost::create(this, m_client.get(), rootLayer, m_settings);
    ASSERT(m_layerTreeHost);

    m_beginning = true;
    beginTest();
    m_beginning = false;
    if (m_endWhenBeginReturns)
        onEndTest(static_cast<void*>(this));
}
