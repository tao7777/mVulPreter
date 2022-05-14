ShellContentUtilityClient::ShellContentUtilityClient() {
ShellContentUtilityClient::ShellContentUtilityClient(bool is_browsertest) {
  if (is_browsertest &&
      base::CommandLine::ForCurrentProcess()->GetSwitchValueASCII(
          switches::kProcessType) == switches::kUtilityProcess) {
     network_service_test_helper_ = std::make_unique<NetworkServiceTestHelper>();
  }
 }
