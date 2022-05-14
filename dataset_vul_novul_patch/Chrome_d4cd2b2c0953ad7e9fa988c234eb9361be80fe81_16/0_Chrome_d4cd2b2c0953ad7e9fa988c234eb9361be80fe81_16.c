 void InspectorPageAgent::updateTouchEventEmulationInPage(bool enabled)
 {
     m_state->setBoolean(PageAgentState::touchEventEmulationEnabled, enabled);
     if (mainFrame() && mainFrame()->settings())
         mainFrame()->settings()->setTouchEventEmulationEnabled(enabled);
