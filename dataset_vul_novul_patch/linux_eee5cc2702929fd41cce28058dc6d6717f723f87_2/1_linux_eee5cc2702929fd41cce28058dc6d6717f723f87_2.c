void file_sb_list_add(struct file *file, struct super_block *sb)
{
	if (likely(!(file->f_mode & FMODE_WRITE)))
		return;
	if (!S_ISREG(file_inode(file)->i_mode))
		return;
	lg_local_lock(&files_lglock);
	__file_sb_list_add(file, sb);
	lg_local_unlock(&files_lglock);
}
