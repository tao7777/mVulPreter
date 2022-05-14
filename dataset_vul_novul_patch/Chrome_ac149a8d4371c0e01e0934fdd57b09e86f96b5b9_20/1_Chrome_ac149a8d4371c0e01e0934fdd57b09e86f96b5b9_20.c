static int hid_open(int sub_api, struct libusb_device_handle *dev_handle)
{
	struct libusb_context *ctx = DEVICE_CTX(dev_handle->dev);
	struct windows_device_priv *priv = _device_priv(dev_handle->dev);
	struct windows_device_handle_priv *handle_priv = _device_handle_priv(dev_handle);
	HIDD_ATTRIBUTES hid_attributes;
	PHIDP_PREPARSED_DATA preparsed_data = NULL;
	HIDP_CAPS capabilities;
	HIDP_VALUE_CAPS *value_caps;
	HANDLE hid_handle = INVALID_HANDLE_VALUE;
	int i, j;
	ULONG size[3];
	const char* type[3] = {"input", "output", "feature"};
	int nb_ids[2];	// zero and nonzero report IDs
	CHECK_HID_AVAILABLE;
	if (priv->hid == NULL) {
		usbi_err(ctx, "program assertion failed - private HID structure is unitialized");
		return LIBUSB_ERROR_NOT_FOUND;
	}
	for (i = 0; i < USB_MAXINTERFACES; i++) {
		if ( (priv->usb_interface[i].path != NULL)
		  && (priv->usb_interface[i].apib->id == USB_API_HID) ) {
			hid_handle = CreateFileA(priv->usb_interface[i].path, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ,
				NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
			/*
			 * http://www.lvr.com/hidfaq.htm: Why do I receive "Access denied" when attempting to access my HID?
			 * "Windows 2000 and later have exclusive read/write access to HIDs that are configured as a system
			 * keyboards or mice. An application can obtain a handle to a system keyboard or mouse by not
			 * requesting READ or WRITE access with CreateFile. Applications can then use HidD_SetFeature and
			 * HidD_GetFeature (if the device supports Feature reports)."
			 */
			if (hid_handle == INVALID_HANDLE_VALUE) {
				usbi_warn(ctx, "could not open HID device in R/W mode (keyboard or mouse?) - trying without");
				hid_handle = CreateFileA(priv->usb_interface[i].path, 0, FILE_SHARE_WRITE | FILE_SHARE_READ,
					NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
				if (hid_handle == INVALID_HANDLE_VALUE) {
					usbi_err(ctx, "could not open device %s (interface %d): %s", priv->path, i, windows_error_str(0));
					switch(GetLastError()) {
					case ERROR_FILE_NOT_FOUND:	// The device was disconnected
						return LIBUSB_ERROR_NO_DEVICE;
					case ERROR_ACCESS_DENIED:
						return LIBUSB_ERROR_ACCESS;
					default:
						return LIBUSB_ERROR_IO;
					}
				}
				priv->usb_interface[i].restricted_functionality = true;
			}
			handle_priv->interface_handle[i].api_handle = hid_handle;
		}
	}
	hid_attributes.Size = sizeof(hid_attributes);
	do {
		if (!HidD_GetAttributes(hid_handle, &hid_attributes)) {
			usbi_err(ctx, "could not gain access to HID top collection (HidD_GetAttributes)");
			break;
		}
		priv->hid->vid = hid_attributes.VendorID;
		priv->hid->pid = hid_attributes.ProductID;
		for (i=32; HidD_SetNumInputBuffers(hid_handle, i); i*=2);
		usbi_dbg("set maximum input buffer size to %d", i/2);
		if (!HidD_GetPreparsedData(hid_handle, &preparsed_data) || !preparsed_data) {
			usbi_err(ctx, "could not read HID preparsed data (HidD_GetPreparsedData)");
			break;
		}
		if (HidP_GetCaps(preparsed_data, &capabilities) != HIDP_STATUS_SUCCESS) {
			usbi_err(ctx, "could not parse HID capabilities (HidP_GetCaps)");
			break;
		}
		size[0] = capabilities.NumberInputValueCaps;
		size[1] = capabilities.NumberOutputValueCaps;
		size[2] = capabilities.NumberFeatureValueCaps;
		for (j=HidP_Input; j<=HidP_Feature; j++) {
			usbi_dbg("%d HID %s report value(s) found", size[j], type[j]);
			priv->hid->uses_report_ids[j] = false;
			if (size[j] > 0) {
				value_caps = (HIDP_VALUE_CAPS*) calloc(size[j], sizeof(HIDP_VALUE_CAPS));
				if ( (value_caps != NULL)
				  && (HidP_GetValueCaps((HIDP_REPORT_TYPE)j, value_caps, &size[j], preparsed_data) == HIDP_STATUS_SUCCESS)
				  && (size[j] >= 1) ) {
					nb_ids[0] = 0;
					nb_ids[1] = 0;
					for (i=0; i<(int)size[j]; i++) {
						usbi_dbg("  Report ID: 0x%02X", value_caps[i].ReportID);
						if (value_caps[i].ReportID != 0) {
							nb_ids[1]++;
						} else {
							nb_ids[0]++;
						}
					}
					if (nb_ids[1] != 0) {
						if (nb_ids[0] != 0) {
							usbi_warn(ctx, "program assertion failed: zero and nonzero report IDs used for %s",
								type[j]);
						}
						priv->hid->uses_report_ids[j] = true;
					}
				} else {
					usbi_warn(ctx, "  could not process %s report IDs", type[j]);
				}
				safe_free(value_caps);
			}
		}
		priv->hid->input_report_size = capabilities.InputReportByteLength;
		priv->hid->output_report_size = capabilities.OutputReportByteLength;
		priv->hid->feature_report_size = capabilities.FeatureReportByteLength;
		priv->hid->string_index[0] = priv->dev_descriptor.iManufacturer;
		if (priv->hid->string_index[0] != 0) {
			HidD_GetManufacturerString(hid_handle, priv->hid->string[0],
				sizeof(priv->hid->string[0]));
		} else {
			priv->hid->string[0][0] = 0;
		}
		priv->hid->string_index[1] = priv->dev_descriptor.iProduct;
		if (priv->hid->string_index[1] != 0) {
			HidD_GetProductString(hid_handle, priv->hid->string[1],
				sizeof(priv->hid->string[1]));
		} else {
			priv->hid->string[1][0] = 0;
		}
		priv->hid->string_index[2] = priv->dev_descriptor.iSerialNumber;
		if (priv->hid->string_index[2] != 0) {
			HidD_GetSerialNumberString(hid_handle, priv->hid->string[2],
				sizeof(priv->hid->string[2]));
		} else {
			priv->hid->string[2][0] = 0;
		}
	} while(0);
	if (preparsed_data) {
		HidD_FreePreparsedData(preparsed_data);
	}
	return LIBUSB_SUCCESS;
}
