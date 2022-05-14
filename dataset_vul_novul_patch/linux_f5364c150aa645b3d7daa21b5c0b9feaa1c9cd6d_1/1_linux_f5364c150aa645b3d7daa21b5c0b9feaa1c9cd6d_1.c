 static inline void schedule_debug(struct task_struct *prev)
 {
 #ifdef CONFIG_SCHED_STACK_END_CHECK
	BUG_ON(task_stack_end_corrupted(prev));
 #endif
 
 	if (unlikely(in_atomic_preempt_off())) {
		__schedule_bug(prev);
		preempt_count_set(PREEMPT_DISABLED);
	}
	rcu_sleep_check();

	profile_hit(SCHED_PROFILING, __builtin_return_address(0));

	schedstat_inc(this_rq(), sched_count);
}
