ExtensionFunction::ResponseAction UsbFindDevicesFunction::Run() {
  scoped_ptr<extensions::core_api::usb::FindDevices::Params> parameters =
      FindDevices::Params::Create(*args_);
  EXTENSION_FUNCTION_VALIDATE(parameters.get());
 
   vendor_id_ = parameters->options.vendor_id;
   product_id_ = parameters->options.product_id;
  interface_id_ = parameters->options.interface_id.get()
                      ? *parameters->options.interface_id.get()
                      : UsbDevicePermissionData::ANY_INTERFACE;
  UsbDevicePermission::CheckParam param(vendor_id_, product_id_, interface_id_);
   if (!extension()->permissions_data()->CheckAPIPermissionWithParam(
           APIPermission::kUsbDevice, &param)) {
     return RespondNow(Error(kErrorPermissionDenied));
  }

  UsbService* service = device::DeviceClient::Get()->GetUsbService();
  if (!service) {
    return RespondNow(Error(kErrorInitService));
  }

  service->GetDevices(
      base::Bind(&UsbFindDevicesFunction::OnGetDevicesComplete, this));
  return RespondLater();
}
