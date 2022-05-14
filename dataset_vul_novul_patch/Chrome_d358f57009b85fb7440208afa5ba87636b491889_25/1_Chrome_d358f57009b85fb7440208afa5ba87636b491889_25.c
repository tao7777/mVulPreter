 void BluetoothDeviceChromeOS::SetPasskey(uint32 passkey) {
  if (!agent_.get() || passkey_callback_.is_null())
     return;
 
  passkey_callback_.Run(SUCCESS, passkey);
  passkey_callback_.Reset();
 }
