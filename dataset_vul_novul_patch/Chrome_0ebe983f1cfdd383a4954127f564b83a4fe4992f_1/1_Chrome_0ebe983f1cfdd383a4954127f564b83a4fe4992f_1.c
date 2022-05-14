void UsbDeviceImpl::OpenInterface(int interface_id,
                                  const OpenCallback& callback) {
  chromeos::PermissionBrokerClient* client =
      chromeos::DBusThreadManager::Get()->GetPermissionBrokerClient();
  DCHECK(client) << "Could not get permission broker client.";
  client->RequestPathAccess(
      device_path_, interface_id,
      base::Bind(&UsbDeviceImpl::OnPathAccessRequestComplete, this, callback));
}
