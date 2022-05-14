 base::TimeTicks SchedulerHelper::Now() const {
  return time_source_->NowTicks();
 }
