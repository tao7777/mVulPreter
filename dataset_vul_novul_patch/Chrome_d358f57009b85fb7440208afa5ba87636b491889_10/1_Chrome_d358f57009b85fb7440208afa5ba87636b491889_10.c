 bool BluetoothDeviceChromeOS::ExpectingConfirmation() const {
  return !confirmation_callback_.is_null();
 }
