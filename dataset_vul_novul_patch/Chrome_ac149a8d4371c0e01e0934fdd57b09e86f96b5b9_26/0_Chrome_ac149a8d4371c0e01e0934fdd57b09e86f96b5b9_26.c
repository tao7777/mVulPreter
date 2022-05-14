static int set_composite_interface(struct libusb_context* ctx, struct libusb_device* dev,
							char* dev_interface_path, char* device_id, int api, int sub_api)
{
	unsigned i;
	struct windows_device_priv *priv = _device_priv(dev);
	int interface_number;

	if (priv->apib->id != USB_API_COMPOSITE) {
		usbi_err(ctx, "program assertion failed: '%s' is not composite", device_id);
		return LIBUSB_ERROR_NO_DEVICE;
	}

	interface_number = 0;
	for (i=0; device_id[i] != 0; ) {
		if ( (device_id[i++] == 'M') && (device_id[i++] == 'I')
		  && (device_id[i++] == '_') ) {
			interface_number = (device_id[i++] - '0')*10;
			interface_number += device_id[i] - '0';
			break;
		}
	}

	if (device_id[i] == 0) {
		usbi_warn(ctx, "failure to read interface number for %s. Using default value %d",
			device_id, interface_number);
 	}
 
 	if (priv->usb_interface[interface_number].path != NULL) {
 	}
 
 	usbi_dbg("interface[%d] = %s", interface_number, dev_interface_path);
 	priv->usb_interface[interface_number].path = dev_interface_path;
 	priv->usb_interface[interface_number].apib = &usb_api_backend[api];
 	priv->usb_interface[interface_number].sub_api = sub_api;
