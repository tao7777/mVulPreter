void InspectorPageAgent::clearDeviceOrientationOverride(ErrorString* error)
{
    setDeviceOrientationOverride(error, 0, 0, 0);
}
