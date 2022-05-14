void BluetoothDeviceChromeOS::RequestPasskey(
    const dbus::ObjectPath& device_path,
    const PasskeyCallback& callback) {
  DCHECK(agent_.get());
  DCHECK(device_path == object_path_);
  VLOG(1) << object_path_.value() << ": RequestPasskey";
  UMA_HISTOGRAM_ENUMERATION("Bluetooth.PairingMethod",
                            UMA_PAIRING_METHOD_REQUEST_PASSKEY,
                            UMA_PAIRING_METHOD_COUNT);
  DCHECK(pairing_delegate_);
  DCHECK(passkey_callback_.is_null());
  passkey_callback_ = callback;
  pairing_delegate_->RequestPasskey(this);
  pairing_delegate_used_ = true;
}
