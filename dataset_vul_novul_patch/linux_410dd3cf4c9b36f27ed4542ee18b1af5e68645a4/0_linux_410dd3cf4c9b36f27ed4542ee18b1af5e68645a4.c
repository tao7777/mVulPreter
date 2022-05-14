struct inode *isofs_iget(struct super_block *sb,
struct inode *__isofs_iget(struct super_block *sb,
			   unsigned long block,
			   unsigned long offset,
			   int relocated)
 {
 	unsigned long hashval;
 	struct inode *inode;
	struct isofs_iget5_callback_data data;
	long ret;

	if (offset >= 1ul << sb->s_blocksize_bits)
		return ERR_PTR(-EINVAL);

	data.block = block;
	data.offset = offset;

	hashval = (block << sb->s_blocksize_bits) | offset;

	inode = iget5_locked(sb, hashval, &isofs_iget5_test,
				&isofs_iget5_set, &data);

	if (!inode)
 		return ERR_PTR(-ENOMEM);
 
 	if (inode->i_state & I_NEW) {
		ret = isofs_read_inode(inode, relocated);
 		if (ret < 0) {
 			iget_failed(inode);
 			inode = ERR_PTR(ret);
		} else {
			unlock_new_inode(inode);
		}
	}

	return inode;
}
