 void BluetoothDeviceChromeOS::CancelPairing() {
  if (!RunPairingCallbacks(CANCELLED)) {
     DBusThreadManager::Get()->GetBluetoothDeviceClient()->
         CancelPairing(
             object_path_,
            base::Bind(&base::DoNothing),
            base::Bind(&BluetoothDeviceChromeOS::OnCancelPairingError,
                        weak_ptr_factory_.GetWeakPtr()));
 
    UnregisterAgent();
   }
 }
