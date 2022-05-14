void FakeBluetoothAgentManagerClient::UnregisterAgent(
    const dbus::ObjectPath& agent_path,
     const base::Closure& callback,
     const ErrorCallback& error_callback) {
   VLOG(1) << "UnregisterAgent: " << agent_path.value();
  if (service_provider_ != NULL) {
    error_callback.Run(bluetooth_agent_manager::kErrorInvalidArguments,
                        "Agent still registered");
   } else {
     callback.Run();
  }
}
