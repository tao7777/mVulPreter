 int flush_completed_IO(struct inode *inode)
 {
 	ext4_io_end_t *io;
	struct ext4_inode_info *ei = EXT4_I(inode);
	unsigned long flags;
 	int ret = 0;
 	int ret2 = 0;
 
	if (list_empty(&ei->i_completed_io_list))
 		return ret;
 
 	dump_completed_IO(inode);
	spin_lock_irqsave(&ei->i_completed_io_lock, flags);
	while (!list_empty(&ei->i_completed_io_list)){
		io = list_entry(ei->i_completed_io_list.next,
 				ext4_io_end_t, list);
 		/*
 		 * Calling ext4_end_io_nolock() to convert completed
		 * IO to written.
		 *
		 * When ext4_sync_file() is called, run_queue() may already
		 * about to flush the work corresponding to this io structure.
		 * It will be upset if it founds the io structure related
		 * to the work-to-be schedule is freed.
		 *
		 * Thus we need to keep the io structure still valid here after
		 * convertion finished. The io structure has a flag to
 		 * avoid double converting from both fsync and background work
 		 * queue work.
 		 */
		spin_unlock_irqrestore(&ei->i_completed_io_lock, flags);
 		ret = ext4_end_io_nolock(io);
		spin_lock_irqsave(&ei->i_completed_io_lock, flags);
 		if (ret < 0)
 			ret2 = ret;
 		else
 			list_del_init(&io->list);
 	}
	spin_unlock_irqrestore(&ei->i_completed_io_lock, flags);
 	return (ret2 < 0) ? ret2 : 0;
 }
