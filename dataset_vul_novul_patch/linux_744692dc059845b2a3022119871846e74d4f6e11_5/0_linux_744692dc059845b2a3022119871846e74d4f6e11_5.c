 static void ext4_end_io_work(struct work_struct *work)
 {
	ext4_io_end_t		*io = container_of(work, ext4_io_end_t, work);
	struct inode		*inode = io->inode;
	struct ext4_inode_info	*ei = EXT4_I(inode);
	unsigned long		flags;
	int			ret;
 
 	mutex_lock(&inode->i_mutex);
 	ret = ext4_end_io_nolock(io);
	if (ret < 0) {
		mutex_unlock(&inode->i_mutex);
		return;
 	}

	spin_lock_irqsave(&ei->i_completed_io_lock, flags);
	if (!list_empty(&io->list))
		list_del_init(&io->list);
	spin_unlock_irqrestore(&ei->i_completed_io_lock, flags);
 	mutex_unlock(&inode->i_mutex);
	ext4_free_io_end(io);
 }
