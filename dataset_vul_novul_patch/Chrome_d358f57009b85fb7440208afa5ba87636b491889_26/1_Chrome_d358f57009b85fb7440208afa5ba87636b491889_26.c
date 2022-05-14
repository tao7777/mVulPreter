 void BluetoothDeviceChromeOS::SetPinCode(const std::string& pincode) {
  if (!agent_.get() || pincode_callback_.is_null())
     return;
 
  pincode_callback_.Run(SUCCESS, pincode);
  pincode_callback_.Reset();
 }
