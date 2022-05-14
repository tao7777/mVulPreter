static int hid_claim_interface(int sub_api, struct libusb_device_handle *dev_handle, int iface)
{
	struct windows_device_handle_priv *handle_priv = _device_handle_priv(dev_handle);
	struct windows_device_priv *priv = _device_priv(dev_handle->dev);
	CHECK_HID_AVAILABLE;
	if (priv->usb_interface[iface].path == NULL) {
		return LIBUSB_ERROR_NOT_FOUND;	// invalid iface
	}
	if (handle_priv->interface_handle[iface].dev_handle == INTERFACE_CLAIMED) {
		return LIBUSB_ERROR_BUSY;	// already claimed
	}
	handle_priv->interface_handle[iface].dev_handle = INTERFACE_CLAIMED;
	usbi_dbg("claimed interface %d", iface);
	handle_priv->active_interface = iface;
	return LIBUSB_SUCCESS;
}
