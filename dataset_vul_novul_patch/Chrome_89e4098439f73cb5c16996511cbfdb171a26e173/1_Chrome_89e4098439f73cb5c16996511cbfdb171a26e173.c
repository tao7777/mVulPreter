 void QQuickWebViewFlickablePrivate::onComponentComplete()
 {
    Q_Q(QQuickWebView);
    m_viewportHandler.reset(new QtViewportHandler(webPageProxy.get(), q, pageView.data()));
    pageView->eventHandler()->setViewportHandler(m_viewportHandler.data());
 
     _q_onVisibleChanged();
}
