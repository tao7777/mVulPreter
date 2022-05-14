 void HTMLElement::setInnerHTML(const String& html, ExceptionCode& ec)
 {
    if (RefPtr<DocumentFragment> fragment = createFragmentForInnerOuterHTML(html, this, ec))
         replaceChildrenWithFragment(this, fragment.release(), ec);
 }
