void BluetoothDeviceChromeOS::OnUnregisterAgentError(
    const std::string& error_name,
    const std::string& error_message) {
  LOG(WARNING) << object_path_.value() << ": Failed to unregister agent: "
               << error_name << ": " << error_message;
}
