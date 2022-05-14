static int acm_probe(struct usb_interface *intf,
		     const struct usb_device_id *id)
{
	struct usb_cdc_union_desc *union_header = NULL;
	struct usb_cdc_country_functional_desc *cfd = NULL;
	unsigned char *buffer = intf->altsetting->extra;
	int buflen = intf->altsetting->extralen;
	struct usb_interface *control_interface;
	struct usb_interface *data_interface;
	struct usb_endpoint_descriptor *epctrl = NULL;
	struct usb_endpoint_descriptor *epread = NULL;
	struct usb_endpoint_descriptor *epwrite = NULL;
	struct usb_device *usb_dev = interface_to_usbdev(intf);
	struct acm *acm;
	int minor;
	int ctrlsize, readsize;
	u8 *buf;
	u8 ac_management_function = 0;
	u8 call_management_function = 0;
	int call_interface_num = -1;
	int data_interface_num = -1;
	unsigned long quirks;
	int num_rx_buf;
	int i;
	unsigned int elength = 0;
	int combined_interfaces = 0;
	struct device *tty_dev;
	int rv = -ENOMEM;

	/* normal quirks */
	quirks = (unsigned long)id->driver_info;

	if (quirks == IGNORE_DEVICE)
		return -ENODEV;

	num_rx_buf = (quirks == SINGLE_RX_URB) ? 1 : ACM_NR;

	/* handle quirks deadly to normal probing*/
 	if (quirks == NO_UNION_NORMAL) {
 		data_interface = usb_ifnum_to_if(usb_dev, 1);
 		control_interface = usb_ifnum_to_if(usb_dev, 0);
		/* we would crash */
		if (!data_interface || !control_interface)
			return -ENODEV;
 		goto skip_normal_probe;
 	}
 
	/* normal probing*/
	if (!buffer) {
		dev_err(&intf->dev, "Weird descriptor references\n");
		return -EINVAL;
	}

	if (!buflen) {
		if (intf->cur_altsetting->endpoint &&
				intf->cur_altsetting->endpoint->extralen &&
				intf->cur_altsetting->endpoint->extra) {
			dev_dbg(&intf->dev,
				"Seeking extra descriptors on endpoint\n");
			buflen = intf->cur_altsetting->endpoint->extralen;
			buffer = intf->cur_altsetting->endpoint->extra;
		} else {
			dev_err(&intf->dev,
				"Zero length descriptor references\n");
			return -EINVAL;
		}
	}

	while (buflen > 0) {
		elength = buffer[0];
		if (!elength) {
			dev_err(&intf->dev, "skipping garbage byte\n");
			elength = 1;
			goto next_desc;
		}
		if (buffer[1] != USB_DT_CS_INTERFACE) {
			dev_err(&intf->dev, "skipping garbage\n");
			goto next_desc;
		}

		switch (buffer[2]) {
		case USB_CDC_UNION_TYPE: /* we've found it */
			if (elength < sizeof(struct usb_cdc_union_desc))
				goto next_desc;
			if (union_header) {
				dev_err(&intf->dev, "More than one "
					"union descriptor, skipping ...\n");
				goto next_desc;
			}
			union_header = (struct usb_cdc_union_desc *)buffer;
			break;
		case USB_CDC_COUNTRY_TYPE: /* export through sysfs*/
			if (elength < sizeof(struct usb_cdc_country_functional_desc))
				goto next_desc;
			cfd = (struct usb_cdc_country_functional_desc *)buffer;
			break;
		case USB_CDC_HEADER_TYPE: /* maybe check version */
			break; /* for now we ignore it */
		case USB_CDC_ACM_TYPE:
			if (elength < 4)
				goto next_desc;
			ac_management_function = buffer[3];
			break;
		case USB_CDC_CALL_MANAGEMENT_TYPE:
			if (elength < 5)
				goto next_desc;
			call_management_function = buffer[3];
			call_interface_num = buffer[4];
			break;
		default:
			/*
			 * there are LOTS more CDC descriptors that
			 * could legitimately be found here.
			 */
			dev_dbg(&intf->dev, "Ignoring descriptor: "
					"type %02x, length %ud\n",
					buffer[2], elength);
			break;
		}
next_desc:
		buflen -= elength;
		buffer += elength;
	}

	if (!union_header) {
		if (call_interface_num > 0) {
			dev_dbg(&intf->dev, "No union descriptor, using call management descriptor\n");
			/* quirks for Droids MuIn LCD */
			if (quirks & NO_DATA_INTERFACE)
				data_interface = usb_ifnum_to_if(usb_dev, 0);
			else
				data_interface = usb_ifnum_to_if(usb_dev, (data_interface_num = call_interface_num));
			control_interface = intf;
		} else {
			if (intf->cur_altsetting->desc.bNumEndpoints != 3) {
				dev_dbg(&intf->dev,"No union descriptor, giving up\n");
				return -ENODEV;
			} else {
				dev_warn(&intf->dev,"No union descriptor, testing for castrated device\n");
				combined_interfaces = 1;
				control_interface = data_interface = intf;
				goto look_for_collapsed_interface;
			}
		}
	} else {
		control_interface = usb_ifnum_to_if(usb_dev, union_header->bMasterInterface0);
		data_interface = usb_ifnum_to_if(usb_dev, (data_interface_num = union_header->bSlaveInterface0));
	}

	if (!control_interface || !data_interface) {
		dev_dbg(&intf->dev, "no interfaces\n");
		return -ENODEV;
	}

	if (data_interface_num != call_interface_num)
		dev_dbg(&intf->dev, "Separate call control interface. That is not fully supported.\n");

	if (control_interface == data_interface) {
		/* some broken devices designed for windows work this way */
		dev_warn(&intf->dev,"Control and data interfaces are not separated!\n");
		combined_interfaces = 1;
		/* a popular other OS doesn't use it */
		quirks |= NO_CAP_LINE;
		if (data_interface->cur_altsetting->desc.bNumEndpoints != 3) {
			dev_err(&intf->dev, "This needs exactly 3 endpoints\n");
			return -EINVAL;
		}
look_for_collapsed_interface:
		for (i = 0; i < 3; i++) {
			struct usb_endpoint_descriptor *ep;
			ep = &data_interface->cur_altsetting->endpoint[i].desc;

			if (usb_endpoint_is_int_in(ep))
				epctrl = ep;
			else if (usb_endpoint_is_bulk_out(ep))
				epwrite = ep;
			else if (usb_endpoint_is_bulk_in(ep))
				epread = ep;
			else
				return -EINVAL;
		}
		if (!epctrl || !epread || !epwrite)
			return -ENODEV;
		else
			goto made_compressed_probe;
	}

skip_normal_probe:

	/*workaround for switched interfaces */
	if (data_interface->cur_altsetting->desc.bInterfaceClass
						!= CDC_DATA_INTERFACE_TYPE) {
		if (control_interface->cur_altsetting->desc.bInterfaceClass
						== CDC_DATA_INTERFACE_TYPE) {
			dev_dbg(&intf->dev,
				"Your device has switched interfaces.\n");
			swap(control_interface, data_interface);
		} else {
			return -EINVAL;
		}
	}

	/* Accept probe requests only for the control interface */
	if (!combined_interfaces && intf != control_interface)
		return -ENODEV;

	if (!combined_interfaces && usb_interface_claimed(data_interface)) {
		/* valid in this context */
		dev_dbg(&intf->dev, "The data interface isn't available\n");
		return -EBUSY;
	}


	if (data_interface->cur_altsetting->desc.bNumEndpoints < 2 ||
	    control_interface->cur_altsetting->desc.bNumEndpoints == 0)
		return -EINVAL;

	epctrl = &control_interface->cur_altsetting->endpoint[0].desc;
	epread = &data_interface->cur_altsetting->endpoint[0].desc;
	epwrite = &data_interface->cur_altsetting->endpoint[1].desc;


	/* workaround for switched endpoints */
	if (!usb_endpoint_dir_in(epread)) {
		/* descriptors are swapped */
		dev_dbg(&intf->dev,
			"The data interface has switched endpoints\n");
		swap(epread, epwrite);
	}
made_compressed_probe:
	dev_dbg(&intf->dev, "interfaces are valid\n");

	acm = kzalloc(sizeof(struct acm), GFP_KERNEL);
	if (acm == NULL)
		goto alloc_fail;

	minor = acm_alloc_minor(acm);
	if (minor < 0) {
		dev_err(&intf->dev, "no more free acm devices\n");
		kfree(acm);
		return -ENODEV;
	}

	ctrlsize = usb_endpoint_maxp(epctrl);
	readsize = usb_endpoint_maxp(epread) *
				(quirks == SINGLE_RX_URB ? 1 : 2);
	acm->combined_interfaces = combined_interfaces;
	acm->writesize = usb_endpoint_maxp(epwrite) * 20;
	acm->control = control_interface;
	acm->data = data_interface;
	acm->minor = minor;
	acm->dev = usb_dev;
	acm->ctrl_caps = ac_management_function;
	if (quirks & NO_CAP_LINE)
		acm->ctrl_caps &= ~USB_CDC_CAP_LINE;
	acm->ctrlsize = ctrlsize;
	acm->readsize = readsize;
	acm->rx_buflimit = num_rx_buf;
	INIT_WORK(&acm->work, acm_softint);
	init_waitqueue_head(&acm->wioctl);
	spin_lock_init(&acm->write_lock);
	spin_lock_init(&acm->read_lock);
	mutex_init(&acm->mutex);
	acm->rx_endpoint = usb_rcvbulkpipe(usb_dev, epread->bEndpointAddress);
	acm->is_int_ep = usb_endpoint_xfer_int(epread);
	if (acm->is_int_ep)
		acm->bInterval = epread->bInterval;
	tty_port_init(&acm->port);
	acm->port.ops = &acm_port_ops;
	init_usb_anchor(&acm->delayed);
	acm->quirks = quirks;

	buf = usb_alloc_coherent(usb_dev, ctrlsize, GFP_KERNEL, &acm->ctrl_dma);
	if (!buf)
		goto alloc_fail2;
	acm->ctrl_buffer = buf;

	if (acm_write_buffers_alloc(acm) < 0)
		goto alloc_fail4;

	acm->ctrlurb = usb_alloc_urb(0, GFP_KERNEL);
	if (!acm->ctrlurb)
		goto alloc_fail5;

	for (i = 0; i < num_rx_buf; i++) {
		struct acm_rb *rb = &(acm->read_buffers[i]);
		struct urb *urb;

		rb->base = usb_alloc_coherent(acm->dev, readsize, GFP_KERNEL,
								&rb->dma);
		if (!rb->base)
			goto alloc_fail6;
		rb->index = i;
		rb->instance = acm;

		urb = usb_alloc_urb(0, GFP_KERNEL);
		if (!urb)
			goto alloc_fail6;

		urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;
		urb->transfer_dma = rb->dma;
		if (acm->is_int_ep) {
			usb_fill_int_urb(urb, acm->dev,
					 acm->rx_endpoint,
					 rb->base,
					 acm->readsize,
					 acm_read_bulk_callback, rb,
					 acm->bInterval);
		} else {
			usb_fill_bulk_urb(urb, acm->dev,
					  acm->rx_endpoint,
					  rb->base,
					  acm->readsize,
					  acm_read_bulk_callback, rb);
		}

		acm->read_urbs[i] = urb;
		__set_bit(i, &acm->read_urbs_free);
	}
	for (i = 0; i < ACM_NW; i++) {
		struct acm_wb *snd = &(acm->wb[i]);

		snd->urb = usb_alloc_urb(0, GFP_KERNEL);
		if (snd->urb == NULL)
			goto alloc_fail7;

		if (usb_endpoint_xfer_int(epwrite))
			usb_fill_int_urb(snd->urb, usb_dev,
				usb_sndintpipe(usb_dev, epwrite->bEndpointAddress),
				NULL, acm->writesize, acm_write_bulk, snd, epwrite->bInterval);
		else
			usb_fill_bulk_urb(snd->urb, usb_dev,
				usb_sndbulkpipe(usb_dev, epwrite->bEndpointAddress),
				NULL, acm->writesize, acm_write_bulk, snd);
		snd->urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;
		if (quirks & SEND_ZERO_PACKET)
			snd->urb->transfer_flags |= URB_ZERO_PACKET;
		snd->instance = acm;
	}

	usb_set_intfdata(intf, acm);

	i = device_create_file(&intf->dev, &dev_attr_bmCapabilities);
	if (i < 0)
		goto alloc_fail7;

	if (cfd) { /* export the country data */
		acm->country_codes = kmalloc(cfd->bLength - 4, GFP_KERNEL);
		if (!acm->country_codes)
			goto skip_countries;
		acm->country_code_size = cfd->bLength - 4;
		memcpy(acm->country_codes, (u8 *)&cfd->wCountyCode0,
							cfd->bLength - 4);
		acm->country_rel_date = cfd->iCountryCodeRelDate;

		i = device_create_file(&intf->dev, &dev_attr_wCountryCodes);
		if (i < 0) {
			kfree(acm->country_codes);
			acm->country_codes = NULL;
			acm->country_code_size = 0;
			goto skip_countries;
		}

		i = device_create_file(&intf->dev,
						&dev_attr_iCountryCodeRelDate);
		if (i < 0) {
			device_remove_file(&intf->dev, &dev_attr_wCountryCodes);
			kfree(acm->country_codes);
			acm->country_codes = NULL;
			acm->country_code_size = 0;
			goto skip_countries;
		}
	}

skip_countries:
	usb_fill_int_urb(acm->ctrlurb, usb_dev,
			 usb_rcvintpipe(usb_dev, epctrl->bEndpointAddress),
			 acm->ctrl_buffer, ctrlsize, acm_ctrl_irq, acm,
			 /* works around buggy devices */
			 epctrl->bInterval ? epctrl->bInterval : 16);
	acm->ctrlurb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;
	acm->ctrlurb->transfer_dma = acm->ctrl_dma;

	dev_info(&intf->dev, "ttyACM%d: USB ACM device\n", minor);

	acm->line.dwDTERate = cpu_to_le32(9600);
	acm->line.bDataBits = 8;
	acm_set_line(acm, &acm->line);

	usb_driver_claim_interface(&acm_driver, data_interface, acm);
	usb_set_intfdata(data_interface, acm);

	usb_get_intf(control_interface);
	tty_dev = tty_port_register_device(&acm->port, acm_tty_driver, minor,
			&control_interface->dev);
	if (IS_ERR(tty_dev)) {
		rv = PTR_ERR(tty_dev);
		goto alloc_fail8;
	}

	if (quirks & CLEAR_HALT_CONDITIONS) {
		usb_clear_halt(usb_dev, usb_rcvbulkpipe(usb_dev, epread->bEndpointAddress));
		usb_clear_halt(usb_dev, usb_sndbulkpipe(usb_dev, epwrite->bEndpointAddress));
	}

	return 0;
alloc_fail8:
	if (acm->country_codes) {
		device_remove_file(&acm->control->dev,
				&dev_attr_wCountryCodes);
		device_remove_file(&acm->control->dev,
				&dev_attr_iCountryCodeRelDate);
		kfree(acm->country_codes);
	}
	device_remove_file(&acm->control->dev, &dev_attr_bmCapabilities);
alloc_fail7:
	usb_set_intfdata(intf, NULL);
	for (i = 0; i < ACM_NW; i++)
		usb_free_urb(acm->wb[i].urb);
alloc_fail6:
	for (i = 0; i < num_rx_buf; i++)
		usb_free_urb(acm->read_urbs[i]);
	acm_read_buffers_free(acm);
	usb_free_urb(acm->ctrlurb);
alloc_fail5:
	acm_write_buffers_free(acm);
alloc_fail4:
	usb_free_coherent(usb_dev, ctrlsize, acm->ctrl_buffer, acm->ctrl_dma);
alloc_fail2:
	acm_release_minor(acm);
	kfree(acm);
alloc_fail:
	return rv;
}
