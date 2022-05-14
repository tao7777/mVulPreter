static void ext4_end_io_dio(struct kiocb *iocb, loff_t offset,
			    ssize_t size, void *private)
 {
         ext4_io_end_t *io_end = iocb->private;
 	struct workqueue_struct *wq;
 
 	/* if not async direct IO or dio with 0 bytes write, just return */
 	if (!io_end || !size)
		return;

	ext_debug("ext4_end_io_dio(): io_end 0x%p"
		  "for inode %lu, iocb 0x%p, offset %llu, size %llu\n",
 		  iocb->private, io_end->inode->i_ino, iocb, offset,
		  size);

	/* if not aio dio with unwritten extents, just free io and return */
	if (io_end->flag != EXT4_IO_UNWRITTEN){
		ext4_free_io_end(io_end);
		iocb->private = NULL;
		return;
	}
 
 	io_end->offset = offset;
 	io_end->size = size;
 	wq = EXT4_SB(io_end->inode->i_sb)->dio_unwritten_wq;
 
 	/* queue the work to convert unwritten extents to written */
 	queue_work(wq, &io_end->work);
 
 	/* Add the io_end to per-inode completed aio dio list*/
	list_add_tail(&io_end->list,
		 &EXT4_I(io_end->inode)->i_completed_io_list);
 	iocb->private = NULL;
 }
