static void hid_close(int sub_api, struct libusb_device_handle *dev_handle)
{
	struct windows_device_priv *priv = _device_priv(dev_handle->dev);
	struct windows_device_handle_priv *handle_priv = _device_handle_priv(dev_handle);
	HANDLE file_handle;
	int i;
	if (!api_hid_available)
		return;
	for (i = 0; i < USB_MAXINTERFACES; i++) {
		if (priv->usb_interface[i].apib->id == USB_API_HID) {
			file_handle = handle_priv->interface_handle[i].api_handle;
			if ( (file_handle != 0) && (file_handle != INVALID_HANDLE_VALUE)) {
				CloseHandle(file_handle);
			}
		}
	}
}
