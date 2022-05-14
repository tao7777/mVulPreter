 CachedCSSStyleSheet* Cache::requestUserCSSStyleSheet(DocLoader* docLoader, const String& url, const String& charset)
 {
    if (CachedResource* existing = m_resources.get(url))
        return existing->type() == CachedResource::CSSStyleSheet ? static_cast<CachedCSSStyleSheet*>(existing) : 0;

     CachedCSSStyleSheet* userSheet = new CachedCSSStyleSheet(url, charset);
 
    userSheet->setInCache(true);
    userSheet->load(docLoader, false, true, false);
    if (!disabled())
        m_resources.set(url, userSheet);
    else
        userSheet->setInCache(false);

    return userSheet;
}
