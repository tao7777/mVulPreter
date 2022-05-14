 DeviceOrientationController::DeviceOrientationController(Document* document)
     : DeviceSensorEventController(document)
     , DOMWindowLifecycleObserver(document->domWindow())
 {
    Page* page = document->page();
    ASSERT(page);
    OwnPtr<DeviceOrientationInspectorAgent> deviceOrientationAgent(DeviceOrientationInspectorAgent::create(page));
    InspectorController& inspectorController = page->inspectorController();
    if (!inspectorController.hasAgent(deviceOrientationAgent.get()->name()))
        inspectorController.registerModuleAgent(deviceOrientationAgent.release());
 }
