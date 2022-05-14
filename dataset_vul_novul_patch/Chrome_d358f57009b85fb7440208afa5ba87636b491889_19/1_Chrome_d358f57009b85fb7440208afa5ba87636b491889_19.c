void BluetoothDeviceChromeOS::Release() {
  DCHECK(agent_.get());
  DCHECK(pairing_delegate_);
  VLOG(1) << object_path_.value() << ": Release";
  pincode_callback_.Reset();
  passkey_callback_.Reset();
  confirmation_callback_.Reset();
  UnregisterAgent();
}
