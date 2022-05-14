static int hid_clear_halt(int sub_api, struct libusb_device_handle *dev_handle, unsigned char endpoint)
{
	struct libusb_context *ctx = DEVICE_CTX(dev_handle->dev);
	struct windows_device_handle_priv *handle_priv = _device_handle_priv(dev_handle);
	struct windows_device_priv *priv = _device_priv(dev_handle->dev);
	HANDLE hid_handle;
	int current_interface;
	CHECK_HID_AVAILABLE;
	current_interface = interface_by_endpoint(priv, handle_priv, endpoint);
	if (current_interface < 0) {
		usbi_err(ctx, "unable to match endpoint to an open interface - cannot clear");
		return LIBUSB_ERROR_NOT_FOUND;
	}
	usbi_dbg("matched endpoint %02X with interface %d", endpoint, current_interface);
	hid_handle = handle_priv->interface_handle[current_interface].api_handle;
	if (!HidD_FlushQueue(hid_handle)) {
		usbi_err(ctx, "Flushing of HID queue failed: %s", windows_error_str(0));
		return LIBUSB_ERROR_NO_DEVICE;
	}
	return LIBUSB_SUCCESS;
}
