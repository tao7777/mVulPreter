void InspectorPageAgent::updateViewMetrics(int width, int height, double deviceScaleFactor, bool fitWindow, bool textAutosizing)
{
    m_client->overrideDeviceMetrics(width, height, static_cast<float>(deviceScaleFactor), fitWindow);

    Settings& settings = m_page->settings();
    if (m_enabled && textAutosizing) {
        IntSize textAutosizingWindowSizeOverride = IntSize(width, height);
        textAutosizingWindowSizeOverride.scale((float)(1.0 / deviceScaleFactor));
        settings.setTextAutosizingWindowSizeOverride(textAutosizingWindowSizeOverride);
    }

    Document* document = mainFrame()->document();
     if (document)
         document->styleResolverChanged(RecalcStyleImmediately);
     InspectorInstrumentation::mediaQueryResultChanged(document);
 
     bool override = width && height;
     m_client->setShowFPSCounter(m_state->getBoolean(PageAgentState::pageAgentShowFPSCounter) && !override);
     m_client->setContinuousPaintingEnabled(m_state->getBoolean(PageAgentState::pageAgentContinuousPaintingEnabled) && !override);
 }
