void HTMLLinkElement::removedFromDocument()
{
    HTMLElement::removedFromDocument();

    document()->removeStyleSheetCandidateNode(this);

     if (document()->renderer())
         document()->updateStyleSelector();
    m_shouldProcessAfterAttach = false;
 }
