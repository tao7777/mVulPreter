void InspectorPageAgent::updateSensorsOverlayMessage()
{
    m_overlay->setOverride(InspectorOverlay::SensorsOverride, m_geolocationOverridden || m_deviceOrientation);
 }
