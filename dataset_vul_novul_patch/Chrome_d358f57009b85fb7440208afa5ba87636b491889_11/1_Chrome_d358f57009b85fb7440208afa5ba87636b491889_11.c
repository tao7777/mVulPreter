 bool BluetoothDeviceChromeOS::ExpectingPasskey() const {
  return !passkey_callback_.is_null();
 }
