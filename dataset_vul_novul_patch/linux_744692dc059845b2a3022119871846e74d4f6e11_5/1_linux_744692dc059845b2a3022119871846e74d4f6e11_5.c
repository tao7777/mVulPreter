 static void ext4_end_io_work(struct work_struct *work)
 {
	ext4_io_end_t *io  = container_of(work, ext4_io_end_t, work);
	struct inode *inode = io->inode;
	int ret = 0;
 
 	mutex_lock(&inode->i_mutex);
 	ret = ext4_end_io_nolock(io);
	if (ret >= 0) {
		if (!list_empty(&io->list))
			list_del_init(&io->list);
		ext4_free_io_end(io);
 	}
 	mutex_unlock(&inode->i_mutex);
 }
