 void InspectorResourceAgent::setUserAgentOverride(ErrorString*, const String& userAgent)
 {
     m_state->setString(ResourceAgentState::userAgentOverride, userAgent);
    m_overlay->setOverride(InspectorOverlay::UserAgentOverride, !userAgent.isEmpty());
 }
