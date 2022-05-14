void WebPagePrivate::willComposite()
 {
     if (!m_page->settings()->developerExtrasEnabled())
         return;
    m_page->inspectorController()->willComposite();
 }
