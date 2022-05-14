static inline int file_list_cpu(struct file *file)
{
#ifdef CONFIG_SMP
	return file->f_sb_list_cpu;
#else
	return smp_processor_id();
#endif
}
