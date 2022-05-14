 void BluetoothDeviceChromeOS::ConfirmPairing() {
  if (!pairing_context_.get())
     return;
 
  pairing_context_->ConfirmPairing();
 }
