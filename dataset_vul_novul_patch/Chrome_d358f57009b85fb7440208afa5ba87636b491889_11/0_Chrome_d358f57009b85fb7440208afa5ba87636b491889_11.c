 bool BluetoothDeviceChromeOS::ExpectingPasskey() const {
  return pairing_context_.get() && pairing_context_->ExpectingPasskey();
 }
