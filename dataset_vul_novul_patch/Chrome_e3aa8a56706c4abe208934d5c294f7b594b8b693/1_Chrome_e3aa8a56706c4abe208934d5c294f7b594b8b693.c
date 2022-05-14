bool UsbChooserContext::HasDevicePermission(
    const GURL& requesting_origin,
    const GURL& embedding_origin,
    const device::mojom::UsbDeviceInfo& device_info) {
   if (UsbBlocklist::Get().IsExcluded(device_info))
     return false;
 
   if (!CanRequestObjectPermission(requesting_origin, embedding_origin))
     return false;
 
  auto it = ephemeral_devices_.find(
      std::make_pair(requesting_origin, embedding_origin));
  if (it != ephemeral_devices_.end() &&
      base::ContainsKey(it->second, device_info.guid)) {
    return true;
  }

  std::vector<std::unique_ptr<base::DictionaryValue>> device_list =
      GetGrantedObjects(requesting_origin, embedding_origin);
  for (const std::unique_ptr<base::DictionaryValue>& device_dict :
       device_list) {
    int vendor_id;
    int product_id;
    base::string16 serial_number;
    if (device_dict->GetInteger(kVendorIdKey, &vendor_id) &&
        device_info.vendor_id == vendor_id &&
        device_dict->GetInteger(kProductIdKey, &product_id) &&
        device_info.product_id == product_id &&
        device_dict->GetString(kSerialNumberKey, &serial_number) &&
        device_info.serial_number == serial_number) {
      return true;
    }
  }

  return false;
}
