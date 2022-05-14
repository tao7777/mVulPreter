 void BluetoothDeviceChromeOS::CancelPairing() {
  if (!pairing_context_.get() || !pairing_context_->CancelPairing()) {
    VLOG(1) << object_path_.value() << ": No pairing context or callback. "
            << "Sending explicit cancel";
     DBusThreadManager::Get()->GetBluetoothDeviceClient()->
         CancelPairing(
             object_path_,
            base::Bind(&base::DoNothing),
            base::Bind(&BluetoothDeviceChromeOS::OnCancelPairingError,
                        weak_ptr_factory_.GetWeakPtr()));
 
    // delegate is going to be freed before things complete, so clear out the
    // context holding it.
    pairing_context_.reset();
   }
 }
