int perf_event_overflow(struct perf_event *event, int nmi,
 			  struct perf_sample_data *data,
 			  struct pt_regs *regs)
 {
	return __perf_event_overflow(event, nmi, 1, data, regs);
 }
