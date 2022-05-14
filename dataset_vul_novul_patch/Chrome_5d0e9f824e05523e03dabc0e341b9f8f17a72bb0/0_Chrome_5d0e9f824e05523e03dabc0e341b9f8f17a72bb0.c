 bool CSPSourceList::matches(const KURL& url, ContentSecurityPolicy::RedirectStatus redirectStatus) const
 {
    // The CSP spec specifically states that data:, blob:, and filesystem URLs
    // should not be captured by a '*" source
    // (http://www.w3.org/TR/CSP2/#source-list-guid-matching). Thus, in the
    // case of a full wildcard, data:, blob:, and filesystem: URLs are
    // explicitly checked for in the source list before allowing them through.
    if (m_allowStar) {
        if (url.protocolIs("blob") || url.protocolIs("data") || url.protocolIs("filesystem"))
            return hasSourceMatchInList(url, redirectStatus);
         return true;
    }
 
     KURL effectiveURL = m_policy->selfMatchesInnerURL() && SecurityOrigin::shouldUseInnerURL(url) ? SecurityOrigin::extractInnerURL(url) : url;
 
     if (m_allowSelf && m_policy->urlMatchesSelf(effectiveURL))
         return true;
 
    return hasSourceMatchInList(effectiveURL, redirectStatus);
 }
