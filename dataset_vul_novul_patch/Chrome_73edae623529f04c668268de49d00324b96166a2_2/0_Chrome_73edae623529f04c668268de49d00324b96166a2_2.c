 void ShadowRoot::setInnerHTML(const String& markup, ExceptionCode& ec)
 {
    if (RefPtr<DocumentFragment> fragment = createFragmentForInnerOuterHTML(markup, host(), ec))
         replaceChildrenWithFragment(this, fragment.release(), ec);
 }
