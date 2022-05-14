static void perf_event_for_each_child(struct perf_event *event,
					void (*func)(struct perf_event *))
{
 	struct perf_event *child;
 
 	WARN_ON_ONCE(event->ctx->parent_ctx);

 	mutex_lock(&event->child_mutex);
 	func(event);
 	list_for_each_entry(child, &event->child_list, child_list)
		func(child);
	mutex_unlock(&event->child_mutex);
}
