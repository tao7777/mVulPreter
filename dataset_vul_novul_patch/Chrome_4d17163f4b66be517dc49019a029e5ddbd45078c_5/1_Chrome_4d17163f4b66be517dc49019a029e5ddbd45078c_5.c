StyleResolver::StyleResolver(Document& document)
    : m_document(document)
    , m_fontSelector(CSSFontSelector::create(&document))
    , m_viewportStyleResolver(ViewportStyleResolver::create(&document))
    , m_styleResourceLoader(document.fetcher())
     , m_styleResolverStatsSequence(0)
     , m_accessCount(0)
 {
    Element* root = document.documentElement();
     m_fontSelector->registerForInvalidationCallbacks(this);
 
    CSSDefaultStyleSheets::initDefaultStyle(root);
 
     FrameView* view = document.view();
     if (view)
         m_medium = adoptPtr(new MediaQueryEvaluator(view->mediaType()));
     else
         m_medium = adoptPtr(new MediaQueryEvaluator("all"));
 
     if (root)
         m_rootDefaultStyle = styleForElement(root, 0, DisallowStyleSharing, MatchOnlyUserAgentRules);
 
    if (m_rootDefaultStyle && view)
        m_medium = adoptPtr(new MediaQueryEvaluator(view->mediaType(), &view->frame(), m_rootDefaultStyle.get()));

    m_styleTree.clear();

    initWatchedSelectorRules(CSSSelectorWatch::from(document).watchedCallbackSelectors());

#if ENABLE(SVG_FONTS)
    if (document.svgExtensions()) {
        const HashSet<SVGFontFaceElement*>& svgFontFaceElements = document.svgExtensions()->svgFontFaceElements();
        HashSet<SVGFontFaceElement*>::const_iterator end = svgFontFaceElements.end();
        for (HashSet<SVGFontFaceElement*>::const_iterator it = svgFontFaceElements.begin(); it != end; ++it)
            fontSelector()->addFontFaceRule((*it)->fontFaceRule());
    }
#endif

    document.styleEngine()->appendActiveAuthorStyleSheets(this);
}
