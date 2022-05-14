 bool MessageLoop::DoDelayedWork(TimeTicks* next_delayed_work_time) {
  if (!NestableTasksAllowed() ||
       !SweepDelayedWorkQueueAndReturnTrueIfStillHasWork()) {
     recent_time_ = *next_delayed_work_time = TimeTicks();
     return false;
  }


  TimeTicks next_run_time = delayed_work_queue_.top().delayed_run_time;
  if (next_run_time > recent_time_) {
    recent_time_ = TimeTicks::Now();  // Get a better view of Now();
    if (next_run_time > recent_time_) {
      *next_delayed_work_time = next_run_time;
      return false;
    }
  }

  PendingTask pending_task =
      std::move(const_cast<PendingTask&>(delayed_work_queue_.top()));
  delayed_work_queue_.pop();

  if (SweepDelayedWorkQueueAndReturnTrueIfStillHasWork())
    *next_delayed_work_time = delayed_work_queue_.top().delayed_run_time;

  return DeferOrRunPendingTask(std::move(pending_task));
}
