 void BluetoothDeviceChromeOS::RejectPairing() {
  if (!pairing_context_.get())
    return;

  pairing_context_->RejectPairing();
 }
