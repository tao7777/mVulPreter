static int hid_reset_device(int sub_api, struct libusb_device_handle *dev_handle)
{
	struct windows_device_handle_priv *handle_priv = _device_handle_priv(dev_handle);
	HANDLE hid_handle;
	int current_interface;
	CHECK_HID_AVAILABLE;
	for (current_interface = 0; current_interface < USB_MAXINTERFACES; current_interface++) {
		hid_handle = handle_priv->interface_handle[current_interface].api_handle;
		if ((hid_handle != 0) && (hid_handle != INVALID_HANDLE_VALUE)) {
			HidD_FlushQueue(hid_handle);
		}
	}
	return LIBUSB_SUCCESS;
}
