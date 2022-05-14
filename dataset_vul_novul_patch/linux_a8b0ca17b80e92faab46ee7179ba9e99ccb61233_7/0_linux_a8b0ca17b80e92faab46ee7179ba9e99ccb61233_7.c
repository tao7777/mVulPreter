handle_associated_event(struct cpu_hw_events *cpuc,
	int idx, struct perf_sample_data *data, struct pt_regs *regs)
{
	struct perf_event *event = cpuc->events[idx];
	struct hw_perf_event *hwc = &event->hw;

	mipspmu_event_update(event, hwc, idx);
	data->period = event->hw.last_period;
 	if (!mipspmu_event_set_period(event, hwc, idx))
 		return;
 
	if (perf_event_overflow(event, data, regs))
 		mipspmu->disable_event(idx);
 }
