 void InspectorPageAgent::clearGeolocationOverride(ErrorString*)
{
    if (!m_geolocationOverridden)
        return;
    m_geolocationOverridden = false;
    m_geolocationPosition.clear();

     GeolocationController* controller = GeolocationController::from(m_page);
     if (controller && m_platformGeolocationPosition.get())
         controller->positionChanged(m_platformGeolocationPosition.get());
 }
