static int _hid_get_descriptor(struct hid_device_priv* dev, HANDLE hid_handle, int recipient,
							   int type, int _index, void *data, size_t *size)
{
	switch(type) {
	case LIBUSB_DT_DEVICE:
		usbi_dbg("LIBUSB_DT_DEVICE");
		return _hid_get_device_descriptor(dev, data, size);
	case LIBUSB_DT_CONFIG:
		usbi_dbg("LIBUSB_DT_CONFIG");
		if (!_index)
			return _hid_get_config_descriptor(dev, data, size);
		return LIBUSB_ERROR_INVALID_PARAM;
	case LIBUSB_DT_STRING:
		usbi_dbg("LIBUSB_DT_STRING");
		return _hid_get_string_descriptor(dev, _index, data, size);
	case LIBUSB_DT_HID:
		usbi_dbg("LIBUSB_DT_HID");
		if (!_index)
			return _hid_get_hid_descriptor(dev, data, size);
		return LIBUSB_ERROR_INVALID_PARAM;
	case LIBUSB_DT_REPORT:
		usbi_dbg("LIBUSB_DT_REPORT");
		if (!_index)
			return _hid_get_report_descriptor(dev, data, size);
		return LIBUSB_ERROR_INVALID_PARAM;
	case LIBUSB_DT_PHYSICAL:
		usbi_dbg("LIBUSB_DT_PHYSICAL");
		if (HidD_GetPhysicalDescriptor(hid_handle, data, (ULONG)*size))
			return LIBUSB_COMPLETED;
		return LIBUSB_ERROR_OTHER;
	}
	usbi_dbg("unsupported");
	return LIBUSB_ERROR_INVALID_PARAM;
}
