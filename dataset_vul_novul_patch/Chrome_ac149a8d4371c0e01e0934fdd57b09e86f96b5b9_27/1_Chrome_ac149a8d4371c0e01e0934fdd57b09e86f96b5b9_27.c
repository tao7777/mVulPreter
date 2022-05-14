static int set_hid_interface(struct libusb_context* ctx, struct libusb_device* dev,
							char* dev_interface_path)
{
	int i;
	struct windows_device_priv *priv = _device_priv(dev);
	if (priv->hid == NULL) {
		usbi_err(ctx, "program assertion failed: parent is not HID");
		return LIBUSB_ERROR_NO_DEVICE;
	}
	if (priv->hid->nb_interfaces == USB_MAXINTERFACES) {
		usbi_err(ctx, "program assertion failed: max USB interfaces reached for HID device");
		return LIBUSB_ERROR_NO_DEVICE;
	}
	for (i=0; i<priv->hid->nb_interfaces; i++) {
		if (safe_strcmp(priv->usb_interface[i].path, dev_interface_path) == 0) {
			usbi_dbg("interface[%d] already set to %s", i, dev_interface_path);
			return LIBUSB_SUCCESS;
		}
	}
 
	priv->usb_interface[priv->hid->nb_interfaces].path = dev_interface_path;
	priv->usb_interface[priv->hid->nb_interfaces].apib = &usb_api_backend[USB_API_HID];
	usbi_dbg("interface[%d] = %s", priv->hid->nb_interfaces, dev_interface_path);
	priv->hid->nb_interfaces++;
 	return LIBUSB_SUCCESS;
 }
