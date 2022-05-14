  ScopedRequest(PepperDeviceEnumerationHostHelper* owner,
                const Delegate::EnumerateDevicesCallback& callback)
      : owner_(owner),
        callback_(callback),
        requested_(false),
        request_id_(0),
        sync_call_(false) {
    if (!owner_->document_url_.is_valid())
      return;

    requested_ = true;

     sync_call_ = true;
     request_id_ = owner_->delegate_->EnumerateDevices(
         owner_->device_type_,
         owner_->document_url_,
        base::Bind(&ScopedRequest::EnumerateDevicesCallbackBody, AsWeakPtr()));
    sync_call_ = false;
   }
