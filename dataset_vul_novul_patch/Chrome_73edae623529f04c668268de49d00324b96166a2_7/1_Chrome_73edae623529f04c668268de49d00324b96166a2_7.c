void HTMLElement::setOuterHTML(const String& html, ExceptionCode& ec)
{
    Node* p = parentNode();
    if (!p || !p->isHTMLElement()) {
        ec = NO_MODIFICATION_ALLOWED_ERR;
        return;
    }
    RefPtr<HTMLElement> parent = toHTMLElement(p);
     RefPtr<Node> prev = previousSibling();
     RefPtr<Node> next = nextSibling();
 
    RefPtr<DocumentFragment> fragment = createFragmentFromSource(html, parent.get(), ec);
     if (ec)
         return;
       
    parent->replaceChild(fragment.release(), this, ec);
    RefPtr<Node> node = next ? next->previousSibling() : 0;
    if (!ec && node && node->isTextNode())
        mergeWithNextTextNode(node.release(), ec);

    if (!ec && prev && prev->isTextNode())
        mergeWithNextTextNode(prev.release(), ec);
}
