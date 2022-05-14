void OnGetDevicesOnServiceThread(
    const std::vector<UsbDeviceFilter>& filters,
    const base::Callback<void(mojo::Array<DeviceInfoPtr>)>& callback,
    scoped_refptr<base::TaskRunner> callback_task_runner,
     const std::vector<scoped_refptr<UsbDevice>>& devices) {
   mojo::Array<DeviceInfoPtr> mojo_devices(0);
   for (size_t i = 0; i < devices.size(); ++i) {
    if (UsbDeviceFilter::MatchesAny(devices[i], filters) || filters.empty())
       mojo_devices.push_back(DeviceInfo::From(*devices[i]));
   }
   callback_task_runner->PostTask(
      FROM_HERE, base::Bind(callback, base::Passed(&mojo_devices)));
}
