SYSCALL_DEFINE5(osf_getsysinfo, unsigned long, op, void __user *, buffer,
		unsigned long, nbytes, int __user *, start, void __user *, arg)
{
	unsigned long w;
	struct percpu_struct *cpu;

	switch (op) {
	case GSI_IEEE_FP_CONTROL:
		/* Return current software fp control & status bits.  */
		/* Note that DU doesn't verify available space here.  */

 		w = current_thread_info()->ieee_state & IEEE_SW_MASK;
 		w = swcr_update_status(w, rdfpcr());
		if (put_user(w, (unsigned long __user *) buffer))
			return -EFAULT;
		return 0;

	case GSI_IEEE_STATE_AT_SIGNAL:
		/*
		 * Not sure anybody will ever use this weird stuff.  These
		 * ops can be used (under OSF/1) to set the fpcr that should
		 * be used when a signal handler starts executing.
		 */
		break;

 	case GSI_UACPROC:
		if (nbytes < sizeof(unsigned int))
			return -EINVAL;
 		w = (current_thread_info()->flags >> UAC_SHIFT) & UAC_BITMASK;
 		if (put_user(w, (unsigned int __user *)buffer))
 			return -EFAULT;
 		return 1;

	case GSI_PROC_TYPE:
		if (nbytes < sizeof(unsigned long))
			return -EINVAL;
		cpu = (struct percpu_struct*)
		  ((char*)hwrpb + hwrpb->processor_offset);
		w = cpu->type;
		if (put_user(w, (unsigned long  __user*)buffer))
			return -EFAULT;
 		return 1;
 
 	case GSI_GET_HWRPB:
		if (nbytes < sizeof(*hwrpb))
 			return -EINVAL;
 		if (copy_to_user(buffer, hwrpb, nbytes) != 0)
 			return -EFAULT;
		return 1;

	default:
		break;
	}

	return -EOPNOTSUPP;
}
