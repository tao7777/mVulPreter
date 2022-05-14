BluetoothDeviceChromeOS::BluetoothDeviceChromeOS(
    BluetoothAdapterChromeOS* adapter,
    const dbus::ObjectPath& object_path)
     : adapter_(adapter),
       object_path_(object_path),
       num_connecting_calls_(0),
       weak_ptr_factory_(this) {
 }
