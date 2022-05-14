void BluetoothDeviceChromeOS::OnPairError(
    const ConnectErrorCallback& error_callback,
    const std::string& error_name,
    const std::string& error_message) {
  if (--num_connecting_calls_ == 0)
    adapter_->NotifyDeviceChanged(this);

  DCHECK(num_connecting_calls_ >= 0);
  LOG(WARNING) << object_path_.value() << ": Failed to pair device: "
               << error_name << ": " << error_message;
   VLOG(1) << object_path_.value() << ": " << num_connecting_calls_
           << " still in progress";
 
  UnregisterAgent();
 
   ConnectErrorCode error_code = ERROR_UNKNOWN;
  if (error_name == bluetooth_device::kErrorConnectionAttemptFailed) {
    error_code = ERROR_FAILED;
  } else if (error_name == bluetooth_device::kErrorFailed) {
    error_code = ERROR_FAILED;
  } else if (error_name == bluetooth_device::kErrorAuthenticationFailed) {
    error_code = ERROR_AUTH_FAILED;
  } else if (error_name == bluetooth_device::kErrorAuthenticationCanceled) {
    error_code = ERROR_AUTH_CANCELED;
  } else if (error_name == bluetooth_device::kErrorAuthenticationRejected) {
    error_code = ERROR_AUTH_REJECTED;
  } else if (error_name == bluetooth_device::kErrorAuthenticationTimeout) {
    error_code = ERROR_AUTH_TIMEOUT;
  }

  RecordPairingResult(error_code);
  error_callback.Run(error_code);
}
