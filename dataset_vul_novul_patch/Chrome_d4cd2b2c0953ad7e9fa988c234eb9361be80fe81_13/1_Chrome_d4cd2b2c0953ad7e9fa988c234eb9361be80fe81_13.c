void InspectorPageAgent::setShowFPSCounter(ErrorString*, bool show)
{
     bool viewMetricsOverride = m_state->getLong(PageAgentState::pageAgentScreenWidthOverride);
     m_state->setBoolean(PageAgentState::pageAgentShowFPSCounter, show);
     m_client->setShowFPSCounter(show && !viewMetricsOverride);
    updateOverridesTopOffset();
 }
