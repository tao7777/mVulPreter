void BluetoothDeviceChromeOS::DisplayPasskey(
    const dbus::ObjectPath& device_path,
    uint32 passkey,
    uint16 entered) {
  DCHECK(agent_.get());
  DCHECK(device_path == object_path_);
  VLOG(1) << object_path_.value() << ": DisplayPasskey: " << passkey
          << " (" << entered << " entered)";
  if (entered == 0)
    UMA_HISTOGRAM_ENUMERATION("Bluetooth.PairingMethod",
                              UMA_PAIRING_METHOD_DISPLAY_PASSKEY,
                              UMA_PAIRING_METHOD_COUNT);
  DCHECK(pairing_delegate_);
  if (entered == 0)
    pairing_delegate_->DisplayPasskey(this, passkey);
  pairing_delegate_->KeysEntered(this, entered);
  pairing_delegate_used_ = true;
}
