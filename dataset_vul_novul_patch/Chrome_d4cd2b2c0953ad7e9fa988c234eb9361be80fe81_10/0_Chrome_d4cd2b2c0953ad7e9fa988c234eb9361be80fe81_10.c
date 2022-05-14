void InspectorPageAgent::setDeviceOrientationOverride(ErrorString* error, double alpha, double beta, double gamma)
{
    NewDeviceOrientationController* controller = NewDeviceOrientationController::from(mainFrame()->document());
    if (!controller) {
        *error = "Internal error: unable to override device orientation";
        return;
    }

    ErrorString clearError;
    clearDeviceOrientationOverride(&clearError);
 
     m_deviceOrientation = DeviceOrientationData::create(true, alpha, true, beta, true, gamma);
     controller->didChangeDeviceOrientation(m_deviceOrientation.get());
 }
