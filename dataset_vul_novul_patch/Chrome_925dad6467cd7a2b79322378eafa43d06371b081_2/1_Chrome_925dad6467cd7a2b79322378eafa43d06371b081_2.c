ShellContentUtilityClient::ShellContentUtilityClient() {
  if (base::CommandLine::ForCurrentProcess()->GetSwitchValueASCII(
          switches::kProcessType) == switches::kUtilityProcess)
     network_service_test_helper_ = std::make_unique<NetworkServiceTestHelper>();
 }
