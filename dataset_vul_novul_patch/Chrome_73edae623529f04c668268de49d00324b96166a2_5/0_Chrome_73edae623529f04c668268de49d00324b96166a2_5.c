 void HTMLElement::insertAdjacentHTML(const String& where, const String& markup, ExceptionCode& ec)
 {
     Element* contextElement = contextElementForInsertion(where, this, ec);
     if (!contextElement)
         return;
    ExceptionCode ignoredEc = 0; // FIXME: We should propagate a syntax error exception out here.
    RefPtr<DocumentFragment> fragment = createFragmentForInnerOuterHTML(markup, this, ignoredEc);
    if (ignoredEc)
        return;
     insertAdjacent(where, fragment.get(), ec);
 }
