 void TaskQueueManager::SetTimeSourceForTesting(
    scoped_ptr<TimeSource> time_source) {
   DCHECK(main_thread_checker_.CalledOnValidThread());
   time_source_ = time_source.Pass();
 }
