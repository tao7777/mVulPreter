 void WebPagePrivate::didComposite()
 {
     if (!m_page->settings()->developerExtrasEnabled())
         return;
    m_page->inspectorController()->didComposite();
 }
