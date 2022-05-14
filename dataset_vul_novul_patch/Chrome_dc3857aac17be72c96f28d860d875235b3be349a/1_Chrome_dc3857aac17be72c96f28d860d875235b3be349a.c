void attachLoaderClientToView(WebKitWebView* webView)
{
    WKPageLoaderClient wkLoaderClient = {
        kWKPageLoaderClientCurrentVersion,
        webView, // clientInfo
        didStartProvisionalLoadForFrame,
        didReceiveServerRedirectForProvisionalLoadForFrame,
        didFailProvisionalLoadWithErrorForFrame,
        didCommitLoadForFrame,
        0, // didFinishDocumentLoadForFrame
        didFinishLoadForFrame,
        didFailLoadWithErrorForFrame,
        didSameDocumentNavigationForFrame,
        didReceiveTitleForFrame,
        0, // didFirstLayoutForFrame
        0, // didFirstVisuallyNonEmptyLayoutForFrame
        0, // didRemoveFrameFromHierarchy
        didDisplayInsecureContentForFrame,
        didRunInsecureContentForFrame,
        0, // canAuthenticateAgainstProtectionSpaceInFrame
        didReceiveAuthenticationChallengeInFrame,
        didChangeProgress, // didStartProgress
        didChangeProgress,
        didChangeProgress, // didFinishProgress
        0, // didBecomeUnresponsive
        0, // didBecomeResponsive
        0, // processDidCrash
        didChangeBackForwardList,
        0, // shouldGoToBackForwardListItem
        0, // didFailToInitializePlugin
        0, // didDetectXSSForFrame
        0, // didFirstVisuallyNonEmptyLayoutForFrame
         0, // willGoToBackForwardListItem
         0, // interactionOccurredWhileProcessUnresponsive
         0, // pluginDidFail
         0, // didLayout
     };
     WKPageRef wkPage = toAPI(webkitWebViewBaseGetPage(WEBKIT_WEB_VIEW_BASE(webView)));
    WKPageSetPageLoaderClient(wkPage, &wkLoaderClient);
}
