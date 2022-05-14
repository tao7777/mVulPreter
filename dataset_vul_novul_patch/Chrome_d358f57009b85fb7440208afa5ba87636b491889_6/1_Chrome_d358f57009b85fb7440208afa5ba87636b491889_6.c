 void BluetoothDeviceChromeOS::ConfirmPairing() {
  if (!agent_.get() || confirmation_callback_.is_null())
     return;
 
  confirmation_callback_.Run(SUCCESS);
  confirmation_callback_.Reset();
 }
