BluetoothDeviceChromeOS::BluetoothDeviceChromeOS(
    BluetoothAdapterChromeOS* adapter,
    const dbus::ObjectPath& object_path)
     : adapter_(adapter),
       object_path_(object_path),
       num_connecting_calls_(0),
      pairing_delegate_(NULL),
      pairing_delegate_used_(false),
       weak_ptr_factory_(this) {
 }
