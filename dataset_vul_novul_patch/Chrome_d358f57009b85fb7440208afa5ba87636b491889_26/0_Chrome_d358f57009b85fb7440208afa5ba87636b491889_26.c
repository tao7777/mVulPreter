 void BluetoothDeviceChromeOS::SetPinCode(const std::string& pincode) {
  if (!pairing_context_.get())
     return;
 
  pairing_context_->SetPinCode(pincode);
 }
