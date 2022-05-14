 DeviceOrientationController::DeviceOrientationController(Document* document)
     : DeviceSensorEventController(document)
     , DOMWindowLifecycleObserver(document->domWindow())
 {
 }
