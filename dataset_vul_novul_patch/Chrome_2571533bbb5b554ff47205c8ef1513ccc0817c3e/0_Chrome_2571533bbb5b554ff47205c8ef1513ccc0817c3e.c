void DocumentThreadableLoader::loadRequest(const ResourceRequest& request, ResourceLoaderOptions resourceLoaderOptions)
{
    const KURL& requestURL = request.url();
    ASSERT(m_sameOriginRequest || requestURL.user().isEmpty());
    ASSERT(m_sameOriginRequest || requestURL.pass().isEmpty());

    if (m_forceDoNotAllowStoredCredentials)
        resourceLoaderOptions.allowCredentials = DoNotAllowStoredCredentials;
    resourceLoaderOptions.securityOrigin = m_securityOrigin;
    if (m_async) {
        if (!m_actualRequest.isNull())
            resourceLoaderOptions.dataBufferingPolicy = BufferData;

        if (m_options.timeoutMilliseconds > 0)
            m_timeoutTimer.startOneShot(m_options.timeoutMilliseconds / 1000.0, BLINK_FROM_HERE);

        FetchRequest newRequest(request, m_options.initiator, resourceLoaderOptions);
        if (m_options.crossOriginRequestPolicy == AllowCrossOriginRequests)
             newRequest.setOriginRestriction(FetchRequest::NoOriginRestriction);
         ASSERT(!resource());
 
        WeakPtr<DocumentThreadableLoader> self(m_weakFactory.createWeakPtr());

         if (request.requestContext() == WebURLRequest::RequestContextVideo || request.requestContext() == WebURLRequest::RequestContextAudio)
             setResource(RawResource::fetchMedia(newRequest, document().fetcher()));
         else if (request.requestContext() == WebURLRequest::RequestContextManifest)
             setResource(RawResource::fetchManifest(newRequest, document().fetcher()));
         else
             setResource(RawResource::fetch(newRequest, document().fetcher()));
 
        // setResource() might call notifyFinished() synchronously, and thus
        // clear() might be called and |this| may be dead here.
        if (!self)
            return;

         if (!resource()) {
             InspectorInstrumentation::documentThreadableLoaderFailedToStartLoadingForClient(m_document, m_client);
             ThreadableLoaderClient* client = m_client;
             clear();
            // setResource() might call notifyFinished() and thus clear()
            // synchronously, and in such cases ThreadableLoaderClient is
            // already notified and |client| is null.
            if (!client)
                return;
             client->didFail(ResourceError(errorDomainBlinkInternal, 0, requestURL.getString(), "Failed to start loading."));
             return;
        }

        if (resource()->loader()) {
            unsigned long identifier = resource()->identifier();
            InspectorInstrumentation::documentThreadableLoaderStartedLoadingForClient(m_document, identifier, m_client);
        } else {
            InspectorInstrumentation::documentThreadableLoaderFailedToStartLoadingForClient(m_document, m_client);
        }
        return;
    }

    FetchRequest fetchRequest(request, m_options.initiator, resourceLoaderOptions);
    if (m_options.crossOriginRequestPolicy == AllowCrossOriginRequests)
        fetchRequest.setOriginRestriction(FetchRequest::NoOriginRestriction);
    Resource* resource = RawResource::fetchSynchronously(fetchRequest, document().fetcher());
    ResourceResponse response = resource ? resource->response() : ResourceResponse();
    unsigned long identifier = resource ? resource->identifier() : std::numeric_limits<unsigned long>::max();
    ResourceError error = resource ? resource->resourceError() : ResourceError();

    InspectorInstrumentation::documentThreadableLoaderStartedLoadingForClient(m_document, identifier, m_client);

    if (!resource) {
        m_client->didFail(error);
        return;
    }

    if (!error.isNull() && !requestURL.isLocalFile() && response.httpStatusCode() <= 0) {
        m_client->didFail(error);
        return;
    }

    if (requestURL != response.url() && !isAllowedRedirect(response.url())) {
        m_client->didFailRedirectCheck();
        return;
    }

    handleResponse(identifier, response, nullptr);

    if (!m_client)
        return;

    SharedBuffer* data = resource->resourceBuffer();
    if (data)
        handleReceivedData(data->data(), data->size());

    if (!m_client)
        return;

    handleSuccessfulFinish(identifier, 0.0);
}
