void CSSDefaultStyleSheets::loadFullDefaultStyle()
 {
    if (simpleDefaultStyleSheet) {
        ASSERT(defaultStyle);
        ASSERT(defaultPrintStyle == defaultStyle);
        delete defaultStyle;
        simpleDefaultStyleSheet->deref();
        defaultStyle = RuleSet::create().leakPtr();
        defaultPrintStyle = RuleSet::create().leakPtr();
        simpleDefaultStyleSheet = 0;
    } else {
        ASSERT(!defaultStyle);
        defaultStyle = RuleSet::create().leakPtr();
        defaultPrintStyle = RuleSet::create().leakPtr();
        defaultQuirksStyle = RuleSet::create().leakPtr();
    }
 
     String defaultRules = String(htmlUserAgentStyleSheet, sizeof(htmlUserAgentStyleSheet)) + RenderTheme::theme().extraDefaultStyleSheet();
    defaultStyleSheet = parseUASheet(defaultRules);
    defaultStyle->addRulesFromSheet(defaultStyleSheet, screenEval());
    defaultStyle->addRulesFromSheet(parseUASheet(ViewportStyle::viewportStyleSheet()), screenEval());
    defaultPrintStyle->addRulesFromSheet(defaultStyleSheet, printEval());

    String quirksRules = String(quirksUserAgentStyleSheet, sizeof(quirksUserAgentStyleSheet)) + RenderTheme::theme().extraQuirksStyleSheet();
    quirksStyleSheet = parseUASheet(quirksRules);
     defaultQuirksStyle->addRulesFromSheet(quirksStyleSheet, screenEval());
 }
