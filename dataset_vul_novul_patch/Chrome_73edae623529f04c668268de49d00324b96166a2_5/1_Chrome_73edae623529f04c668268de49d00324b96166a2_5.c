 void HTMLElement::insertAdjacentHTML(const String& where, const String& markup, ExceptionCode& ec)
 {
    RefPtr<DocumentFragment> fragment = document()->createDocumentFragment();
     Element* contextElement = contextElementForInsertion(where, this, ec);
     if (!contextElement)
         return;
    if (document()->isHTMLDocument())
         fragment->parseHTML(markup, contextElement);
    else {
        if (!fragment->parseXML(markup, contextElement))
            return;
    }
     insertAdjacent(where, fragment.get(), ec);
 }
