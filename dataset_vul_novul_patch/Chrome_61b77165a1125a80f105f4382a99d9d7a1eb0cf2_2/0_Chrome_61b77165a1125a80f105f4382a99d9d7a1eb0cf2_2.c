 void HTMLLinkElement::insertedIntoDocument()
 {
     HTMLElement::insertedIntoDocument();
     document()->addStyleSheetCandidateNode(this, m_createdByParser);
 
     process();
 }
