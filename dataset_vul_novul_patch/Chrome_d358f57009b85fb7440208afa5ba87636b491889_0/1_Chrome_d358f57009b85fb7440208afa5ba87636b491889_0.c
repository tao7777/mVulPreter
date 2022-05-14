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
 }
