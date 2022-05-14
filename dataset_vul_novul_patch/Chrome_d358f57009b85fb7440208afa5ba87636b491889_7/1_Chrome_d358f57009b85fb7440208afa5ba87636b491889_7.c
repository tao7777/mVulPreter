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
    DCHECK(!pairing_delegate_);
    DCHECK(agent_.get() == NULL);
    pairing_delegate_ = pairing_delegate;
    pairing_delegate_used_ = false;
    dbus::Bus* system_bus = DBusThreadManager::Get()->GetSystemBus();
    agent_.reset(BluetoothAgentServiceProvider::Create(
        system_bus, dbus::ObjectPath(kAgentPath), this));
    DCHECK(agent_.get());
    VLOG(1) << object_path_.value() << ": Registering agent for pairing";
    DBusThreadManager::Get()->GetBluetoothAgentManagerClient()->
        RegisterAgent(
            dbus::ObjectPath(kAgentPath),
            bluetooth_agent_manager::kKeyboardDisplayCapability,
            base::Bind(&BluetoothDeviceChromeOS::OnRegisterAgent,
                       weak_ptr_factory_.GetWeakPtr(),
                       callback,
                       error_callback),
            base::Bind(&BluetoothDeviceChromeOS::OnRegisterAgentError,
                       weak_ptr_factory_.GetWeakPtr(),
                       error_callback));
   }
 }
