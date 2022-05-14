void BluetoothDeviceChromeOS::RequestConfirmation(
    const dbus::ObjectPath& device_path,
    uint32 passkey,
    const ConfirmationCallback& callback) {
  DCHECK(agent_.get());
  DCHECK(device_path == object_path_);
  VLOG(1) << object_path_.value() << ": RequestConfirmation: " << passkey;
  UMA_HISTOGRAM_ENUMERATION("Bluetooth.PairingMethod",
                            UMA_PAIRING_METHOD_CONFIRM_PASSKEY,
                            UMA_PAIRING_METHOD_COUNT);
  DCHECK(pairing_delegate_);
  DCHECK(confirmation_callback_.is_null());
  confirmation_callback_ = callback;
  pairing_delegate_->ConfirmPasskey(this, passkey);
  pairing_delegate_used_ = true;
}
