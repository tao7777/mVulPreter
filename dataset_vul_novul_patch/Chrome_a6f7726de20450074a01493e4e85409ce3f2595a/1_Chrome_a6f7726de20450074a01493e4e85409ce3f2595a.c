void Document::processHttpEquiv(const String& equiv, const String& content)
{
    ASSERT(!equiv.isNull() && !content.isNull());

    Frame* frame = this->frame();

    if (equalIgnoringCase(equiv, "default-style")) {
        m_styleSheetCollection->setSelectedStylesheetSetName(content);
        m_styleSheetCollection->setPreferredStylesheetSetName(content);
        styleResolverChanged(DeferRecalcStyle);
    } else if (equalIgnoringCase(equiv, "refresh")) {
        double delay;
        String url;
        if (frame && parseHTTPRefresh(content, true, delay, url)) {
            if (url.isEmpty())
                url = m_url.string();
            else
                url = completeURL(url).string();
            frame->navigationScheduler()->scheduleRedirect(delay, url);
        }
    } else if (equalIgnoringCase(equiv, "set-cookie")) {
        if (isHTMLDocument()) {
            toHTMLDocument(this)->setCookie(content, IGNORE_EXCEPTION);
        }
    } else if (equalIgnoringCase(equiv, "content-language"))
        setContentLanguage(content);
    else if (equalIgnoringCase(equiv, "x-dns-prefetch-control"))
        parseDNSPrefetchControlHeader(content);
    else if (equalIgnoringCase(equiv, "x-frame-options")) {
        if (frame) {
            FrameLoader* frameLoader = frame->loader();
            unsigned long requestIdentifier = 0;
            if (frameLoader->activeDocumentLoader() && frameLoader->activeDocumentLoader()->mainResourceLoader())
                requestIdentifier = frameLoader->activeDocumentLoader()->mainResourceLoader()->identifier();
             if (frameLoader->shouldInterruptLoadForXFrameOptions(content, url(), requestIdentifier)) {
                 String message = "Refused to display '" + url().elidedString() + "' in a frame because it set 'X-Frame-Options' to '" + content + "'.";
                 frameLoader->stopAllLoaders();
                frame->navigationScheduler()->scheduleLocationChange(securityOrigin(), "data:text/html,<p></p>", String());
                 addConsoleMessage(SecurityMessageSource, ErrorMessageLevel, message, requestIdentifier);
             }
         }
    } else if (equalIgnoringCase(equiv, "content-security-policy"))
        contentSecurityPolicy()->didReceiveHeader(content, ContentSecurityPolicy::Enforce);
    else if (equalIgnoringCase(equiv, "content-security-policy-report-only"))
        contentSecurityPolicy()->didReceiveHeader(content, ContentSecurityPolicy::Report);
    else if (equalIgnoringCase(equiv, "x-webkit-csp"))
        contentSecurityPolicy()->didReceiveHeader(content, ContentSecurityPolicy::PrefixedEnforce);
    else if (equalIgnoringCase(equiv, "x-webkit-csp-report-only"))
        contentSecurityPolicy()->didReceiveHeader(content, ContentSecurityPolicy::PrefixedReport);
}
