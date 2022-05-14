 static grub_err_t read_foo (struct grub_disk *disk, grub_disk_addr_t sector, grub_size_t size, char *buf) {
	if (disk != NULL) {
		const int blocksize = 512; // unhardcode 512
		int ret;
		RIOBind *iob = disk->data;
		if (bio) iob = bio;
		ret = iob->read_at (iob->io, delta+(blocksize*sector),
			(ut8*)buf, size*blocksize);
		if (ret == -1)
			return 1;
	} else eprintf ("oops. no disk\n");
	return 0; // 0 is ok
 }
