 static void dump_completed_IO(struct inode * inode)
 {
 #ifdef	EXT4_DEBUG
 	struct list_head *cur, *before, *after;
 	ext4_io_end_t *io, *io0, *io1;
 
 	if (list_empty(&EXT4_I(inode)->i_completed_io_list)){
 		ext4_debug("inode %lu completed_io list is empty\n", inode->i_ino);
 		return;
 	}
 
 	ext4_debug("Dump inode %lu completed_io list \n", inode->i_ino);
 	list_for_each_entry(io, &EXT4_I(inode)->i_completed_io_list, list){
 		cur = &io->list;
 		before = cur->prev;
		io0 = container_of(before, ext4_io_end_t, list);
		after = cur->next;
		io1 = container_of(after, ext4_io_end_t, list);

 		ext4_debug("io 0x%p from inode %lu,prev 0x%p,next 0x%p\n",
 			    io, inode->i_ino, io0, io1);
 	}
 #endif
 }
