 dotraplinkage void notrace do_int3(struct pt_regs *regs, long error_code)
 {
 #ifdef CONFIG_DYNAMIC_FTRACE
	/*
	 * ftrace must be first, everything else may cause a recursive crash.
	 * See note by declaration of modifying_ftrace_code in ftrace.c
	 */
	if (unlikely(atomic_read(&modifying_ftrace_code)) &&
	    ftrace_int3_handler(regs))
		return;
#endif
 	if (poke_int3_handler(regs))
 		return;
 
	/*
	 * Use ist_enter despite the fact that we don't use an IST stack.
	 * We can be called from a kprobe in non-CONTEXT_KERNEL kernel
	 * mode or even during context tracking state changes.
	 *
	 * This means that we can't schedule.  That's okay.
	 */
 	ist_enter(regs);
 	RCU_LOCKDEP_WARN(!rcu_is_watching(), "entry code didn't wake RCU");
 #ifdef CONFIG_KGDB_LOW_LEVEL_TRAP
	if (kgdb_ll_trap(DIE_INT3, "int3", regs, error_code, X86_TRAP_BP,
				SIGTRAP) == NOTIFY_STOP)
		goto exit;
#endif /* CONFIG_KGDB_LOW_LEVEL_TRAP */

#ifdef CONFIG_KPROBES
	if (kprobe_int3_handler(regs))
		goto exit;
#endif

	if (notify_die(DIE_INT3, "int3", regs, error_code, X86_TRAP_BP,
 			SIGTRAP) == NOTIFY_STOP)
 		goto exit;
 
 	cond_local_irq_enable(regs);
 	do_trap(X86_TRAP_BP, SIGTRAP, "int3", regs, error_code, NULL);
 	cond_local_irq_disable(regs);

 exit:
 	ist_exit(regs);
 }
