static void perf_remove_from_owner(struct perf_event *event)
{
	struct task_struct *owner;

	rcu_read_lock();
	owner = ACCESS_ONCE(event->owner);
	/*
	 * Matches the smp_wmb() in perf_event_exit_task(). If we observe
	 * !owner it means the list deletion is complete and we can indeed
	 * free this event, otherwise we need to serialize on
	 * owner->perf_event_mutex.
	 */
	smp_read_barrier_depends();
	if (owner) {
		/*
		 * Since delayed_put_task_struct() also drops the last
		 * task reference we can safely take a new reference
		 * while holding the rcu_read_lock().
		 */
		get_task_struct(owner);
	}
 	rcu_read_unlock();
 
 	if (owner) {
		/*
		 * If we're here through perf_event_exit_task() we're already
		 * holding ctx->mutex which would be an inversion wrt. the
		 * normal lock order.
		 *
		 * However we can safely take this lock because its the child
		 * ctx->mutex.
		 */
		mutex_lock_nested(&owner->perf_event_mutex, SINGLE_DEPTH_NESTING);

 		/*
 		 * We have to re-check the event->owner field, if it is cleared
 		 * we raced with perf_event_exit_task(), acquiring the mutex
		 * ensured they're done, and we can proceed with freeing the
		 * event.
		 */
		if (event->owner)
			list_del_init(&event->owner_entry);
		mutex_unlock(&owner->perf_event_mutex);
		put_task_struct(owner);
	}
}
