static int get_valid_interface(struct libusb_device_handle *dev_handle, int api_id)
{
	struct windows_device_handle_priv *handle_priv = _device_handle_priv(dev_handle);
 	struct windows_device_priv *priv = _device_priv(dev_handle->dev);
 	int i;
 
	if ((api_id < USB_API_WINUSBX) || (api_id >= USB_API_MAX)) {
 		usbi_dbg("unsupported API ID");
 		return -1;
 	}

	for (i=0; i<USB_MAXINTERFACES; i++) {
		if ( (handle_priv->interface_handle[i].dev_handle != 0)
		  && (handle_priv->interface_handle[i].dev_handle != INVALID_HANDLE_VALUE)
		  && (handle_priv->interface_handle[i].api_handle != 0)
		  && (handle_priv->interface_handle[i].api_handle != INVALID_HANDLE_VALUE)
		  && (priv->usb_interface[i].apib->id == api_id) ) {
			return i;
		}
	}
	return -1;
}
