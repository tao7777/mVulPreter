void EnterpriseEnrollmentScreen::OnAuthCancelled() {
  UMA_HISTOGRAM_ENUMERATION(policy::kMetricEnrollment,
                            policy::kMetricEnrollmentCancelled,
                             policy::kMetricEnrollmentSize);
   auth_fetcher_.reset();
   registrar_.reset();
  g_browser_process->browser_policy_connector()->ResetDevicePolicy();
   get_screen_observer()->OnExit(
       ScreenObserver::ENTERPRISE_ENROLLMENT_CANCELLED);
 }
