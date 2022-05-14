 CSSStyleRule* InspectorCSSOMWrappers::getWrapperForRuleInSheets(StyleRule* rule, StyleEngine* styleSheetCollection)
 {
     if (m_styleRuleToCSSOMWrapperMap.isEmpty()) {
         collectFromStyleSheetContents(m_styleSheetCSSOMWrapperSet, CSSDefaultStyleSheets::defaultStyleSheet);
         collectFromStyleSheetContents(m_styleSheetCSSOMWrapperSet, CSSDefaultStyleSheets::quirksStyleSheet);
         collectFromStyleSheetContents(m_styleSheetCSSOMWrapperSet, CSSDefaultStyleSheets::svgStyleSheet);
        collectFromStyleSheetContents(m_styleSheetCSSOMWrapperSet, CSSDefaultStyleSheets::mediaControlsStyleSheet);
        collectFromStyleSheetContents(m_styleSheetCSSOMWrapperSet, CSSDefaultStyleSheets::fullscreenStyleSheet);

        collectFromStyleEngine(styleSheetCollection);
    }
    return m_styleRuleToCSSOMWrapperMap.get(rule);
}
