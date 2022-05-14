static int _hid_class_request(struct hid_device_priv* dev, HANDLE hid_handle, int request_type,
							  int request, int value, int _index, void *data, struct windows_transfer_priv *tp,
							  size_t *size, OVERLAPPED* overlapped)
{
	int report_type = (value >> 8) & 0xFF;
	int report_id = value & 0xFF;
	if ( (LIBUSB_REQ_RECIPIENT(request_type) != LIBUSB_RECIPIENT_INTERFACE)
	  && (LIBUSB_REQ_RECIPIENT(request_type) != LIBUSB_RECIPIENT_DEVICE) )
		return LIBUSB_ERROR_INVALID_PARAM;
	if (LIBUSB_REQ_OUT(request_type) && request == HID_REQ_SET_REPORT)
		return _hid_set_report(dev, hid_handle, report_id, data, tp, size, overlapped, report_type);
	if (LIBUSB_REQ_IN(request_type) && request == HID_REQ_GET_REPORT)
		return _hid_get_report(dev, hid_handle, report_id, data, tp, size, overlapped, report_type);
	return LIBUSB_ERROR_INVALID_PARAM;
}
