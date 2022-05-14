TargetThread::TargetThread()
TargetThread::TargetThread(DynamicStackAllocationConfig allocation_config)
     : thread_started_event_(false, false), finish_event_(false, false),
