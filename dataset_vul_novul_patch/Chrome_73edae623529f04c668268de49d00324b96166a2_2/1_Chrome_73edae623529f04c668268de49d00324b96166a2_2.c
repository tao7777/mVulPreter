 void ShadowRoot::setInnerHTML(const String& markup, ExceptionCode& ec)
 {
    RefPtr<DocumentFragment> fragment = createFragmentFromSource(markup, host(), ec);
    if (fragment)
         replaceChildrenWithFragment(this, fragment.release(), ec);
 }
