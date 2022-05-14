void FrameLoader::startLoad(FrameLoadRequest& frameLoadRequest, FrameLoadType type, NavigationPolicy navigationPolicy)
{
    ASSERT(client()->hasWebView());
    if (m_frame->document()->pageDismissalEventBeingDispatched() != Document::NoDismissal)
        return;

    NavigationType navigationType = determineNavigationType(type, frameLoadRequest.resourceRequest().httpBody() || frameLoadRequest.form(), frameLoadRequest.triggeringEvent());
    frameLoadRequest.resourceRequest().setRequestContext(determineRequestContextFromNavigationType(navigationType));
    frameLoadRequest.resourceRequest().setFrameType(m_frame->isMainFrame() ? WebURLRequest::FrameTypeTopLevel : WebURLRequest::FrameTypeNested);
    ResourceRequest& request = frameLoadRequest.resourceRequest();
    if (!shouldContinueForNavigationPolicy(request, frameLoadRequest.substituteData(), nullptr, frameLoadRequest.shouldCheckMainWorldContentSecurityPolicy(), navigationType, navigationPolicy, type == FrameLoadTypeReplaceCurrentItem, frameLoadRequest.clientRedirect() == ClientRedirectPolicy::ClientRedirect))
        return;
    if (!shouldClose(navigationType == NavigationTypeReload))
         return;
 
     m_frame->document()->cancelParsing();
    if (m_provisionalDocumentLoader) {
        FrameNavigationDisabler navigationDisabler(*m_frame);
        detachDocumentLoader(m_provisionalDocumentLoader);
    }
 
    if (!m_frame->host())
        return;

    m_provisionalDocumentLoader = client()->createDocumentLoader(m_frame, request, frameLoadRequest.substituteData().isValid() ? frameLoadRequest.substituteData() : defaultSubstituteDataForURL(request.url()));
    m_provisionalDocumentLoader->setNavigationType(navigationType);
    m_provisionalDocumentLoader->setReplacesCurrentHistoryItem(type == FrameLoadTypeReplaceCurrentItem);
    m_provisionalDocumentLoader->setIsClientRedirect(frameLoadRequest.clientRedirect() == ClientRedirectPolicy::ClientRedirect);

    InspectorInstrumentation::didStartProvisionalLoad(m_frame);

    m_frame->navigationScheduler().cancel();
    m_checkTimer.stop();

    m_loadType = type;

    if (frameLoadRequest.form())
        client()->dispatchWillSubmitForm(frameLoadRequest.form());

    m_progressTracker->progressStarted();
    if (m_provisionalDocumentLoader->isClientRedirect())
        m_provisionalDocumentLoader->appendRedirect(m_frame->document()->url());
    m_provisionalDocumentLoader->appendRedirect(m_provisionalDocumentLoader->request().url());
    double triggeringEventTime = frameLoadRequest.triggeringEvent() ? frameLoadRequest.triggeringEvent()->platformTimeStamp() : 0;
    client()->dispatchDidStartProvisionalLoad(triggeringEventTime);
    ASSERT(m_provisionalDocumentLoader);
    m_provisionalDocumentLoader->startLoadingMainResource();

    takeObjectSnapshot();
}
