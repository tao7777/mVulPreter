DateTimeFieldElement::DateTimeFieldElement(Document* document, FieldOwner& fieldOwner)
    : HTMLElement(spanTag, document)
    , m_fieldOwner(&fieldOwner)
 {
     setAttribute(roleAttr, "spinbutton");
 }
