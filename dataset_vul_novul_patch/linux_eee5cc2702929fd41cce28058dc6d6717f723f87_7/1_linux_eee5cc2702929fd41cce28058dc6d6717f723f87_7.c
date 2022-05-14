void mark_files_ro(struct super_block *sb)
{
	struct file *f;
	lg_global_lock(&files_lglock);
	do_file_list_for_each_entry(sb, f) {
		if (!file_count(f))
			continue;
		if (!(f->f_mode & FMODE_WRITE))
			continue;
		spin_lock(&f->f_lock);
		f->f_mode &= ~FMODE_WRITE;
		spin_unlock(&f->f_lock);
		if (file_check_writeable(f) != 0)
			continue;
		__mnt_drop_write(f->f_path.mnt);
		file_release_write(f);
	} while_file_list_for_each_entry;
	lg_global_unlock(&files_lglock);
}
