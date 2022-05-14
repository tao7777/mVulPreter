void InspectorPageAgent::setEmulatedMedia(ErrorString*, const String& media)
{
    String currentMedia = m_state->getString(PageAgentState::pageAgentEmulatedMedia);
    if (media == currentMedia)
        return;

    m_state->setString(PageAgentState::pageAgentEmulatedMedia, media);
    Document* document = 0;
    if (m_page->mainFrame())
        document = m_page->mainFrame()->document();
    if (document) {
         document->styleResolverChanged(RecalcStyleImmediately);
         document->updateLayout();
     }
    m_overlay->setOverride(InspectorOverlay::CSSMediaOverride, !media.isEmpty());
 }
