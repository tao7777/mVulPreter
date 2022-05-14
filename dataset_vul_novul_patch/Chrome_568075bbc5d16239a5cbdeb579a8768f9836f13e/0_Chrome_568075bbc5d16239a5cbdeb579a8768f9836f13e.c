bool CSPSource::schemeMatches(const KURL& url) const
 {
     if (m_scheme.isEmpty())
         return m_policy->protocolMatchesSelf(url);
    if (equalIgnoringCase(m_scheme, "http"))
        return equalIgnoringCase(url.protocol(), "http") || equalIgnoringCase(url.protocol(), "https");
    if (equalIgnoringCase(m_scheme, "ws"))
        return equalIgnoringCase(url.protocol(), "ws") || equalIgnoringCase(url.protocol(), "wss");
     return equalIgnoringCase(url.protocol(), m_scheme);
 }
