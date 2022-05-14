static int _hid_set_report(struct hid_device_priv* dev, HANDLE hid_handle, int id, void *data,
						   struct windows_transfer_priv *tp, size_t *size, OVERLAPPED* overlapped,
						   int report_type)
{
	uint8_t *buf = NULL;
	DWORD ioctl_code, write_size= (DWORD)*size;
	if (tp->hid_buffer != NULL) {
		usbi_dbg("program assertion failed: hid_buffer is not NULL");
	}
	if ((*size == 0) || (*size > MAX_HID_REPORT_SIZE)) {
		usbi_dbg("invalid size (%d)", *size);
		return LIBUSB_ERROR_INVALID_PARAM;
	}
	switch (report_type) {
		case HID_REPORT_TYPE_OUTPUT:
			ioctl_code = IOCTL_HID_SET_OUTPUT_REPORT;
			break;
		case HID_REPORT_TYPE_FEATURE:
			ioctl_code = IOCTL_HID_SET_FEATURE;
			break;
		default:
			usbi_dbg("unknown HID report type %d", report_type);
			return LIBUSB_ERROR_INVALID_PARAM;
	}
	usbi_dbg("report ID: 0x%02X", id);
	if (id == 0) {
		write_size++;
	}
	buf = (uint8_t*) malloc(write_size);
	if (buf == NULL) {
		return LIBUSB_ERROR_NO_MEM;
	}
	if (id == 0) {
		buf[0] = 0;
		memcpy(buf + 1, data, *size);
	} else {
		memcpy(buf, data, *size);
		if (buf[0] != id) {
			usbi_warn(NULL, "mismatched report ID (data is %02X, parameter is %02X)", buf[0], id);
		}
	}
	if (!DeviceIoControl(hid_handle, ioctl_code, buf, write_size,
		buf, write_size, &write_size, overlapped)) {
		if (GetLastError() != ERROR_IO_PENDING) {
			usbi_dbg("Failed to Write HID Output Report: %s", windows_error_str(0));
			safe_free(buf);
			return LIBUSB_ERROR_IO;
		}
		tp->hid_buffer = buf;
		tp->hid_dest = NULL;
		return LIBUSB_SUCCESS;
	}
	*size = write_size;
	if (write_size == 0) {
		usbi_dbg("program assertion failed - write completed synchronously, but no data was written");
	}
	safe_free(buf);
	return LIBUSB_COMPLETED;
}
