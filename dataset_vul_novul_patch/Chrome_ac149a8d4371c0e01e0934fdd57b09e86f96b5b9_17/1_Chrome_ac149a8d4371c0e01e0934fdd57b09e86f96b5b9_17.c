static int hid_copy_transfer_data(int sub_api, struct usbi_transfer *itransfer, uint32_t io_size) {
	struct libusb_transfer *transfer = USBI_TRANSFER_TO_LIBUSB_TRANSFER(itransfer);
	struct libusb_context *ctx = DEVICE_CTX(transfer->dev_handle->dev);
	struct windows_transfer_priv *transfer_priv = usbi_transfer_get_os_priv(itransfer);
	int r = LIBUSB_TRANSFER_COMPLETED;
	uint32_t corrected_size = io_size;
	if (transfer_priv->hid_buffer != NULL) {
		if (transfer_priv->hid_dest != NULL) {	// Data readout
			if (corrected_size > transfer_priv->hid_expected_size) {
				usbi_err(ctx, "OVERFLOW!");
				corrected_size = (uint32_t)transfer_priv->hid_expected_size;
				r = LIBUSB_TRANSFER_OVERFLOW;
			}
			if (transfer_priv->hid_buffer[0] == 0) {
				corrected_size--;
				memcpy(transfer_priv->hid_dest, transfer_priv->hid_buffer+1, corrected_size);
			} else {
				memcpy(transfer_priv->hid_dest, transfer_priv->hid_buffer, corrected_size);
			}
			transfer_priv->hid_dest = NULL;
		}
		safe_free(transfer_priv->hid_buffer);
	}
	itransfer->transferred += corrected_size;
	return r;
}
