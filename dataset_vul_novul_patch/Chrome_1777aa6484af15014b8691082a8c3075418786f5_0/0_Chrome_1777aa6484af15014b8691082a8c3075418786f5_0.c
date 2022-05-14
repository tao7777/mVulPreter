 void LayerTreeHostQt::setRootCompositingLayer(WebCore::GraphicsLayer* graphicsLayer)
 {
     m_nonCompositedContentLayer->removeAllChildren();
    m_nonCompositedContentLayer->setContentsOpaque(m_webPage->drawsBackground() && !m_webPage->drawsTransparentBackground());
 
     if (graphicsLayer)
        m_nonCompositedContentLayer->addChild(graphicsLayer);
}
