void CSSDefaultStyleSheets::loadFullDefaultStyle()
void CSSDefaultStyleSheets::loadDefaultStyle()
 {
    ASSERT(!defaultStyle);
    defaultStyle = RuleSet::create().leakPtr();
    defaultPrintStyle = RuleSet::create().leakPtr();
    defaultQuirksStyle = RuleSet::create().leakPtr();
 
     String defaultRules = String(htmlUserAgentStyleSheet, sizeof(htmlUserAgentStyleSheet)) + RenderTheme::theme().extraDefaultStyleSheet();
    defaultStyleSheet = parseUASheet(defaultRules);
    defaultStyle->addRulesFromSheet(defaultStyleSheet, screenEval());
    defaultStyle->addRulesFromSheet(parseUASheet(ViewportStyle::viewportStyleSheet()), screenEval());
    defaultPrintStyle->addRulesFromSheet(defaultStyleSheet, printEval());

    String quirksRules = String(quirksUserAgentStyleSheet, sizeof(quirksUserAgentStyleSheet)) + RenderTheme::theme().extraQuirksStyleSheet();
    quirksStyleSheet = parseUASheet(quirksRules);
     defaultQuirksStyle->addRulesFromSheet(quirksStyleSheet, screenEval());
 }
