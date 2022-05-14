static int ext4_end_io_nolock(ext4_io_end_t *io)
{
	struct inode *inode = io->inode;
	loff_t offset = io->offset;
	ssize_t size = io->size;
	int ret = 0;

	ext4_debug("ext4_end_io_nolock: io 0x%p from inode %lu,list->next 0x%p,"
		   "list->prev 0x%p\n",
	           io, inode->i_ino, io->list.next, io->list.prev);

	if (list_empty(&io->list))
		return ret;

 	if (io->flag != EXT4_IO_UNWRITTEN)
 		return ret;
 
	ret = ext4_convert_unwritten_extents(inode, offset, size);
 	if (ret < 0) {
 		printk(KERN_EMERG "%s: failed to convert unwritten"
 			"extents to written extents, error is %d"
			" io is still on inode %lu aio dio list\n",
                       __func__, ret, inode->i_ino);
		return ret;
	}

	/* clear the DIO AIO unwritten flag */
	io->flag = 0;
	return ret;
}
