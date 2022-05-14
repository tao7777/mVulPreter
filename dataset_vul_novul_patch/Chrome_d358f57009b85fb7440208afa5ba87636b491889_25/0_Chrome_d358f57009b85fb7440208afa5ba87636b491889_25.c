 void BluetoothDeviceChromeOS::SetPasskey(uint32 passkey) {
  if (!pairing_context_.get())
     return;
 
  pairing_context_->SetPasskey(passkey);
 }
