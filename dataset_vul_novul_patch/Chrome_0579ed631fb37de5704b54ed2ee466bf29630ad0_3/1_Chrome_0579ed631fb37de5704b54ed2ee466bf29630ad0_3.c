NetworkChangeNotifierMac::NetworkChangeNotifierMac()
    : NetworkChangeNotifier(NetworkChangeCalculatorParamsMac()),
      connection_type_(CONNECTION_UNKNOWN),
       connection_type_initialized_(false),
       initial_connection_type_cv_(&connection_type_lock_),
       forwarder_(this),
      dns_config_service_thread_(base::MakeUnique<DnsConfigServiceThread>()) {
  config_watcher_ = base::MakeUnique<NetworkConfigWatcherMac>(&forwarder_);
   dns_config_service_thread_->StartWithOptions(
       base::Thread::Options(base::MessageLoop::TYPE_IO, 0));
 }
