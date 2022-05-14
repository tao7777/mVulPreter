void StyleResolver::matchUARules(ElementRuleCollector& collector)
 {
     collector.setMatchingUARules(true);
 
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
