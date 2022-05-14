static int _hid_get_report(struct hid_device_priv* dev, HANDLE hid_handle, int id, void *data,
						   struct windows_transfer_priv *tp, size_t *size, OVERLAPPED* overlapped,
						   int report_type)
{
	uint8_t *buf;
	DWORD ioctl_code, read_size, expected_size = (DWORD)*size;
	int r = LIBUSB_SUCCESS;
	if (tp->hid_buffer != NULL) {
		usbi_dbg("program assertion failed: hid_buffer is not NULL");
	}
	if ((*size == 0) || (*size > MAX_HID_REPORT_SIZE)) {
		usbi_dbg("invalid size (%d)", *size);
		return LIBUSB_ERROR_INVALID_PARAM;
	}
	switch (report_type) {
		case HID_REPORT_TYPE_INPUT:
			ioctl_code = IOCTL_HID_GET_INPUT_REPORT;
			break;
		case HID_REPORT_TYPE_FEATURE:
			ioctl_code = IOCTL_HID_GET_FEATURE;
			break;
		default:
			usbi_dbg("unknown HID report type %d", report_type);
			return LIBUSB_ERROR_INVALID_PARAM;
	}
	buf = (uint8_t*)calloc(expected_size+1, 1);
	if (buf == NULL) {
		return LIBUSB_ERROR_NO_MEM;
	}
	buf[0] = (uint8_t)id;	// Must be set always
	usbi_dbg("report ID: 0x%02X", buf[0]);
	tp->hid_expected_size = expected_size;
	read_size = expected_size;
	if (!DeviceIoControl(hid_handle, ioctl_code, buf, expected_size+1,
		buf, expected_size+1, &read_size, overlapped)) {
		if (GetLastError() != ERROR_IO_PENDING) {
			usbi_dbg("Failed to Read HID Report: %s", windows_error_str(0));
			safe_free(buf);
			return LIBUSB_ERROR_IO;
		}
		tp->hid_buffer = buf;
		tp->hid_dest = (uint8_t*)data; // copy dest, as not necessarily the start of the transfer buffer
		return LIBUSB_SUCCESS;
	}
	if (read_size == 0) {
		usbi_warn(NULL, "program assertion failed - read completed synchronously, but no data was read");
		*size = 0;
	} else {
		if (buf[0] != id) {
			usbi_warn(NULL, "mismatched report ID (data is %02X, parameter is %02X)", buf[0], id);
		}
		if ((size_t)read_size > expected_size) {
			r = LIBUSB_ERROR_OVERFLOW;
			usbi_dbg("OVERFLOW!");
		} else {
			r = LIBUSB_COMPLETED;
		}
		*size = MIN((size_t)read_size, *size);
		if (id == 0) {
			memcpy(data, buf+1, *size);
		} else {
			memcpy(data, buf, *size);
		}
	}
	safe_free(buf);
	return r;
}
