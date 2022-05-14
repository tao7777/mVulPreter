static int hid_submit_control_transfer(int sub_api, struct usbi_transfer *itransfer)
{
	struct libusb_transfer *transfer = USBI_TRANSFER_TO_LIBUSB_TRANSFER(itransfer);
	struct windows_transfer_priv *transfer_priv = (struct windows_transfer_priv*)usbi_transfer_get_os_priv(itransfer);
	struct windows_device_handle_priv *handle_priv = _device_handle_priv(transfer->dev_handle);
	struct windows_device_priv *priv = _device_priv(transfer->dev_handle->dev);
	struct libusb_context *ctx = DEVICE_CTX(transfer->dev_handle->dev);
	WINUSB_SETUP_PACKET *setup = (WINUSB_SETUP_PACKET *) transfer->buffer;
	HANDLE hid_handle;
	struct winfd wfd;
	int current_interface, config;
	size_t size;
	int r = LIBUSB_ERROR_INVALID_PARAM;
	CHECK_HID_AVAILABLE;
	transfer_priv->pollable_fd = INVALID_WINFD;
	safe_free(transfer_priv->hid_buffer);
	transfer_priv->hid_dest = NULL;
	size = transfer->length - LIBUSB_CONTROL_SETUP_SIZE;
	if (size > MAX_CTRL_BUFFER_LENGTH) {
		return LIBUSB_ERROR_INVALID_PARAM;
	}
	current_interface = get_valid_interface(transfer->dev_handle, USB_API_HID);
	if (current_interface < 0) {
		if (auto_claim(transfer, &current_interface, USB_API_HID) != LIBUSB_SUCCESS) {
			return LIBUSB_ERROR_NOT_FOUND;
		}
	}
	usbi_dbg("will use interface %d", current_interface);
	hid_handle = handle_priv->interface_handle[current_interface].api_handle;
	wfd = usbi_create_fd(hid_handle, RW_READ, NULL, NULL);
	if (wfd.fd < 0) {
		return LIBUSB_ERROR_NOT_FOUND;
	}
	switch(LIBUSB_REQ_TYPE(setup->request_type)) {
	case LIBUSB_REQUEST_TYPE_STANDARD:
		switch(setup->request) {
		case LIBUSB_REQUEST_GET_DESCRIPTOR:
			r = _hid_get_descriptor(priv->hid, wfd.handle, LIBUSB_REQ_RECIPIENT(setup->request_type),
				(setup->value >> 8) & 0xFF, setup->value & 0xFF, transfer->buffer + LIBUSB_CONTROL_SETUP_SIZE, &size);
			break;
		case LIBUSB_REQUEST_GET_CONFIGURATION:
			r = windows_get_configuration(transfer->dev_handle, &config);
			if (r == LIBUSB_SUCCESS) {
				size = 1;
				((uint8_t*)transfer->buffer)[LIBUSB_CONTROL_SETUP_SIZE] = (uint8_t)config;
				r = LIBUSB_COMPLETED;
			}
			break;
		case LIBUSB_REQUEST_SET_CONFIGURATION:
			if (setup->value == priv->active_config) {
				r = LIBUSB_COMPLETED;
			} else {
				usbi_warn(ctx, "cannot set configuration other than the default one");
				r = LIBUSB_ERROR_INVALID_PARAM;
			}
			break;
		case LIBUSB_REQUEST_GET_INTERFACE:
			size = 1;
			((uint8_t*)transfer->buffer)[LIBUSB_CONTROL_SETUP_SIZE] = 0;
			r = LIBUSB_COMPLETED;
			break;
		case LIBUSB_REQUEST_SET_INTERFACE:
			r = hid_set_interface_altsetting(0, transfer->dev_handle, setup->index, setup->value);
			if (r == LIBUSB_SUCCESS) {
				r = LIBUSB_COMPLETED;
			}
			break;
		default:
			usbi_warn(ctx, "unsupported HID control request");
			r = LIBUSB_ERROR_INVALID_PARAM;
			break;
		}
		break;
	case LIBUSB_REQUEST_TYPE_CLASS:
		r =_hid_class_request(priv->hid, wfd.handle, setup->request_type, setup->request, setup->value,
			setup->index, transfer->buffer + LIBUSB_CONTROL_SETUP_SIZE, transfer_priv,
			&size, wfd.overlapped);
		break;
	default:
		usbi_warn(ctx, "unsupported HID control request");
		r = LIBUSB_ERROR_INVALID_PARAM;
		break;
	}
	if (r == LIBUSB_COMPLETED) {
		wfd.overlapped->Internal = STATUS_COMPLETED_SYNCHRONOUSLY;
		wfd.overlapped->InternalHigh = (DWORD)size;
		r = LIBUSB_SUCCESS;
	}
	if (r == LIBUSB_SUCCESS) {
		transfer_priv->pollable_fd = wfd;
		transfer_priv->interface_number = (uint8_t)current_interface;
	} else {
		usbi_free_fd(&wfd);
	}
	return r;
}
