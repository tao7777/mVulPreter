 void CSSDefaultStyleSheets::ensureDefaultStyleSheetsForElement(Element* element, bool& changedDefaultStyle)
 {
     if (element->isSVGElement() && !svgStyleSheet) {
         svgStyleSheet = parseUASheet(svgUserAgentStyleSheet, sizeof(svgUserAgentStyleSheet));
        defaultStyle->addRulesFromSheet(svgStyleSheet, screenEval());
        defaultPrintStyle->addRulesFromSheet(svgStyleSheet, printEval());
        changedDefaultStyle = true;
    }

    if (!mediaControlsStyleSheet && (isHTMLVideoElement(element) || element->hasTagName(audioTag))) {
        String mediaRules = String(mediaControlsUserAgentStyleSheet, sizeof(mediaControlsUserAgentStyleSheet)) + RenderTheme::theme().extraMediaControlsStyleSheet();
        mediaControlsStyleSheet = parseUASheet(mediaRules);
        defaultStyle->addRulesFromSheet(mediaControlsStyleSheet, screenEval());
        defaultPrintStyle->addRulesFromSheet(mediaControlsStyleSheet, printEval());
        changedDefaultStyle = true;
    }

    if (!fullscreenStyleSheet && FullscreenElementStack::isFullScreen(&element->document())) {
        String fullscreenRules = String(fullscreenUserAgentStyleSheet, sizeof(fullscreenUserAgentStyleSheet)) + RenderTheme::theme().extraFullScreenStyleSheet();
        fullscreenStyleSheet = parseUASheet(fullscreenRules);
        defaultStyle->addRulesFromSheet(fullscreenStyleSheet, screenEval());
        defaultQuirksStyle->addRulesFromSheet(fullscreenStyleSheet, screenEval());
        changedDefaultStyle = true;
    }

    ASSERT(defaultStyle->features().idsInRules.isEmpty());
    ASSERT(defaultStyle->features().siblingRules.isEmpty());
}
