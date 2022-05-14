static inline RefPtr<DocumentFragment> createFragmentFromSource(const String& sourceString, const String& sourceMIMEType, Document* outputDoc)
{
    RefPtr<DocumentFragment> fragment = outputDoc->createDocumentFragment();
    if (sourceMIMEType == "text/html") {
        RefPtr<HTMLBodyElement> fakeBody = HTMLBodyElement::create(outputDoc);
        fragment->parseHTML(sourceString, fakeBody.get());
    } else if (sourceMIMEType == "text/plain")
        fragment->parserAddChild(Text::create(outputDoc, sourceString));
    else {
        bool successfulParse = fragment->parseXML(sourceString, 0);
        if (!successfulParse)
            return 0;
    }
    return fragment;
}
