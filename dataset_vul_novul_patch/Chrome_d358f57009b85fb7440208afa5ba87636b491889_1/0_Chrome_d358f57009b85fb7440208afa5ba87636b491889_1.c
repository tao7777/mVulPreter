 BluetoothAdapterChromeOS::~BluetoothAdapterChromeOS() {
   DBusThreadManager::Get()->GetBluetoothAdapterClient()->RemoveObserver(this);
   DBusThreadManager::Get()->GetBluetoothDeviceClient()->RemoveObserver(this);
   DBusThreadManager::Get()->GetBluetoothInputClient()->RemoveObserver(this);

  VLOG(1) << "Unregistering pairing agent";
  DBusThreadManager::Get()->GetBluetoothAgentManagerClient()->
      UnregisterAgent(
          dbus::ObjectPath(kAgentPath),
          base::Bind(&base::DoNothing),
          base::Bind(&OnUnregisterAgentError));
 }
