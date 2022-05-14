static void sample_hbp_handler(struct perf_event *bp, int nmi,
 			       struct perf_sample_data *data,
 			       struct pt_regs *regs)
 {
	printk(KERN_INFO "%s value is changed\n", ksym_name);
	dump_stack();
	printk(KERN_INFO "Dump stack from sample_hbp_handler\n");
}
