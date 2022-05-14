 void HTMLElement::setInnerHTML(const String& html, ExceptionCode& ec)
 {
    RefPtr<DocumentFragment> fragment = createFragmentFromSource(html, this, ec);
    if (fragment)
         replaceChildrenWithFragment(this, fragment.release(), ec);
 }
