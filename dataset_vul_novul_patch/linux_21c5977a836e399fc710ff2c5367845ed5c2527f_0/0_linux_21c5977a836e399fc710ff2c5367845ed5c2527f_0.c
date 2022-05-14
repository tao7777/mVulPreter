SYSCALL_DEFINE3(osf_sysinfo, int, command, char __user *, buf, long, count)
{
	const char *sysinfo_table[] = {
		utsname()->sysname,
		utsname()->nodename,
		utsname()->release,
		utsname()->version,
		utsname()->machine,
		"alpha",	/* instruction set architecture */
		"dummy",	/* hardware serial number */
		"dummy",	/* hardware manufacturer */
		"dummy",	/* secure RPC domain */
	};
	unsigned long offset;
	const char *res;
	long len, err = -EINVAL;

	offset = command-1;
	if (offset >= ARRAY_SIZE(sysinfo_table)) {
		/* Digital UNIX has a few unpublished interfaces here */
		printk("sysinfo(%d)", command);
		goto out;
	}

 	down_read(&uts_sem);
 	res = sysinfo_table[offset];
 	len = strlen(res)+1;
	if ((unsigned long)len > (unsigned long)count)
 		len = count;
 	if (copy_to_user(buf, res, len))
 		err = -EFAULT;
	else
		err = 0;
	up_read(&uts_sem);
 out:
	return err;
}
