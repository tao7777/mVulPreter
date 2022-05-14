 bool CSPSourceList::matches(const KURL& url, ContentSecurityPolicy::RedirectStatus redirectStatus) const
 {
    if (m_allowStar)
         return true;
 
     KURL effectiveURL = m_policy->selfMatchesInnerURL() && SecurityOrigin::shouldUseInnerURL(url) ? SecurityOrigin::extractInnerURL(url) : url;
 
     if (m_allowSelf && m_policy->urlMatchesSelf(effectiveURL))
         return true;
 
    for (size_t i = 0; i < m_list.size(); ++i) {
        if (m_list[i].matches(effectiveURL, redirectStatus))
            return true;
    }
    return false;
 }
