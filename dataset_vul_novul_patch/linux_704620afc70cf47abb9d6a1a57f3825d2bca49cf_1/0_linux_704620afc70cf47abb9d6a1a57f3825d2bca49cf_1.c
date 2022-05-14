static int hwahc_security_create(struct hwahc *hwahc)
{
	int result;
	struct wusbhc *wusbhc = &hwahc->wusbhc;
	struct usb_device *usb_dev = hwahc->wa.usb_dev;
	struct device *dev = &usb_dev->dev;
	struct usb_security_descriptor *secd;
	struct usb_encryption_descriptor *etd;
	void *itr, *top;
	size_t itr_size, needed, bytes;
	u8 index;
	char buf[64];

	/* Find the host's security descriptors in the config descr bundle */
	index = (usb_dev->actconfig - usb_dev->config) /
		sizeof(usb_dev->config[0]);
	itr = usb_dev->rawdescriptors[index];
	itr_size = le16_to_cpu(usb_dev->actconfig->desc.wTotalLength);
 	top = itr + itr_size;
 	result = __usb_get_extra_descriptor(usb_dev->rawdescriptors[index],
 			le16_to_cpu(usb_dev->actconfig->desc.wTotalLength),
			USB_DT_SECURITY, (void **) &secd, sizeof(*secd));
 	if (result == -1) {
 		dev_warn(dev, "BUG? WUSB host has no security descriptors\n");
 		return 0;
	}
	needed = sizeof(*secd);
	if (top - (void *)secd < needed) {
		dev_err(dev, "BUG? Not enough data to process security "
			"descriptor header (%zu bytes left vs %zu needed)\n",
			top - (void *) secd, needed);
		return 0;
	}
	needed = le16_to_cpu(secd->wTotalLength);
	if (top - (void *)secd < needed) {
		dev_err(dev, "BUG? Not enough data to process security "
			"descriptors (%zu bytes left vs %zu needed)\n",
			top - (void *) secd, needed);
		return 0;
	}
	/* Walk over the sec descriptors and store CCM1's on wusbhc */
	itr = (void *) secd + sizeof(*secd);
	top = (void *) secd + le16_to_cpu(secd->wTotalLength);
	index = 0;
	bytes = 0;
	while (itr < top) {
		etd = itr;
		if (top - itr < sizeof(*etd)) {
			dev_err(dev, "BUG: bad host security descriptor; "
				"not enough data (%zu vs %zu left)\n",
				top - itr, sizeof(*etd));
			break;
		}
		if (etd->bLength < sizeof(*etd)) {
			dev_err(dev, "BUG: bad host encryption descriptor; "
				"descriptor is too short "
				"(%zu vs %zu needed)\n",
				(size_t)etd->bLength, sizeof(*etd));
			break;
		}
		itr += etd->bLength;
		bytes += snprintf(buf + bytes, sizeof(buf) - bytes,
				  "%s (0x%02x) ",
				  wusb_et_name(etd->bEncryptionType),
				  etd->bEncryptionValue);
		wusbhc->ccm1_etd = etd;
	}
	dev_info(dev, "supported encryption types: %s\n", buf);
	if (wusbhc->ccm1_etd == NULL) {
		dev_err(dev, "E: host doesn't support CCM-1 crypto\n");
		return 0;
	}
	/* Pretty print what we support */
	return 0;
}
