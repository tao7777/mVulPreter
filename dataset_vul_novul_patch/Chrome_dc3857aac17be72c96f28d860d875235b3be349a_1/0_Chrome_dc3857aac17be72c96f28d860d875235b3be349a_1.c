InjectedBundlePage::InjectedBundlePage(WKBundlePageRef page)
    : m_page(page)
    , m_world(AdoptWK, WKBundleScriptWorldCreateWorld())
{
    WKBundlePageLoaderClient loaderClient = {
        kWKBundlePageLoaderClientCurrentVersion,
        this,
        didStartProvisionalLoadForFrame,
        didReceiveServerRedirectForProvisionalLoadForFrame,
        didFailProvisionalLoadWithErrorForFrame,
        didCommitLoadForFrame,
        didFinishDocumentLoadForFrame,
        didFinishLoadForFrame,
        didFailLoadWithErrorForFrame,
        didSameDocumentNavigationForFrame,
        didReceiveTitleForFrame,
        0, // didFirstLayoutForFrame
        0, // didFirstVisuallyNonEmptyLayoutForFrame
        0, // didRemoveFrameFromHierarchy
        didDisplayInsecureContentForFrame,
        didRunInsecureContentForFrame,
        didClearWindowForFrame,
        didCancelClientRedirectForFrame,
        willPerformClientRedirectForFrame,
        didHandleOnloadEventsForFrame,
        0, // didLayoutForFrame
        0, // didNewFirstVisuallyNonEmptyLayoutForFrame
        didDetectXSSForFrame,
        0, // shouldGoToBackForwardListItem
        0, // didCreateGlobalObjectForFrame
        0, // willDisconnectDOMWindowExtensionFromGlobalObject
        0, // didReconnectDOMWindowExtensionToGlobalObject
         0, // willDestroyGlobalObjectForDOMWindowExtension
         didFinishProgress, // didFinishProgress
         0, // shouldForceUniversalAccessFromLocalURL
        0, // didReceiveIntentForFrame
        0, // registerIntentServiceForFrame
         0, // didLayout
     };
     WKBundlePageSetPageLoaderClient(m_page, &loaderClient);

    WKBundlePageResourceLoadClient resourceLoadClient = {
        kWKBundlePageResourceLoadClientCurrentVersion,
        this,
        didInitiateLoadForResource,
        willSendRequestForFrame,
        didReceiveResponseForResource,
        didReceiveContentLengthForResource,
        didFinishLoadForResource,
        didFailLoadForResource,
        shouldCacheResponse,
        0 // shouldUseCredentialStorage
    };
    WKBundlePageSetResourceLoadClient(m_page, &resourceLoadClient);

    WKBundlePagePolicyClient policyClient = {
        kWKBundlePagePolicyClientCurrentVersion,
        this,
        decidePolicyForNavigationAction,
        decidePolicyForNewWindowAction,
        decidePolicyForResponse,
        unableToImplementPolicy
    };
    WKBundlePageSetPolicyClient(m_page, &policyClient);

    WKBundlePageUIClient uiClient = {
        kWKBundlePageUIClientCurrentVersion,
        this,
        willAddMessageToConsole,
        willSetStatusbarText,
        willRunJavaScriptAlert,
        willRunJavaScriptConfirm,
        willRunJavaScriptPrompt,
        0, /*mouseDidMoveOverElement*/
        0, /*pageDidScroll*/
        0, /*paintCustomOverhangArea*/
        0, /*shouldGenerateFileForUpload*/
        0, /*generateFileForUpload*/
        0, /*shouldRubberBandInDirection*/
        0, /*statusBarIsVisible*/
        0, /*menuBarIsVisible*/
        0, /*toolbarsAreVisible*/
        didReachApplicationCacheOriginQuota,
        didExceedDatabaseQuota,
        0, /*plugInStartLabelTitle*/
        0, /*plugInStartLabelSubtitle*/
        0, /*plugInExtraStyleSheet*/
    };
    WKBundlePageSetUIClient(m_page, &uiClient);

    WKBundlePageEditorClient editorClient = {
        kWKBundlePageEditorClientCurrentVersion,
        this,
        shouldBeginEditing,
        shouldEndEditing,
        shouldInsertNode,
        shouldInsertText,
        shouldDeleteRange,
        shouldChangeSelectedRange,
        shouldApplyStyle,
        didBeginEditing,
        didEndEditing,
        didChange,
        didChangeSelection,
        0, /* willWriteToPasteboard */
        0, /* getPasteboardDataForRange */
        0  /* didWriteToPasteboard */
    };
    WKBundlePageSetEditorClient(m_page, &editorClient);

#if ENABLE(FULLSCREEN_API)
    WKBundlePageFullScreenClient fullScreenClient = {
        kWKBundlePageFullScreenClientCurrentVersion,
        this,
        supportsFullScreen,
        enterFullScreenForElement,
        exitFullScreenForElement,
        beganEnterFullScreen,
        beganExitFullScreen,
        closeFullScreen,
    };
    WKBundlePageSetFullScreenClient(m_page, &fullScreenClient);
#endif
}
