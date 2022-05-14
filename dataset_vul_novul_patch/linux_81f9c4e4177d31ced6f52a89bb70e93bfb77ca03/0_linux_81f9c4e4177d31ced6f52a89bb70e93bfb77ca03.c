static void __local_bh_enable(unsigned int cnt)
 {
 	lockdep_assert_irqs_disabled();
 
	if (preempt_count() == cnt)
		trace_preempt_on(CALLER_ADDR0, get_lock_parent_ip());

 	if (softirq_count() == (cnt & SOFTIRQ_MASK))
 		trace_softirqs_on(_RET_IP_);

	__preempt_count_sub(cnt);
 }
