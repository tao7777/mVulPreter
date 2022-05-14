static ext4_io_end_t *ext4_init_io_end (struct inode *inode)
static ext4_io_end_t *ext4_init_io_end (struct inode *inode, gfp_t flags)
 {
 	ext4_io_end_t *io = NULL;
 
	io = kmalloc(sizeof(*io), flags);
 
 	if (io) {
 		igrab(inode);
 		io->inode = inode;
 		io->flag = 0;
 		io->offset = 0;
 		io->size = 0;
		io->page = NULL;
 		INIT_WORK(&io->work, ext4_end_io_work);
 		INIT_LIST_HEAD(&io->list);
 	}

	return io;
}
