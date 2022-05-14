 void SchedulerHelper::SetTimeSourceForTesting(
    scoped_ptr<base::TickClock> time_source) {
   CheckOnValidThread();
   time_source_ = time_source.Pass();
 }
