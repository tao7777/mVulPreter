 CachedCSSStyleSheet* Cache::requestUserCSSStyleSheet(DocLoader* docLoader, const String& url, const String& charset)
 {
     CachedCSSStyleSheet* userSheet = new CachedCSSStyleSheet(url, charset);
 
    userSheet->setInCache(true);
    userSheet->load(docLoader, false, true, false);
    if (!disabled())
        m_resources.set(url, userSheet);
    else
        userSheet->setInCache(false);

    return userSheet;
}
