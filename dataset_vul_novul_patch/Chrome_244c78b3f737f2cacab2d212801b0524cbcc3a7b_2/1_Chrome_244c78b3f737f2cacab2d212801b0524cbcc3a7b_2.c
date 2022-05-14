void BrowserPolicyConnector::DeviceStopAutoRetry() {
 #if defined(OS_CHROMEOS)
   if (device_cloud_policy_subsystem_.get())
    device_cloud_policy_subsystem_->StopAutoRetry();
 #endif
 }
