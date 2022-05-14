void DocumentLoader::responseReceived(CachedResource* resource, const ResourceResponse& response)
{
    ASSERT_UNUSED(resource, m_mainResource == resource);
    RefPtr<DocumentLoader> protect(this);
    bool willLoadFallback = m_applicationCacheHost->maybeLoadFallbackForMainResponse(request(), response);

    bool shouldRemoveResourceFromCache = willLoadFallback;
#if PLATFORM(CHROMIUM)
    if (response.appCacheID())
        shouldRemoveResourceFromCache = true;
#endif
    if (shouldRemoveResourceFromCache)
        memoryCache()->remove(m_mainResource.get());

    if (willLoadFallback)
        return;

    DEFINE_STATIC_LOCAL(AtomicString, xFrameOptionHeader, ("x-frame-options", AtomicString::ConstructFromLiteral));
    HTTPHeaderMap::const_iterator it = response.httpHeaderFields().find(xFrameOptionHeader);
    if (it != response.httpHeaderFields().end()) {
        String content = it->value;
        ASSERT(m_mainResource);
        unsigned long identifier = m_identifierForLoadWithoutResourceLoader ? m_identifierForLoadWithoutResourceLoader : m_mainResource->identifier();
        ASSERT(identifier);
        if (frameLoader()->shouldInterruptLoadForXFrameOptions(content, response.url(), identifier)) {
             InspectorInstrumentation::continueAfterXFrameOptionsDenied(m_frame, this, identifier, response);
             String message = "Refused to display '" + response.url().elidedString() + "' in a frame because it set 'X-Frame-Options' to '" + content + "'.";
             frame()->document()->addConsoleMessage(SecurityMessageSource, ErrorMessageLevel, message, identifier);
            frame()->document()->enforceSandboxFlags(SandboxOrigin);
             if (HTMLFrameOwnerElement* ownerElement = frame()->ownerElement())
                 ownerElement->dispatchEvent(Event::create(eventNames().loadEvent, false, false));
             cancelMainResourceLoad(frameLoader()->cancelledError(m_request));
            return;
        }
    }

#if !USE(CF)
    ASSERT(!mainResourceLoader() || !mainResourceLoader()->defersLoading());
#endif

    if (m_isLoadingMultipartContent) {
        setupForReplace();
        m_mainResource->clear();
    } else if (response.isMultipart()) {
        FeatureObserver::observe(m_frame->document(), FeatureObserver::MultipartMainResource);
        m_isLoadingMultipartContent = true;
    }

    m_response = response;

    if (m_identifierForLoadWithoutResourceLoader)
        frameLoader()->notifier()->dispatchDidReceiveResponse(this, m_identifierForLoadWithoutResourceLoader, m_response, 0);

    ASSERT(!m_waitingForContentPolicy);
    m_waitingForContentPolicy = true;

    if (m_substituteData.isValid()) {
        continueAfterContentPolicy(PolicyUse);
        return;
    }

#if ENABLE(FTPDIR)
    Settings* settings = m_frame->settings();
    if (settings && settings->forceFTPDirectoryListings() && m_response.mimeType() == "application/x-ftp-directory") {
        continueAfterContentPolicy(PolicyUse);
        return;
    }
#endif

#if USE(CONTENT_FILTERING)
    if (response.url().protocolIs("https") && ContentFilter::isEnabled())
        m_contentFilter = ContentFilter::create(response);
#endif

    frameLoader()->policyChecker()->checkContentPolicy(m_response, callContinueAfterContentPolicy, this);
}
