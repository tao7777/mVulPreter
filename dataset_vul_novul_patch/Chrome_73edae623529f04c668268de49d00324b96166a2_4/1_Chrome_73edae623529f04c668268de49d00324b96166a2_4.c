PassRefPtr<DocumentFragment> createFragmentFromSource(const String& markup, Element* contextElement, ExceptionCode& ec)
 {
     Document* document = contextElement->document();
     RefPtr<DocumentFragment> fragment = DocumentFragment::create(document);

    if (document->isHTMLDocument()) {
        fragment->parseHTML(markup, contextElement);
        return fragment;
    }

    bool wasValid = fragment->parseXML(markup, contextElement);
    if (!wasValid) {
        ec = INVALID_STATE_ERR;
        return 0;
    }
     return fragment.release();
 }
