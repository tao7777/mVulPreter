static int windows_get_device_list(struct libusb_context *ctx, struct discovered_devs **_discdevs)
{
	struct discovered_devs *discdevs;
	HDEVINFO dev_info = { 0 };
 	const char* usb_class[] = {"USB", "NUSB3", "IUSB3"};
 	SP_DEVINFO_DATA dev_info_data = { 0 };
 	SP_DEVICE_INTERFACE_DETAIL_DATA_A *dev_interface_details = NULL;
	GUID hid_guid;
 #define MAX_ENUM_GUIDS 64
 	const GUID* guid[MAX_ENUM_GUIDS];
 #define HCD_PASS 0
 #define HUB_PASS 1
 #define GEN_PASS 2
 #define DEV_PASS 3
#define HID_PASS 4
 	int r = LIBUSB_SUCCESS;
 	int api, sub_api;
 	size_t class_index = 0;
	unsigned int nb_guids, pass, i, j, ancestor;
	char path[MAX_PATH_LENGTH];
	char strbuf[MAX_PATH_LENGTH];
	struct libusb_device *dev, *parent_dev;
	struct windows_device_priv *priv, *parent_priv;
	char* dev_interface_path = NULL;
	char* dev_id_path = NULL;
	unsigned long session_id;
	DWORD size, reg_type, port_nr, install_state;
	HKEY key;
	WCHAR guid_string_w[MAX_GUID_STRING_LENGTH];
	GUID* if_guid;
	LONG s;
	libusb_device** unref_list;
	unsigned int unref_size = 64;
	unsigned int unref_cur = 0;

 
 	guid[HCD_PASS] = &GUID_DEVINTERFACE_USB_HOST_CONTROLLER;
 	guid[HUB_PASS] = &GUID_DEVINTERFACE_USB_HUB;
 	guid[GEN_PASS] = NULL;
 	guid[DEV_PASS] = &GUID_DEVINTERFACE_USB_DEVICE;
	HidD_GetHidGuid(&hid_guid);
	guid[HID_PASS] = &hid_guid;
	nb_guids = HID_PASS+1;
 
 	unref_list = (libusb_device**) calloc(unref_size, sizeof(libusb_device*));
 	if (unref_list == NULL) {
		return LIBUSB_ERROR_NO_MEM;
	}

 	for (pass = 0; ((pass < nb_guids) && (r == LIBUSB_SUCCESS)); pass++) {
 #ifdef ENUM_DEBUG
		const char *passname[] = { "HCD", "HUB", "GEN", "DEV", "HID", "EXT" };
		usbi_dbg("\n#### PROCESSING %ss %s", passname[(pass<=HID_PASS)?pass:HID_PASS+1],
 			(pass!=GEN_PASS)?guid_to_string(guid[pass]):"");
 #endif
 		for (i = 0; ; i++) {
			safe_free(dev_interface_details);
			safe_free(dev_interface_path);
			safe_free(dev_id_path);
			priv = parent_priv = NULL;
			dev = parent_dev = NULL;

			if (r != LIBUSB_SUCCESS) {
				break;
			}
			if ((pass == HCD_PASS) && (i == UINT8_MAX)) {
				usbi_warn(ctx, "program assertion failed - found more than %d buses, skipping the rest.", UINT8_MAX);
				break;
			}
			if (pass != GEN_PASS) {
				dev_interface_details = get_interface_details(ctx, &dev_info, &dev_info_data, guid[pass], i);
				if (dev_interface_details == NULL) {
					break;
				} else {
					dev_interface_path = sanitize_path(dev_interface_details->DevicePath);
					if (dev_interface_path == NULL) {
						usbi_warn(ctx, "could not sanitize device interface path for '%s'", dev_interface_details->DevicePath);
						continue;
					}
				}
			} else {
				for (; class_index < ARRAYSIZE(usb_class); class_index++) {
					if (get_devinfo_data(ctx, &dev_info, &dev_info_data, usb_class[class_index], i))
						break;
					i = 0;
				}
				if (class_index >= ARRAYSIZE(usb_class))
					break;
			}

			if (CM_Get_Device_IDA(dev_info_data.DevInst, path, sizeof(path), 0) != CR_SUCCESS) {
				usbi_warn(ctx, "could not read the device id path for devinst %X, skipping",
					dev_info_data.DevInst);
				continue;
			}
			dev_id_path = sanitize_path(path);
			if (dev_id_path == NULL) {
				usbi_warn(ctx, "could not sanitize device id path for devinst %X, skipping",
					dev_info_data.DevInst);
				continue;
			}
#ifdef ENUM_DEBUG
			usbi_dbg("PRO: %s", dev_id_path);
#endif

			port_nr = 0;
			if ((pass >= HUB_PASS) && (pass <= GEN_PASS)) {
				if ( (!pSetupDiGetDeviceRegistryPropertyA(dev_info, &dev_info_data, SPDRP_ADDRESS,
					&reg_type, (BYTE*)&port_nr, 4, &size))
				  || (size != 4) ) {
					usbi_warn(ctx, "could not retrieve port number for device '%s', skipping: %s",
						dev_id_path, windows_error_str(0));
					continue;
				}
			}

			api = USB_API_UNSUPPORTED;
			sub_api = SUB_API_NOTSET;
			switch (pass) {
			case HCD_PASS:
				break;
			case GEN_PASS:
				size = sizeof(strbuf);
				if (!pSetupDiGetDeviceRegistryPropertyA(dev_info, &dev_info_data, SPDRP_DRIVER,
					&reg_type, (BYTE*)strbuf, size, &size)) {
						usbi_info(ctx, "The following device has no driver: '%s'", dev_id_path);
						usbi_info(ctx, "libusbx will not be able to access it.");
				}
				key = pSetupDiOpenDevRegKey(dev_info, &dev_info_data, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
				if (key != INVALID_HANDLE_VALUE) {
					size = sizeof(guid_string_w);
					s = pRegQueryValueExW(key, L"DeviceInterfaceGUIDs", NULL, &reg_type,
						(BYTE*)guid_string_w, &size);
					pRegCloseKey(key);
					if (s == ERROR_SUCCESS) {
						if (nb_guids >= MAX_ENUM_GUIDS) {
							usbi_err(ctx, "program assertion failed: too many GUIDs");
							LOOP_BREAK(LIBUSB_ERROR_OVERFLOW);
						}
						if_guid = (GUID*) calloc(1, sizeof(GUID));
						pCLSIDFromString(guid_string_w, if_guid);
						guid[nb_guids++] = if_guid;
						usbi_dbg("extra GUID: %s", guid_to_string(if_guid));
 					}
 				}
 				break;
			case HID_PASS:
				api = USB_API_HID;
				break;
 			default:
 				if ( (!pSetupDiGetDeviceRegistryPropertyA(dev_info, &dev_info_data, SPDRP_INSTALL_STATE,
					&reg_type, (BYTE*)&install_state, 4, &size))
				  || (size != 4) ){
					usbi_warn(ctx, "could not detect installation state of driver for '%s': %s",
						dev_id_path, windows_error_str(0));
				} else if (install_state != 0) {
					usbi_warn(ctx, "driver for device '%s' is reporting an issue (code: %d) - skipping",
						dev_id_path, install_state);
					continue;
				}
				get_api_type(ctx, &dev_info, &dev_info_data, &api, &sub_api);
				break;
			}

			switch (pass) {
			case HCD_PASS:
			case DEV_PASS:
			case HUB_PASS:
				break;
			default:
				parent_dev = NULL;
				for (ancestor = 1; parent_dev == NULL; ancestor++) {
					session_id = get_ancestor_session_id(dev_info_data.DevInst, ancestor);
					if (session_id == 0) {
						break;
					}
					parent_dev = usbi_get_device_by_session_id(ctx, session_id);
				}
				if (parent_dev == NULL) {
					usbi_dbg("unlisted ancestor for '%s' (non USB HID, newly connected, etc.) - ignoring", dev_id_path);
					continue;
				}
				parent_priv = _device_priv(parent_dev);
				if ( (pass == GEN_PASS) && (parent_priv->apib->id != USB_API_HUB) ) {
					continue;
				}
				break;
			}

			if (pass <= DEV_PASS) {	// For subsequent passes, we'll lookup the parent
				session_id = htab_hash(dev_id_path);
				dev = usbi_get_device_by_session_id(ctx, session_id);
				if (dev == NULL) {
					if (pass == DEV_PASS) {
						usbi_warn(ctx, "'%s' was only detected in late pass (newly connected device?)"
							" - ignoring", dev_id_path);
						continue;
					}
					usbi_dbg("allocating new device for session [%X]", session_id);
					if ((dev = usbi_alloc_device(ctx, session_id)) == NULL) {
						LOOP_BREAK(LIBUSB_ERROR_NO_MEM);
					}
					windows_device_priv_init(dev);
					unref_list[unref_cur++] = dev;
					if (unref_cur >= unref_size) {
						unref_size += 64;
						unref_list = usbi_reallocf(unref_list, unref_size*sizeof(libusb_device*));
						if (unref_list == NULL) {
							usbi_err(ctx, "could not realloc list for unref - aborting.");
							LOOP_BREAK(LIBUSB_ERROR_NO_MEM);
						}
					}
				} else {
					usbi_dbg("found existing device for session [%X] (%d.%d)",
						session_id, dev->bus_number, dev->device_address);
				}
				priv = _device_priv(dev);
			}

			switch (pass) {
			case HCD_PASS:
				dev->bus_number = (uint8_t)(i + 1);	// bus 0 is reserved for disconnected
				dev->device_address = 0;
				dev->num_configurations = 0;
				priv->apib = &usb_api_backend[USB_API_HUB];
				priv->sub_api = SUB_API_NOTSET;
				priv->depth = UINT8_MAX;	// Overflow to 0 for HCD Hubs
				priv->path = dev_interface_path; dev_interface_path = NULL;
				break;
			case HUB_PASS:
			case DEV_PASS:
				if (priv->path != NULL)
					break;
				priv->path = dev_interface_path; dev_interface_path = NULL;
				priv->apib = &usb_api_backend[api];
				priv->sub_api = sub_api;
				switch(api) {
 				case USB_API_COMPOSITE:
 				case USB_API_HUB:
 					break;
				case USB_API_HID:
					priv->hid = calloc(1, sizeof(struct hid_device_priv));
					if (priv->hid == NULL) {
						LOOP_BREAK(LIBUSB_ERROR_NO_MEM);
					}
					priv->hid->nb_interfaces = 0;
					break;
 				default:
 					priv->usb_interface[0].path = (char*) calloc(safe_strlen(priv->path)+1, 1);
					if (priv->usb_interface[0].path != NULL) {
						safe_strcpy(priv->usb_interface[0].path, safe_strlen(priv->path)+1, priv->path);
					} else {
						usbi_warn(ctx, "could not duplicate interface path '%s'", priv->path);
					}
					for(j=0; j<USB_MAXINTERFACES; j++) {
						priv->usb_interface[j].apib = &usb_api_backend[api];
					}
					break;
				}
				break;
			case GEN_PASS:
				r = init_device(dev, parent_dev, (uint8_t)port_nr, dev_id_path, dev_info_data.DevInst);
				if (r == LIBUSB_SUCCESS) {
					discdevs = discovered_devs_append(*_discdevs, dev);
					if (!discdevs) {
						LOOP_BREAK(LIBUSB_ERROR_NO_MEM);
					}
					*_discdevs = discdevs;
				} else if (r == LIBUSB_ERROR_NO_DEVICE) {
 					r = LIBUSB_SUCCESS;
 				}
 				break;
			default:	// HID_PASS and later
				if (parent_priv->apib->id == USB_API_HID) {
					usbi_dbg("setting HID interface for [%lX]:", parent_dev->session_data);
					r = set_hid_interface(ctx, parent_dev, dev_interface_path);
					if (r != LIBUSB_SUCCESS) LOOP_BREAK(r);
					dev_interface_path = NULL;
				} else if (parent_priv->apib->id == USB_API_COMPOSITE) {
 					usbi_dbg("setting composite interface for [%lX]:", parent_dev->session_data);
 					switch (set_composite_interface(ctx, parent_dev, dev_interface_path, dev_id_path, api, sub_api)) {
 					case LIBUSB_SUCCESS:
						dev_interface_path = NULL;
						break;
					case LIBUSB_ERROR_ACCESS:
						break;
					default:
						LOOP_BREAK(r);
						break;
					}
				}
				break;
			}
		}
 	}
 
	for (pass = HID_PASS+1; pass < nb_guids; pass++) {
 		safe_free(guid[pass]);
 	}
 
	for (i=0; i<unref_cur; i++) {
		safe_unref_device(unref_list[i]);
	}
	safe_free(unref_list);

	return r;
}
