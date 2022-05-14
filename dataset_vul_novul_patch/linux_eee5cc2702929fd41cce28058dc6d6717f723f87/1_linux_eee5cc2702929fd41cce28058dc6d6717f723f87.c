static inline void __file_sb_list_add(struct file *file, struct super_block *sb)
{
	struct list_head *list;
#ifdef CONFIG_SMP
	int cpu;
	cpu = smp_processor_id();
	file->f_sb_list_cpu = cpu;
	list = per_cpu_ptr(sb->s_files, cpu);
#else
	list = &sb->s_files;
#endif
	list_add(&file->f_u.fu_list, list);
}
