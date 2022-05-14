void InspectorPageAgent::updateOverridesTopOffset()
{
    static const int continousPaintingGraphHeight = 92;
    static const int fpsGraphHeight = 73;
    int topOffset = 0;
    if (m_state->getBoolean(PageAgentState::pageAgentContinuousPaintingEnabled))
        topOffset = continousPaintingGraphHeight;
    else if (m_state->getBoolean(PageAgentState::pageAgentShowFPSCounter))
        topOffset = fpsGraphHeight;
    bool setOffset = false;
    if (setOffset)
        m_overlay->setOverridesTopOffset(topOffset);
}
