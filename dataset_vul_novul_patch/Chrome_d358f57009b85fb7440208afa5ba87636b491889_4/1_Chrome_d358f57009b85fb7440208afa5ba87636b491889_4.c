void BluetoothDeviceChromeOS::Cancel() {
  DCHECK(agent_.get());
  VLOG(1) << object_path_.value() << ": Cancel";
  DCHECK(pairing_delegate_);
  pairing_delegate_->DismissDisplayOrConfirm();
}
