static int spl_load_fit_image(struct spl_load_info *info, ulong sector,
			      void *fit, ulong base_offset, int node,
			      struct spl_image_info *image_info)
{
	int offset;
	size_t length;
	int len;
	ulong size;
	ulong load_addr, load_ptr;
	void *src;
	ulong overhead;
	int nr_sectors;
	int align_len = ARCH_DMA_MINALIGN - 1;
	uint8_t image_comp = -1, type = -1;
	const void *data;
	bool external_data = false;

	if (IS_ENABLED(CONFIG_SPL_FPGA_SUPPORT) ||
	    (IS_ENABLED(CONFIG_SPL_OS_BOOT) && IS_ENABLED(CONFIG_SPL_GZIP))) {
		if (fit_image_get_type(fit, node, &type))
			puts("Cannot get image type.\n");
		else
 			debug("%s ", genimg_get_type_name(type));
 	}
 
	if (IS_ENABLED(CONFIG_SPL_OS_BOOT) && IS_ENABLED(CONFIG_SPL_GZIP)) {
		if (fit_image_get_comp(fit, node, &image_comp))
			puts("Cannot get image compression format.\n");
		else
			debug("%s ", genimg_get_comp_name(image_comp));
 	}
 
 	if (fit_image_get_load(fit, node, &load_addr))
		load_addr = image_info->load_addr;

	if (!fit_image_get_data_position(fit, node, &offset)) {
		external_data = true;
	} else if (!fit_image_get_data_offset(fit, node, &offset)) {
		offset += base_offset;
		external_data = true;
	}

	if (external_data) {
		/* External data */
		if (fit_image_get_data_size(fit, node, &len))
			return -ENOENT;

		load_ptr = (load_addr + align_len) & ~align_len;
		length = len;

		overhead = get_aligned_image_overhead(info, offset);
		nr_sectors = get_aligned_image_size(info, length, offset);

		if (info->read(info,
			       sector + get_aligned_image_offset(info, offset),
			       nr_sectors, (void *)load_ptr) != nr_sectors)
			return -EIO;

		debug("External data: dst=%lx, offset=%x, size=%lx\n",
		      load_ptr, offset, (unsigned long)length);
		src = (void *)load_ptr + overhead;
	} else {
		/* Embedded data */
		if (fit_image_get_data(fit, node, &data, &length)) {
			puts("Cannot get image data/size\n");
			return -ENOENT;
		}
		debug("Embedded data: dst=%lx, size=%lx\n", load_addr,
		      (unsigned long)length);
		src = (void *)data;
	}

#ifdef CONFIG_SPL_FIT_SIGNATURE
	printf("## Checking hash(es) for Image %s ... ",
	       fit_get_name(fit, node, NULL));
	if (!fit_image_verify_with_data(fit, node,
					 src, length))
		return -EPERM;
	puts("OK\n");
#endif

#ifdef CONFIG_SPL_FIT_IMAGE_POST_PROCESS
	board_fit_image_post_process(&src, &length);
#endif

	if (IS_ENABLED(CONFIG_SPL_GZIP) && image_comp == IH_COMP_GZIP) {
		size = length;
		if (gunzip((void *)load_addr, CONFIG_SYS_BOOTM_LEN,
			   src, &size)) {
			puts("Uncompressing error\n");
			return -EIO;
		}
		length = size;
	} else {
		memcpy((void *)load_addr, src, length);
	}

	if (image_info) {
		image_info->load_addr = load_addr;
		image_info->size = length;
		image_info->entry_point = fdt_getprop_u32(fit, node, "entry");
	}

	return 0;
}
