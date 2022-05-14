void BluetoothDeviceChromeOS::AuthorizeService(
    const dbus::ObjectPath& device_path,
    const std::string& uuid,
    const ConfirmationCallback& callback) {
  callback.Run(CANCELLED);
}
