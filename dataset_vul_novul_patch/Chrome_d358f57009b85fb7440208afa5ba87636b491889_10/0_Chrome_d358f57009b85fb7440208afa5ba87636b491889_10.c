 bool BluetoothDeviceChromeOS::ExpectingConfirmation() const {
  return pairing_context_.get() && pairing_context_->ExpectingConfirmation();
 }
