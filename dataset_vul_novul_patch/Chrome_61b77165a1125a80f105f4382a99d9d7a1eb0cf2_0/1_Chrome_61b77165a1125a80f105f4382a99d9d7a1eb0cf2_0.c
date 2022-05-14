inline HTMLLinkElement::HTMLLinkElement(const QualifiedName& tagName, Document* document, bool createdByParser)
    : HTMLElement(tagName, document)
     , m_disabledState(Unset)
     , m_loading(false)
     , m_createdByParser(createdByParser)
    , m_shouldProcessAfterAttach(false)
 {
     ASSERT(hasTagName(linkTag));
 }
