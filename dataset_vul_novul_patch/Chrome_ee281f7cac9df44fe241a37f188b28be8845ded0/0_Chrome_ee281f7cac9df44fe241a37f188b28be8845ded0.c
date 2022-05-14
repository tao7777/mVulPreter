bool ResourceFetcher::canRequest(Resource::Type type, const KURL& url, const ResourceLoaderOptions& options, bool forPreload, FetchRequest::OriginRestriction originRestriction) const
{
    SecurityOrigin* securityOrigin = options.securityOrigin.get();
    if (!securityOrigin && document())
        securityOrigin = document()->securityOrigin();

    if (securityOrigin && !securityOrigin->canDisplay(url)) {
        if (!forPreload)
            context().reportLocalLoadFailed(url);
        WTF_LOG(ResourceLoading, "ResourceFetcher::requestResource URL was not allowed by SecurityOrigin::canDisplay");
        return 0;
    }

    bool shouldBypassMainWorldContentSecurityPolicy = (frame() && frame()->script().shouldBypassMainWorldContentSecurityPolicy()) || (options.contentSecurityPolicyOption == DoNotCheckContentSecurityPolicy);

    switch (type) {
    case Resource::MainResource:
    case Resource::Image:
    case Resource::CSSStyleSheet:
    case Resource::Script:
    case Resource::Font:
    case Resource::Raw:
    case Resource::LinkPrefetch:
    case Resource::LinkSubresource:
    case Resource::TextTrack:
    case Resource::ImportResource:
    case Resource::Media:
        if (originRestriction == FetchRequest::RestrictToSameOrigin && !securityOrigin->canRequest(url)) {
            printAccessDeniedMessage(url);
            return false;
        }
        break;
    case Resource::XSLStyleSheet:
        ASSERT(RuntimeEnabledFeatures::xsltEnabled());
    case Resource::SVGDocument:
        if (!securityOrigin->canRequest(url)) {
            printAccessDeniedMessage(url);
            return false;
        }
        break;
    }

    ContentSecurityPolicy::ReportingStatus cspReporting = forPreload ?
        ContentSecurityPolicy::SuppressReport : ContentSecurityPolicy::SendReport;

    switch (type) {
    case Resource::XSLStyleSheet:
        ASSERT(RuntimeEnabledFeatures::xsltEnabled());
        if (!shouldBypassMainWorldContentSecurityPolicy && !m_document->contentSecurityPolicy()->allowScriptFromSource(url, cspReporting))
            return false;
        break;
    case Resource::Script:
    case Resource::ImportResource:
        if (!shouldBypassMainWorldContentSecurityPolicy && !m_document->contentSecurityPolicy()->allowScriptFromSource(url, cspReporting))
            return false;

        if (frame()) {
            Settings* settings = frame()->settings();
            if (!frame()->loader().client()->allowScriptFromSource(!settings || settings->scriptEnabled(), url)) {
                frame()->loader().client()->didNotAllowScript();
                return false;
            }
        }
        break;
    case Resource::CSSStyleSheet:
        if (!shouldBypassMainWorldContentSecurityPolicy && !m_document->contentSecurityPolicy()->allowStyleFromSource(url, cspReporting))
            return false;
        break;
    case Resource::SVGDocument:
    case Resource::Image:
        if (!shouldBypassMainWorldContentSecurityPolicy && !m_document->contentSecurityPolicy()->allowImageFromSource(url, cspReporting))
            return false;
        break;
    case Resource::Font: {
        if (!shouldBypassMainWorldContentSecurityPolicy && !m_document->contentSecurityPolicy()->allowFontFromSource(url, cspReporting))
            return false;
        break;
    }
    case Resource::MainResource:
    case Resource::Raw:
    case Resource::LinkPrefetch:
    case Resource::LinkSubresource:
        break;
    case Resource::Media:
    case Resource::TextTrack:
        if (!shouldBypassMainWorldContentSecurityPolicy && !m_document->contentSecurityPolicy()->allowMediaFromSource(url, cspReporting))
            return false;
         break;
     }
 
    // SVG Images have unique security rules that prevent all subresource requests
    // except for data urls.
    if (type != Resource::MainResource) {
        if (frame() && frame()->chromeClient().isSVGImageChromeClient() && !url.protocolIsData())
            return false;
    }


    if (!checkInsecureContent(type, url, options.mixedContentBlockingTreatment))
        return false;

    return true;
}
