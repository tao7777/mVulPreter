TokenPreloadScanner::TokenPreloadScanner(const KURL& documentURL, PassOwnPtr<CachedDocumentParameters> documentParameters, const MediaValuesCached::MediaValuesCachedData& mediaValuesCachedData)
    : m_documentURL(documentURL)
    , m_inStyle(false)
    , m_inPicture(false)
    , m_inScript(false)
    , m_isAppCacheEnabled(false)
    , m_isCSPEnabled(false)
    , m_templateCount(0)
    , m_documentParameters(std::move(documentParameters))
    , m_mediaValues(MediaValuesCached::create(mediaValuesCachedData))
    , m_didRewind(false)
 {
     ASSERT(m_documentParameters.get());
     ASSERT(m_mediaValues.get());
 }
