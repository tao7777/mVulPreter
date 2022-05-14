 void BluetoothDeviceChromeOS::Connect(
    BluetoothDevice::PairingDelegate* pairing_delegate,
    const base::Closure& callback,
    const ConnectErrorCallback& error_callback) {
  if (num_connecting_calls_++ == 0)
    adapter_->NotifyDeviceChanged(this);

  VLOG(1) << object_path_.value() << ": Connecting, " << num_connecting_calls_
          << " in progress";

  if (IsPaired() || !pairing_delegate || !IsPairable()) {
     ConnectInternal(false, callback, error_callback);
   } else {
    DCHECK(!pairing_context_);
    pairing_context_.reset(
        new BluetoothAdapterChromeOS::PairingContext(pairing_delegate));

    DBusThreadManager::Get()->GetBluetoothDeviceClient()->
        Pair(object_path_,
             base::Bind(&BluetoothDeviceChromeOS::OnPair,
                        weak_ptr_factory_.GetWeakPtr(),
                        callback, error_callback),
             base::Bind(&BluetoothDeviceChromeOS::OnPairError,
                        weak_ptr_factory_.GetWeakPtr(),
                        error_callback));
   }
 }
