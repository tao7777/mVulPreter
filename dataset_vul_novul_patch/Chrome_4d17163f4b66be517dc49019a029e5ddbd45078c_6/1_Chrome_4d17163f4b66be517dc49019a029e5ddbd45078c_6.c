void StyleResolver::matchUARules(ElementRuleCollector& collector)
 {
     collector.setMatchingUARules(true);
 
    if (CSSDefaultStyleSheets::simpleDefaultStyleSheet)
        collector.matchedResult().isCacheable = false;
     RuleSet* userAgentStyleSheet = m_medium->mediaTypeMatchSpecific("print")
         ? CSSDefaultStyleSheets::defaultPrintStyle : CSSDefaultStyleSheets::defaultStyle;
     matchUARules(collector, userAgentStyleSheet);

    if (document().inQuirksMode())
        matchUARules(collector, CSSDefaultStyleSheets::defaultQuirksStyle);

    if (document().isViewSource())
        matchUARules(collector, CSSDefaultStyleSheets::viewSourceStyle());

    collector.setMatchingUARules(false);

    matchWatchSelectorRules(collector);
}
