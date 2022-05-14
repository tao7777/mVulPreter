 void TaskQueueManager::SetTimeSourceForTesting(
    scoped_ptr<base::TickClock> time_source) {
   DCHECK(main_thread_checker_.CalledOnValidThread());
   time_source_ = time_source.Pass();
 }
