 update_max_tr(struct trace_array *tr, struct task_struct *tsk, int cpu)
 {
	struct ring_buffer *buf;
 	if (tr->stop_count)
 		return;
 
	WARN_ON_ONCE(!irqs_disabled());

	if (!tr->allocated_snapshot) {
		/* Only the nop tracer should hit this when disabling */
		WARN_ON_ONCE(tr->current_trace != &nop_trace);
		return;
	}
 
 	arch_spin_lock(&tr->max_lock);
 
	buf = tr->trace_buffer.buffer;
	tr->trace_buffer.buffer = tr->max_buffer.buffer;
	tr->max_buffer.buffer = buf;
 
 	__update_max_tr(tr, tsk, cpu);
 	arch_spin_unlock(&tr->max_lock);
}
