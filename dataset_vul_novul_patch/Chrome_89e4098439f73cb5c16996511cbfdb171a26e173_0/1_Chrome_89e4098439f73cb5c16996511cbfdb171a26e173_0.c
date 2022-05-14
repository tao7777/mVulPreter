PlatformWebView::PlatformWebView(WKContextRef contextRef, WKPageGroupRef pageGroupRef)
    : m_view(new QQuickWebView(contextRef, pageGroupRef))
    , m_window(new WrapperWindow(m_view))
    , m_windowIsKey(true)
    , m_modalEventLoop(0)
{
     QQuickWebViewExperimental experimental(m_view);
     experimental.setRenderToOffscreenBuffer(true);
     m_view->setAllowAnyHTTPSCertificateForLocalHost(true);
 }
