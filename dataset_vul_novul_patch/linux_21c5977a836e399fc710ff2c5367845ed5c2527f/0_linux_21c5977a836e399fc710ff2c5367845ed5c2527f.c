SYSCALL_DEFINE2(osf_getdomainname, char __user *, name, int, namelen)
{
	unsigned len;
	int i;

	if (!access_ok(VERIFY_WRITE, name, namelen))
 		return -EFAULT;
 
 	len = namelen;
	if (len > 32)
 		len = 32;
 
 	down_read(&uts_sem);
	for (i = 0; i < len; ++i) {
		__put_user(utsname()->domainname[i], name + i);
		if (utsname()->domainname[i] == '\0')
			break;
	}
	up_read(&uts_sem);

	return 0;
}
