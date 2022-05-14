  PepperDeviceEnumerationHostHelperTest()
       : ppapi_host_(&sink_, ppapi::PpapiPermissions()),
         resource_host_(&ppapi_host_, 12345, 67890),
         device_enumeration_(&resource_host_,
                            &delegate_,
                             PP_DEVICETYPE_DEV_AUDIOCAPTURE,
                             GURL("http://example.com")) {}
