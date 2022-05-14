void UsbFindDevicesFunction::OnGetDevicesComplete(
    const std::vector<scoped_refptr<UsbDevice>>& devices) {
  result_.reset(new base::ListValue());
  barrier_ = base::BarrierClosure(
      devices.size(), base::Bind(&UsbFindDevicesFunction::OpenComplete, this));

  for (const scoped_refptr<UsbDevice>& device : devices) {
    if (device->vendor_id() != vendor_id_ ||
         device->product_id() != product_id_) {
       barrier_.Run();
     } else {
      device->OpenInterface(
          interface_id_,
          base::Bind(&UsbFindDevicesFunction::OnDeviceOpened, this));
     }
   }
 }
