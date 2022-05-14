void BluetoothDeviceChromeOS::OnRegisterAgentError(
    const ConnectErrorCallback& error_callback,
    const std::string& error_name,
    const std::string& error_message) {
  if (--num_connecting_calls_ == 0)
    adapter_->NotifyDeviceChanged(this);
  DCHECK(num_connecting_calls_ >= 0);
  LOG(WARNING) << object_path_.value() << ": Failed to register agent: "
               << error_name << ": " << error_message;
  VLOG(1) << object_path_.value() << ": " << num_connecting_calls_
          << " still in progress";
  UnregisterAgent();
  ConnectErrorCode error_code = ERROR_UNKNOWN;
  if (error_name == bluetooth_agent_manager::kErrorAlreadyExists)
    error_code = ERROR_INPROGRESS;
  RecordPairingResult(error_code);
  error_callback.Run(error_code);
}
