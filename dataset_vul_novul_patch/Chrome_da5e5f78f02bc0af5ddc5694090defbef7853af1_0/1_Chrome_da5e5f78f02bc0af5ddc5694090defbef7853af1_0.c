void InspectorPageAgent::setDeviceOrientationOverride(ErrorString* error, double alpha, double beta, double gamma)
{
    DeviceOrientationController* controller = DeviceOrientationController::from(mainFrame()->document());
    if (!controller) {
        *error = "Internal error: unable to override device orientation";
        return;
    }
    controller->didChangeDeviceOrientation(DeviceOrientationData::create(true, alpha, true, beta, true, gamma).get());
}
