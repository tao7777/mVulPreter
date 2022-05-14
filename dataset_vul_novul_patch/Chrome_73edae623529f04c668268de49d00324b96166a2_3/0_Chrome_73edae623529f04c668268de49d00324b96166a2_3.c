PassRefPtr<DocumentFragment> createFragmentFromMarkup(Document* document, const String& markup, const String& baseURL, FragmentScriptingPermission scriptingPermission)
 {
     RefPtr<HTMLBodyElement> fakeBody = HTMLBodyElement::create(document);
    RefPtr<DocumentFragment> fragment = createContextualFragment(markup, fakeBody.get(), scriptingPermission);
 
     if (fragment && !baseURL.isEmpty() && baseURL != blankURL() && baseURL != document->baseURL())
         completeURLs(fragment.get(), baseURL);

    return fragment.release();
}
