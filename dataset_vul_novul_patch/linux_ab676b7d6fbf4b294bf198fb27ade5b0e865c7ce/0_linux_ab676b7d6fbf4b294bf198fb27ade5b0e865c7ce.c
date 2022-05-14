 static int pagemap_open(struct inode *inode, struct file *file)
 {
	/* do not disclose physical addresses: attack vector */
	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;
 	pr_warn_once("Bits 55-60 of /proc/PID/pagemap entries are about "
 			"to stop being page-shift some time soon. See the "
 			"linux/Documentation/vm/pagemap.txt for details.\n");
	return 0;
}
