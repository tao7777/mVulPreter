PassRefPtr<DocumentFragment> Range::createDocumentFragmentForElement(const String& markup, Element* element,  FragmentScriptingPermission scriptingPermission)
{
    ASSERT(element);
    HTMLElement* htmlElement = toHTMLElement(element);
    if (htmlElement->ieForbidsInsertHTML())
        return 0;
    if (htmlElement->hasLocalName(colTag) || htmlElement->hasLocalName(colgroupTag) || htmlElement->hasLocalName(framesetTag)
        || htmlElement->hasLocalName(headTag) || htmlElement->hasLocalName(styleTag) || htmlElement->hasLocalName(titleTag))
        return 0;
    RefPtr<DocumentFragment> fragment = element->document()->createDocumentFragment();
    if (element->document()->isHTMLDocument())
        fragment->parseHTML(markup, element, scriptingPermission);
    else if (!fragment->parseXML(markup, element, scriptingPermission))
        return 0; // FIXME: We should propagate a syntax error exception out here.
    RefPtr<Node> nextNode;
    for (RefPtr<Node> node = fragment->firstChild(); node; node = nextNode) {
        nextNode = node->nextSibling();
        if (node->hasTagName(htmlTag) || node->hasTagName(headTag) || node->hasTagName(bodyTag)) {
            HTMLElement* element = toHTMLElement(node.get());
            if (Node* firstChild = element->firstChild())
                nextNode = firstChild;
            removeElementPreservingChildren(fragment, element);
        }
    }
    return fragment.release();
}
