bool CSPSource::schemeMatches(const KURL& url) const
 {
     if (m_scheme.isEmpty())
         return m_policy->protocolMatchesSelf(url);
     return equalIgnoringCase(url.protocol(), m_scheme);
 }
