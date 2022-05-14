static int _hid_get_device_descriptor(struct hid_device_priv* dev, void *data, size_t *size)
{
	struct libusb_device_descriptor d;
	d.bLength = LIBUSB_DT_DEVICE_SIZE;
	d.bDescriptorType = LIBUSB_DT_DEVICE;
	d.bcdUSB = 0x0200; /* 2.00 */
	d.bDeviceClass = 0;
	d.bDeviceSubClass = 0;
	d.bDeviceProtocol = 0;
	d.bMaxPacketSize0 = 64; /* fix this! */
	d.idVendor = (uint16_t)dev->vid;
	d.idProduct = (uint16_t)dev->pid;
	d.bcdDevice = 0x0100;
	d.iManufacturer = dev->string_index[0];
	d.iProduct = dev->string_index[1];
	d.iSerialNumber = dev->string_index[2];
	d.bNumConfigurations = 1;
	if (*size > LIBUSB_DT_DEVICE_SIZE)
		*size = LIBUSB_DT_DEVICE_SIZE;
	memcpy(data, &d, *size);
	return LIBUSB_COMPLETED;
}
