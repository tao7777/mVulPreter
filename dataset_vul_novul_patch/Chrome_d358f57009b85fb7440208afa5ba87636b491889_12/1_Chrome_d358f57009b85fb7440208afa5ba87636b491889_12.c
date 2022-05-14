 bool BluetoothDeviceChromeOS::ExpectingPinCode() const {
  return !pincode_callback_.is_null();
 }
