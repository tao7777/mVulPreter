void UsbDeviceImpl::OnPathAccessRequestComplete(const OpenCallback& callback,
                                                bool success) {
  if (success) {
    blocking_task_runner_->PostTask(
        FROM_HERE,
        base::Bind(&UsbDeviceImpl::OpenOnBlockingThread, this, callback));
  } else {
    chromeos::PermissionBrokerClient* client =
        chromeos::DBusThreadManager::Get()->GetPermissionBrokerClient();
    DCHECK(client) << "Could not get permission broker client.";
    client->OpenPath(
        device_path_,
        base::Bind(&UsbDeviceImpl::OnOpenRequestComplete, this, callback));
  }
}
