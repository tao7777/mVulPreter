void EnterpriseEnrollmentScreen::OnPolicyStateChanged(
    policy::CloudPolicySubsystem::PolicySubsystemState state,
    policy::CloudPolicySubsystem::ErrorDetails error_details) {

  if (is_showing_) {
    switch (state) {
      case policy::CloudPolicySubsystem::UNENROLLED:
        return;
      case policy::CloudPolicySubsystem::BAD_GAIA_TOKEN:
      case policy::CloudPolicySubsystem::LOCAL_ERROR:
        actor_->ShowFatalEnrollmentError();
        break;
      case policy::CloudPolicySubsystem::UNMANAGED:
        actor_->ShowAccountError();
        break;
      case policy::CloudPolicySubsystem::NETWORK_ERROR:
        actor_->ShowNetworkEnrollmentError();
        break;
      case policy::CloudPolicySubsystem::TOKEN_FETCHED:
        WriteInstallAttributesData();
        return;
      case policy::CloudPolicySubsystem::SUCCESS:
        registrar_.reset();
        UMA_HISTOGRAM_ENUMERATION(policy::kMetricEnrollment,
                                  policy::kMetricEnrollmentOK,
                                  policy::kMetricEnrollmentSize);
        actor_->ShowConfirmationScreen();
        return;
    }
    if (state == policy::CloudPolicySubsystem::UNMANAGED) {
      UMA_HISTOGRAM_ENUMERATION(policy::kMetricEnrollment,
                                policy::kMetricEnrollmentNotSupported,
                                policy::kMetricEnrollmentSize);
    } else {
      UMA_HISTOGRAM_ENUMERATION(policy::kMetricEnrollment,
                                policy::kMetricEnrollmentPolicyFailed,
                                policy::kMetricEnrollmentSize);
    }
    LOG(WARNING) << "Policy subsystem error during enrollment: " << state
                 << " details: " << error_details;
  }
 
   registrar_.reset();
  g_browser_process->browser_policy_connector()->ResetDevicePolicy();
 }
