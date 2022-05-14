 void BluetoothDeviceChromeOS::OnPair(
     const base::Closure& callback,
     const ConnectErrorCallback& error_callback) {
   VLOG(1) << object_path_.value() << ": Paired";
 
  if (!pairing_delegate_used_)
    UMA_HISTOGRAM_ENUMERATION("Bluetooth.PairingMethod",
                              UMA_PAIRING_METHOD_NONE,
                              UMA_PAIRING_METHOD_COUNT);
  UnregisterAgent();
   SetTrusted();
   ConnectInternal(true, callback, error_callback);
 }
