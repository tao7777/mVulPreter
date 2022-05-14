QtBuiltinBundlePage::QtBuiltinBundlePage(QtBuiltinBundle* bundle, WKBundlePageRef page)
    : m_bundle(bundle)
    , m_page(page)
    , m_navigatorQtObject(0)
    , m_navigatorQtObjectEnabled(false)
{
    WKBundlePageLoaderClient loaderClient = {
        kWKBundlePageLoaderClientCurrentVersion,
        this,
        0, // didStartProvisionalLoadForFrame
        0, // didReceiveServerRedirectForProvisionalLoadForFrame
        0, // didFailProvisionalLoadWithErrorForFrame
        0, // didCommitLoadForFrame
        0, // didFinishDocumentLoadForFrame
        0, // didFinishLoadForFrame
        0, // didFailLoadWithErrorForFrame
        0, // didSameDocumentNavigationForFrame
        0, // didReceiveTitleForFrame
        0, // didFirstLayoutForFrame
        0, // didFirstVisuallyNonEmptyLayoutForFrame
        0, // didRemoveFrameFromHierarchy
        0, // didDisplayInsecureContentForFrame
        0, // didRunInsecureContentForFrame
        didClearWindowForFrame,
        0, // didCancelClientRedirectForFrame
        0, // willPerformClientRedirectForFrame
        0, // didHandleOnloadEventsForFrame
        0, // didLayoutForFrame
        0, // didNewFirstVisuallyNonEmptyLayoutForFrame
        0, // didDetectXSSForFrame
        0, // shouldGoToBackForwardListItem
        0, // didCreateGlobalObjectForFrame
        0, // willDisconnectDOMWindowExtensionFromGlobalObject
        0, // didReconnectDOMWindowExtensionToGlobalObject
         0, // willDestroyGlobalObjectForDOMWindowExtension
         0, // didFinishProgress
         0, // shouldForceUniversalAccessFromLocalURL
         0, // didLayout
     };
     WKBundlePageSetPageLoaderClient(m_page, &loaderClient);
}
