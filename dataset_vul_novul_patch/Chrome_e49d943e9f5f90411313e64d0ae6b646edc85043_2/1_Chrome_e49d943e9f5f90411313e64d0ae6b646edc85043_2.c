    void runSetUp(ViewportState viewportState, PreloadState preloadState = PreloadEnabled)
     {
         HTMLParserOptions options(&m_dummyPageHolder->document());
         KURL documentURL(ParsedURLString, "http://whatever.test/");
         m_dummyPageHolder->document().settings()->setViewportEnabled(viewportState == ViewportEnabled);
         m_dummyPageHolder->document().settings()->setViewportMetaEnabled(viewportState == ViewportEnabled);
         m_dummyPageHolder->document().settings()->setDoHtmlPreloadScanning(preloadState == PreloadEnabled);
         m_scanner = HTMLPreloadScanner::create(options, documentURL, CachedDocumentParameters::create(&m_dummyPageHolder->document()), createMediaValuesData());
     }
