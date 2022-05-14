 void InspectorPageAgent::setGeolocationOverride(ErrorString* error, const double* latitude, const double* longitude, const double* accuracy)
{
    GeolocationController* controller = GeolocationController::from(m_page);
    GeolocationPosition* position = 0;
    if (!controller) {
        *error = "Internal error: unable to override geolocation";
        return;
    }
    position = controller->lastPosition();
    if (!m_geolocationOverridden && position)
        m_platformGeolocationPosition = position;

    m_geolocationOverridden = true;
    if (latitude && longitude && accuracy)
        m_geolocationPosition = GeolocationPosition::create(currentTimeMS(), *latitude, *longitude, *accuracy);
    else
         m_geolocationPosition.clear();
 
     controller->positionChanged(0); // Kick location update.
    updateSensorsOverlayMessage();
 }
