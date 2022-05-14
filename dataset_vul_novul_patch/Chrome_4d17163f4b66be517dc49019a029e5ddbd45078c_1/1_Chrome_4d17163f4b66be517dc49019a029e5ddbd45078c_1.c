void CSSDefaultStyleSheets::initDefaultStyle(Element* root)
 {
    if (!defaultStyle) {
        if (!root || elementCanUseSimpleDefaultStyle(root))
            loadSimpleDefaultStyle();
        else
            loadFullDefaultStyle();
    }
 }
