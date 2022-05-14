void BluetoothDeviceChromeOS::OnRegisterAgent(
    const base::Closure& callback,
    const ConnectErrorCallback& error_callback) {
  VLOG(1) << object_path_.value() << ": Agent registered, now pairing";
  DBusThreadManager::Get()->GetBluetoothDeviceClient()->
      Pair(object_path_,
           base::Bind(&BluetoothDeviceChromeOS::OnPair,
                      weak_ptr_factory_.GetWeakPtr(),
                      callback, error_callback),
           base::Bind(&BluetoothDeviceChromeOS::OnPairError,
                      weak_ptr_factory_.GetWeakPtr(),
                      error_callback));
}
