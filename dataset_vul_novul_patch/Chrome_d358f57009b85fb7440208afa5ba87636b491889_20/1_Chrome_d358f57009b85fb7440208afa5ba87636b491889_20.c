void BluetoothDeviceChromeOS::RequestAuthorization(
    const dbus::ObjectPath& device_path,
    const ConfirmationCallback& callback) {
  callback.Run(CANCELLED);
}
