 static grub_err_t read_foo (struct grub_disk *disk, grub_disk_addr_t sector, grub_size_t size, char *buf) {
	if (!disk) {
		eprintf ("oops. no disk\n");
		return 1;
	}
	const int blocksize = 512; // TODO unhardcode 512
	RIOBind *iob = disk->data;
	if (bio) {
		iob = bio;
	}
	//printf ("io %p\n", file->root->iob.io);
	if (iob->read_at (iob->io, delta+(blocksize*sector), (ut8*)buf, size*blocksize) == -1) {
		return 1;
	}
	return 0;
 }
