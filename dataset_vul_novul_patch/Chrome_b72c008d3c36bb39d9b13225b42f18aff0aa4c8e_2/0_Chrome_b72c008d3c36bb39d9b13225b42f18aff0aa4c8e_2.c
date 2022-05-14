  const UsbDeviceHandle::TransferCallback& callback() const {
  UsbDeviceHandle::TransferCallback GetCallback() {
    return base::Bind(&TestCompletionCallback::SetResult,
                      base::Unretained(this));
   }
