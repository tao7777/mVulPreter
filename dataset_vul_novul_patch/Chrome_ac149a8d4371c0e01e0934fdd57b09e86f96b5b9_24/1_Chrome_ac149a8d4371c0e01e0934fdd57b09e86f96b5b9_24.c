static int hid_submit_bulk_transfer(int sub_api, struct usbi_transfer *itransfer) {
	struct libusb_transfer *transfer = USBI_TRANSFER_TO_LIBUSB_TRANSFER(itransfer);
	struct windows_transfer_priv *transfer_priv = (struct windows_transfer_priv*)usbi_transfer_get_os_priv(itransfer);
	struct libusb_context *ctx = DEVICE_CTX(transfer->dev_handle->dev);
	struct windows_device_handle_priv *handle_priv = _device_handle_priv(transfer->dev_handle);
	struct windows_device_priv *priv = _device_priv(transfer->dev_handle->dev);
	struct winfd wfd;
	HANDLE hid_handle;
	bool direction_in, ret;
	int current_interface, length;
	DWORD size;
	int r = LIBUSB_SUCCESS;
	CHECK_HID_AVAILABLE;
	transfer_priv->pollable_fd = INVALID_WINFD;
	transfer_priv->hid_dest = NULL;
	safe_free(transfer_priv->hid_buffer);
	current_interface = interface_by_endpoint(priv, handle_priv, transfer->endpoint);
	if (current_interface < 0) {
		usbi_err(ctx, "unable to match endpoint to an open interface - cancelling transfer");
		return LIBUSB_ERROR_NOT_FOUND;
	}
	usbi_dbg("matched endpoint %02X with interface %d", transfer->endpoint, current_interface);
	hid_handle = handle_priv->interface_handle[current_interface].api_handle;
	direction_in = transfer->endpoint & LIBUSB_ENDPOINT_IN;
	wfd = usbi_create_fd(hid_handle, direction_in?RW_READ:RW_WRITE, NULL, NULL);
	if (wfd.fd < 0) {
		return LIBUSB_ERROR_NO_MEM;
	}
	if ( ((direction_in) && (!priv->hid->uses_report_ids[0]))
	  || ((!direction_in) && (!priv->hid->uses_report_ids[1])) ) {
		length = transfer->length+1;
	} else {
		length = transfer->length;
	}
	transfer_priv->hid_buffer = (uint8_t*)calloc(length+1, 1);
	if (transfer_priv->hid_buffer == NULL) {
		return LIBUSB_ERROR_NO_MEM;
	}
	transfer_priv->hid_expected_size = length;
	if (direction_in) {
		transfer_priv->hid_dest = transfer->buffer;
		usbi_dbg("reading %d bytes (report ID: 0x00)", length);
		ret = ReadFile(wfd.handle, transfer_priv->hid_buffer, length+1, &size, wfd.overlapped);
	} else {
		if (!priv->hid->uses_report_ids[1]) {
			memcpy(transfer_priv->hid_buffer+1, transfer->buffer, transfer->length);
		} else {
			memcpy(transfer_priv->hid_buffer, transfer->buffer, transfer->length);
		}
		usbi_dbg("writing %d bytes (report ID: 0x%02X)", length, transfer_priv->hid_buffer[0]);
		ret = WriteFile(wfd.handle, transfer_priv->hid_buffer, length, &size, wfd.overlapped);
	}
	if (!ret) {
		if (GetLastError() != ERROR_IO_PENDING) {
			usbi_err(ctx, "HID transfer failed: %s", windows_error_str(0));
			usbi_free_fd(&wfd);
			safe_free(transfer_priv->hid_buffer);
			return LIBUSB_ERROR_IO;
		}
	} else {
		if (!direction_in) {
			safe_free(transfer_priv->hid_buffer);
		}
		if (size == 0) {
			usbi_err(ctx, "program assertion failed - no data was transferred");
			size = 1;
		}
		if (size > (size_t)length) {
			usbi_err(ctx, "OVERFLOW!");
			r = LIBUSB_ERROR_OVERFLOW;
		}
		wfd.overlapped->Internal = STATUS_COMPLETED_SYNCHRONOUSLY;
		wfd.overlapped->InternalHigh = size;
	}
	transfer_priv->pollable_fd = wfd;
	transfer_priv->interface_number = (uint8_t)current_interface;
	return r;
}
