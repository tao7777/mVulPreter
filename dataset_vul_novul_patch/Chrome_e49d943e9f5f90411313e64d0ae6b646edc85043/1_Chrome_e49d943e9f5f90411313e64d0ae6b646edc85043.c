CachedDocumentParameters::CachedDocumentParameters(Document* document)
{
    ASSERT(isMainThread());
    ASSERT(document);
    doHtmlPreloadScanning = !document->settings() || document->settings()->doHtmlPreloadScanning();
    doDocumentWritePreloadScanning = doHtmlPreloadScanning && document->frame() && document->frame()->isMainFrame();
     defaultViewportMinWidth = document->viewportDefaultMinWidth();
     viewportMetaZeroValuesQuirk = document->settings() && document->settings()->viewportMetaZeroValuesQuirk();
     viewportMetaEnabled = document->settings() && document->settings()->viewportMetaEnabled();
    referrerPolicy = ReferrerPolicyDefault;
 }
