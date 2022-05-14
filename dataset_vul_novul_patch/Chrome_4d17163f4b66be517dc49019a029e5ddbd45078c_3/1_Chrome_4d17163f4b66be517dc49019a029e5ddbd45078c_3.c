void CSSDefaultStyleSheets::loadSimpleDefaultStyle()
{
    ASSERT(!defaultStyle);
    ASSERT(!simpleDefaultStyleSheet);
    defaultStyle = RuleSet::create().leakPtr();
    defaultPrintStyle = defaultStyle;
    defaultQuirksStyle = RuleSet::create().leakPtr();
    simpleDefaultStyleSheet = parseUASheet(simpleUserAgentStyleSheet, strlen(simpleUserAgentStyleSheet));
    defaultStyle->addRulesFromSheet(simpleDefaultStyleSheet, screenEval());
    defaultStyle->addRulesFromSheet(parseUASheet(ViewportStyle::viewportStyleSheet()), screenEval());
}
