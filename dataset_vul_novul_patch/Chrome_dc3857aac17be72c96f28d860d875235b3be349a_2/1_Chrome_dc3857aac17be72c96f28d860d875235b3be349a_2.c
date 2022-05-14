void TestController::createWebViewWithOptions(WKDictionaryRef options)
{
    m_mainWebView = adoptPtr(new PlatformWebView(m_context.get(), m_pageGroup.get(), options));
    WKPageUIClient pageUIClient = {
        kWKPageUIClientCurrentVersion,
        m_mainWebView.get(),
        0, // createNewPage_deprecatedForUseWithV0
        0, // showPage
        0, // close
        0, // takeFocus
        focus,
        unfocus,
        0, // runJavaScriptAlert
        0, // runJavaScriptConfirm
        0, // runJavaScriptPrompt
        0, // setStatusText
        0, // mouseDidMoveOverElement_deprecatedForUseWithV0
        0, // missingPluginButtonClicked
        0, // didNotHandleKeyEvent
        0, // didNotHandleWheelEvent
        0, // toolbarsAreVisible
        0, // setToolbarsAreVisible
        0, // menuBarIsVisible
        0, // setMenuBarIsVisible
        0, // statusBarIsVisible
        0, // setStatusBarIsVisible
        0, // isResizable
        0, // setIsResizable
        getWindowFrame,
        setWindowFrame,
        runBeforeUnloadConfirmPanel,
        0, // didDraw
        0, // pageDidScroll
        exceededDatabaseQuota,
        0, // runOpenPanel
        decidePolicyForGeolocationPermissionRequest,
        0, // headerHeight
        0, // footerHeight
        0, // drawHeader
        0, // drawFooter
        0, // printFrame
        runModal,
        0, // didCompleteRubberBandForMainFrame
        0, // saveDataToFileInDownloadsFolder
        0, // shouldInterruptJavaScript
        createOtherPage,
        0, // mouseDidMoveOverElement
        decidePolicyForNotificationPermissionRequest, // decidePolicyForNotificationPermissionRequest
        unavailablePluginButtonClicked,
        0, // showColorPicker
        0, // hideColorPicker
        0, // shouldInstantiatePlugin
    };
    WKPageSetPageUIClient(m_mainWebView->page(), &pageUIClient);

    WKPageLoaderClient pageLoaderClient = {
        kWKPageLoaderClientCurrentVersion,
        this,
        0, // didStartProvisionalLoadForFrame
        0, // didReceiveServerRedirectForProvisionalLoadForFrame
        0, // didFailProvisionalLoadWithErrorForFrame
        didCommitLoadForFrame,
        0, // didFinishDocumentLoadForFrame
        didFinishLoadForFrame,
        0, // didFailLoadWithErrorForFrame
        0, // didSameDocumentNavigationForFrame
        0, // didReceiveTitleForFrame
        0, // didFirstLayoutForFrame
        0, // didFirstVisuallyNonEmptyLayoutForFrame
        0, // didRemoveFrameFromHierarchy
        0, // didFailToInitializePlugin
        0, // didDisplayInsecureContentForFrame
        0, // canAuthenticateAgainstProtectionSpaceInFrame
        didReceiveAuthenticationChallengeInFrame, // didReceiveAuthenticationChallengeInFrame
        0, // didStartProgress
        0, // didChangeProgress
        0, // didFinishProgress
        0, // didBecomeUnresponsive
        0, // didBecomeResponsive
        processDidCrash,
        0, // didChangeBackForwardList
        0, // shouldGoToBackForwardListItem
        0, // didRunInsecureContentForFrame
        0, // didDetectXSSForFrame
        0, // didNewFirstVisuallyNonEmptyLayout
         0, // willGoToBackForwardListItem
         0, // interactionOccurredWhileProcessUnresponsive
         0, // pluginDidFail
         0, // didLayout
     };
     WKPageSetPageLoaderClient(m_mainWebView->page(), &pageLoaderClient);

    WKPagePolicyClient pagePolicyClient = {
        kWKPagePolicyClientCurrentVersion,
        this,
        decidePolicyForNavigationAction,
        0, // decidePolicyForNewWindowAction
        decidePolicyForResponse,
        0, // unableToImplementPolicy
    };
    WKPageSetPagePolicyClient(m_mainWebView->page(), &pagePolicyClient);
}
