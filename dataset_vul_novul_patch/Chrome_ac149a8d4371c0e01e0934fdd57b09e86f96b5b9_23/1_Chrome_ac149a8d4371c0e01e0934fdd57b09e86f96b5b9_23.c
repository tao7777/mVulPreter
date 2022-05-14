static int hid_set_interface_altsetting(int sub_api, struct libusb_device_handle *dev_handle, int iface, int altsetting)
{
	struct libusb_context *ctx = DEVICE_CTX(dev_handle->dev);
	CHECK_HID_AVAILABLE;
	if (altsetting > 255) {
		return LIBUSB_ERROR_INVALID_PARAM;
	}
	if (altsetting != 0) {
		usbi_err(ctx, "set interface altsetting not supported for altsetting >0");
		return LIBUSB_ERROR_NOT_SUPPORTED;
	}
	return LIBUSB_SUCCESS;
}
