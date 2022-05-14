static int do_zfs_load(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	char *filename = NULL;
	int dev;
	int part;
 	ulong addr = 0;
 	disk_partition_t info;
 	struct blk_desc *dev_desc;
 	unsigned long count;
 	const char *addr_str;
 	struct zfs_file zfile;
	struct device_s vdev;

	if (argc < 3)
		return CMD_RET_USAGE;

	count = 0;
	addr = simple_strtoul(argv[3], NULL, 16);
	filename = env_get("bootfile");
	switch (argc) {
	case 3:
		addr_str = env_get("loadaddr");
		if (addr_str != NULL)
			addr = simple_strtoul(addr_str, NULL, 16);
		else
			addr = CONFIG_SYS_LOAD_ADDR;

		break;
	case 4:
		break;
	case 5:
		filename = argv[4];
		break;
	case 6:
		filename = argv[4];
		count = simple_strtoul(argv[5], NULL, 16);
		break;

	default:
		return cmd_usage(cmdtp);
	}

	if (!filename) {
		puts("** No boot file defined **\n");
		return 1;
	}

	part = blk_get_device_part_str(argv[1], argv[2], &dev_desc, &info, 1);
	if (part < 0)
		return 1;

	dev = dev_desc->devnum;
	printf("Loading file \"%s\" from %s device %d%c%c\n",
		filename, argv[1], dev,
		part ? ':' : ' ', part ? part + '0' : ' ');

	zfs_set_blk_dev(dev_desc, &info);
	vdev.part_length = info.size;

	memset(&zfile, 0, sizeof(zfile));
	zfile.device = &vdev;
	if (zfs_open(&zfile, filename)) {
		printf("** File not found %s **\n", filename);
		return 1;
	}

	if ((count < zfile.size) && (count != 0))
		zfile.size = (uint64_t)count;

	if (zfs_read(&zfile, (char *)addr, zfile.size) != zfile.size) {
		printf("** Unable to read \"%s\" from %s %d:%d **\n",
			   filename, argv[1], dev, part);
		zfs_close(&zfile);
		return 1;
	}

	zfs_close(&zfile);

	/* Loading ok, update default load address */
	image_load_addr = addr;

	printf("%llu bytes read\n", zfile.size);
	env_set_hex("filesize", zfile.size);

	return 0;
}
