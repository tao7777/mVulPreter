static int _hid_get_report_descriptor(struct hid_device_priv* dev, void *data, size_t *size)
{
	uint8_t d[MAX_HID_DESCRIPTOR_SIZE];
	size_t i = 0;
	/* usage page (0xFFA0 == vendor defined) */
	d[i++] = 0x06; d[i++] = 0xA0; d[i++] = 0xFF;
	/* usage (vendor defined) */
	d[i++] = 0x09; d[i++] = 0x01;
	/* start collection (application) */
	d[i++] = 0xA1; d[i++] = 0x01;
	/* input report */
	if (dev->input_report_size) {
		/* usage (vendor defined) */
		d[i++] = 0x09; d[i++] = 0x01;
		/* logical minimum (0) */
		d[i++] = 0x15; d[i++] = 0x00;
		/* logical maximum (255) */
		d[i++] = 0x25; d[i++] = 0xFF;
		/* report size (8 bits) */
		d[i++] = 0x75; d[i++] = 0x08;
		/* report count */
		d[i++] = 0x95; d[i++] = (uint8_t)dev->input_report_size - 1;
		/* input (data, variable, absolute) */
		d[i++] = 0x81; d[i++] = 0x00;
	}
	/* output report */
	if (dev->output_report_size) {
		/* usage (vendor defined) */
		d[i++] = 0x09; d[i++] = 0x02;
		/* logical minimum (0) */
		d[i++] = 0x15; d[i++] = 0x00;
		/* logical maximum (255) */
		d[i++] = 0x25; d[i++] = 0xFF;
		/* report size (8 bits) */
		d[i++] = 0x75; d[i++] = 0x08;
		/* report count */
		d[i++] = 0x95; d[i++] = (uint8_t)dev->output_report_size - 1;
		/* output (data, variable, absolute) */
		d[i++] = 0x91; d[i++] = 0x00;
	}
	/* feature report */
	if (dev->feature_report_size) {
		/* usage (vendor defined) */
		d[i++] = 0x09; d[i++] = 0x03;
		/* logical minimum (0) */
		d[i++] = 0x15; d[i++] = 0x00;
		/* logical maximum (255) */
		d[i++] = 0x25; d[i++] = 0xFF;
		/* report size (8 bits) */
		d[i++] = 0x75; d[i++] = 0x08;
		/* report count */
		d[i++] = 0x95; d[i++] = (uint8_t)dev->feature_report_size - 1;
		/* feature (data, variable, absolute) */
		d[i++] = 0xb2; d[i++] = 0x02; d[i++] = 0x01;
	}
	/* end collection */
	d[i++] = 0xC0;
	if (*size > i)
		*size = i;
	memcpy(data, d, *size);
	return LIBUSB_COMPLETED;
}
