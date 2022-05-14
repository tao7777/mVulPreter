bool BluetoothDeviceChromeOS::RunPairingCallbacks(Status status) {
  if (!agent_.get())
    return false;
  bool callback_run = false;
  if (!pincode_callback_.is_null()) {
    pincode_callback_.Run(status, "");
    pincode_callback_.Reset();
    callback_run = true;
  }
  if (!passkey_callback_.is_null()) {
    passkey_callback_.Run(status, 0);
    passkey_callback_.Reset();
    callback_run = true;
  }
  if (!confirmation_callback_.is_null()) {
    confirmation_callback_.Run(status);
    confirmation_callback_.Reset();
    callback_run = true;
  }
  return callback_run;
}
