 int flush_completed_IO(struct inode *inode)
 {
 	ext4_io_end_t *io;
 	int ret = 0;
 	int ret2 = 0;
 
	if (list_empty(&EXT4_I(inode)->i_completed_io_list))
 		return ret;
 
 	dump_completed_IO(inode);
	while (!list_empty(&EXT4_I(inode)->i_completed_io_list)){
		io = list_entry(EXT4_I(inode)->i_completed_io_list.next,
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
 		ret = ext4_end_io_nolock(io);
 		if (ret < 0)
 			ret2 = ret;
 		else
 			list_del_init(&io->list);
 	}
 	return (ret2 < 0) ? ret2 : 0;
 }
