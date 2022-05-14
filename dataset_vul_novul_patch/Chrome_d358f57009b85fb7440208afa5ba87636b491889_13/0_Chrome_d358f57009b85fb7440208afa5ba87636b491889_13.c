 void BluetoothDeviceChromeOS::OnPair(
     const base::Closure& callback,
     const ConnectErrorCallback& error_callback) {
   VLOG(1) << object_path_.value() << ": Paired";
 
  pairing_context_.reset();

   SetTrusted();
   ConnectInternal(true, callback, error_callback);
 }
