 void InspectorPageAgent::setContinuousPaintingEnabled(ErrorString*, bool enabled)
 {
     bool viewMetricsOverride = m_state->getLong(PageAgentState::pageAgentScreenWidthOverride);
     m_state->setBoolean(PageAgentState::pageAgentContinuousPaintingEnabled, enabled);
     m_client->setContinuousPaintingEnabled(enabled && !viewMetricsOverride);
    updateOverridesTopOffset();
 }
