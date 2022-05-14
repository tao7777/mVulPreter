 bool BluetoothDeviceChromeOS::ExpectingPinCode() const {
  return pairing_context_.get() && pairing_context_->ExpectingPinCode();
 }
