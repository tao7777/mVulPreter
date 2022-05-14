 PepperDeviceEnumerationHostHelper::PepperDeviceEnumerationHostHelper(
     ppapi::host::ResourceHost* resource_host,
    base::WeakPtr<Delegate> delegate,
     PP_DeviceType_Dev device_type,
     const GURL& document_url)
     : resource_host_(resource_host),
      delegate_(delegate),
      device_type_(device_type),
      document_url_(document_url) {}
