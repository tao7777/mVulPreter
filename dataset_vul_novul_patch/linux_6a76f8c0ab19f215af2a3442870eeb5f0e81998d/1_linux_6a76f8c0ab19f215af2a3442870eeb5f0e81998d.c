ftrace_regex_lseek(struct file *file, loff_t offset, int whence)
 {
 	loff_t ret;
 
	if (file->f_mode & FMODE_READ)
		ret = seq_lseek(file, offset, whence);
	else
		file->f_pos = ret = 1;

	return ret;
}
