static void perf_event_reset(struct perf_event *event)
 {
 	(void)perf_event_read(event);
 	local64_set(&event->count, 0);
	perf_event_update_userpage(event);
}
