void CSSDefaultStyleSheets::initDefaultStyle(Element* root)
void CSSDefaultStyleSheets::loadDefaultStylesheetIfNecessary()
 {
    if (!defaultStyle)
        loadDefaultStyle();
 }
