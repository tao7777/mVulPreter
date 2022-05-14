static int _hid_get_string_descriptor(struct hid_device_priv* dev, int _index,
									  void *data, size_t *size)
{
	void *tmp = NULL;
	size_t tmp_size = 0;
	int i;
	/* language ID, EN-US */
	char string_langid[] = {
		0x09,
		0x04
	};
	if ((*size < 2) || (*size > 255)) {
		return LIBUSB_ERROR_OVERFLOW;
	}
	if (_index == 0) {
		tmp = string_langid;
		tmp_size = sizeof(string_langid)+2;
	} else {
		for (i=0; i<3; i++) {
			if (_index == (dev->string_index[i])) {
				tmp = dev->string[i];
				tmp_size = (_hid_wcslen(dev->string[i])+1) * sizeof(WCHAR);
				break;
			}
		}
		if (i == 3) {	// not found
			return LIBUSB_ERROR_INVALID_PARAM;
		}
	}
	if(!tmp_size) {
		return LIBUSB_ERROR_INVALID_PARAM;
	}
	if (tmp_size < *size) {
		*size = tmp_size;
	}
	((uint8_t*)data)[0] = (uint8_t)*size;
	((uint8_t*)data)[1] = LIBUSB_DT_STRING;
	memcpy((uint8_t*)data+2, tmp, *size-2);
	return LIBUSB_COMPLETED;
}
