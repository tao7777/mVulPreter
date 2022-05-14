void BluetoothDeviceChromeOS::DisplayPinCode(
    const dbus::ObjectPath& device_path,
    const std::string& pincode) {
  DCHECK(agent_.get());
  DCHECK(device_path == object_path_);
  VLOG(1) << object_path_.value() << ": DisplayPinCode: " << pincode;
  UMA_HISTOGRAM_ENUMERATION("Bluetooth.PairingMethod",
                            UMA_PAIRING_METHOD_DISPLAY_PINCODE,
                            UMA_PAIRING_METHOD_COUNT);
  DCHECK(pairing_delegate_);
  pairing_delegate_->DisplayPinCode(this, pincode);
  pairing_delegate_used_ = true;
}
