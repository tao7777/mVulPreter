 BluetoothAdapterChromeOS::~BluetoothAdapterChromeOS() {
   DBusThreadManager::Get()->GetBluetoothAdapterClient()->RemoveObserver(this);
   DBusThreadManager::Get()->GetBluetoothDeviceClient()->RemoveObserver(this);
   DBusThreadManager::Get()->GetBluetoothInputClient()->RemoveObserver(this);
 }
