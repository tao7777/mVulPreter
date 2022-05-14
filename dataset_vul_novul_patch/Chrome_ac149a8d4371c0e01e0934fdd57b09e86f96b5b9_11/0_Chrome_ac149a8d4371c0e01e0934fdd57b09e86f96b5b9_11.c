static int composite_open(int sub_api, struct libusb_device_handle *dev_handle)
{
 	struct windows_device_priv *priv = _device_priv(dev_handle->dev);
 	int r = LIBUSB_ERROR_NOT_FOUND;
 	uint8_t i;
	bool available[SUB_API_MAX] = {0};
 
 	for (i=0; i<USB_MAXINTERFACES; i++) {
 		switch (priv->usb_interface[i].apib->id) {
 		case USB_API_WINUSBX:
 			if (priv->usb_interface[i].sub_api != SUB_API_NOTSET)
 				available[priv->usb_interface[i].sub_api] = true;
 			break;
 		default:
 			break;
 		}
	}

	for (i=0; i<SUB_API_MAX; i++) {	// WinUSB-like drivers
		if (available[i]) {
			r = usb_api_backend[USB_API_WINUSBX].open(i, dev_handle);
			if (r != LIBUSB_SUCCESS) {
				return r;
 			}
 		}
 	}
 	return r;
 }
