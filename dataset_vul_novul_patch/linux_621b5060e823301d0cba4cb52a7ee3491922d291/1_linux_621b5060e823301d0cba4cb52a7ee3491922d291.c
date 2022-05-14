int arch_dup_task_struct(struct task_struct *dst, struct task_struct *src)
{
	flush_fp_to_thread(src);
 	flush_altivec_to_thread(src);
 	flush_vsx_to_thread(src);
 	flush_spe_to_thread(src);
 
 	*dst = *src;
 
	clear_task_ebb(dst);

	return 0;
}
