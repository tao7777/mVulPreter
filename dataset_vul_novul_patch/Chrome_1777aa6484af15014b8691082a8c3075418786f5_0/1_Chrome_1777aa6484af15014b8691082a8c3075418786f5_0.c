 void LayerTreeHostQt::setRootCompositingLayer(WebCore::GraphicsLayer* graphicsLayer)
 {
     m_nonCompositedContentLayer->removeAllChildren();
 
     if (graphicsLayer)
        m_nonCompositedContentLayer->addChild(graphicsLayer);
}
