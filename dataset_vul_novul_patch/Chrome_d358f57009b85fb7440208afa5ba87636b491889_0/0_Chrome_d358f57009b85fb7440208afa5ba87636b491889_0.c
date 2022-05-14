 BluetoothAdapterChromeOS::BluetoothAdapterChromeOS()
    : weak_ptr_factory_(this) {
  DBusThreadManager::Get()->GetBluetoothAdapterClient()->AddObserver(this);
  DBusThreadManager::Get()->GetBluetoothDeviceClient()->AddObserver(this);
  DBusThreadManager::Get()->GetBluetoothInputClient()->AddObserver(this);

  std::vector<dbus::ObjectPath> object_paths =
      DBusThreadManager::Get()->GetBluetoothAdapterClient()->GetAdapters();

  if (!object_paths.empty()) {
     VLOG(1) << object_paths.size() << " Bluetooth adapter(s) available.";
     SetAdapter(object_paths[0]);
   }

  // Register the pairing agent.
  dbus::Bus* system_bus = DBusThreadManager::Get()->GetSystemBus();
  agent_.reset(BluetoothAgentServiceProvider::Create(
      system_bus, dbus::ObjectPath(kAgentPath), this));
  DCHECK(agent_.get());

  VLOG(1) << "Registering pairing agent";
  DBusThreadManager::Get()->GetBluetoothAgentManagerClient()->
      RegisterAgent(
          dbus::ObjectPath(kAgentPath),
          bluetooth_agent_manager::kKeyboardDisplayCapability,
          base::Bind(&BluetoothAdapterChromeOS::OnRegisterAgent,
                     weak_ptr_factory_.GetWeakPtr()),
          base::Bind(&BluetoothAdapterChromeOS::OnRegisterAgentError,
                     weak_ptr_factory_.GetWeakPtr()));
 }
