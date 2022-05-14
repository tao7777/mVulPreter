static inline bool elementCanUseSimpleDefaultStyle(Element* e)
{
    return isHTMLHtmlElement(e) || e->hasTagName(headTag) || e->hasTagName(bodyTag) || e->hasTagName(divTag) || e->hasTagName(spanTag) || e->hasTagName(brTag) || isHTMLAnchorElement(e);
}
