void BrowserPolicyConnector::DeviceStopAutoRetry() {
void BrowserPolicyConnector::ResetDevicePolicy() {
 #if defined(OS_CHROMEOS)
   if (device_cloud_policy_subsystem_.get())
    device_cloud_policy_subsystem_->Reset();
 #endif
 }
