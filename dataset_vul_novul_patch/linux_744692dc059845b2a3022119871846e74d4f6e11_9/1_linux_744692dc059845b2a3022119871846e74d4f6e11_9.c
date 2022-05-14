static ext4_io_end_t *ext4_init_io_end (struct inode *inode)
 {
 	ext4_io_end_t *io = NULL;
 
	io = kmalloc(sizeof(*io), GFP_NOFS);
 
 	if (io) {
 		igrab(inode);
 		io->inode = inode;
 		io->flag = 0;
 		io->offset = 0;
 		io->size = 0;
		io->error = 0;
 		INIT_WORK(&io->work, ext4_end_io_work);
 		INIT_LIST_HEAD(&io->list);
 	}

	return io;
}
