void EnterpriseEnrollmentScreen::RegisterForDevicePolicy(
    const std::string& token,
    policy::BrowserPolicyConnector::TokenType token_type) {
  policy::BrowserPolicyConnector* connector =
      g_browser_process->browser_policy_connector();
  if (!connector->device_cloud_policy_subsystem()) {
    NOTREACHED() << "Cloud policy subsystem not initialized.";
    UMA_HISTOGRAM_ENUMERATION(policy::kMetricEnrollment,
                              policy::kMetricEnrollmentOtherFailed,
                              policy::kMetricEnrollmentSize);
    if (is_showing_)
      actor_->ShowFatalEnrollmentError();
    return;
  }

  connector->ScheduleServiceInitialization(0);
  registrar_.reset(new policy::CloudPolicySubsystem::ObserverRegistrar(
      connector->device_cloud_policy_subsystem(), this));
 
  connector->SetDeviceCredentials(user_, token, token_type);
 }
