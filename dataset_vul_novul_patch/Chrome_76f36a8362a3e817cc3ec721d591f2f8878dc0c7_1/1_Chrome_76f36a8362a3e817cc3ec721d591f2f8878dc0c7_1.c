 void SchedulerHelper::SetTimeSourceForTesting(
    scoped_ptr<TimeSource> time_source) {
   CheckOnValidThread();
   time_source_ = time_source.Pass();
 }
