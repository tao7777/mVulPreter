 bool MessageLoop::DoWork() {
  if (!NestableTasksAllowed()) {
     return false;
   }

  for (;;) {
    ReloadWorkQueue();
    if (work_queue_.empty())
      break;

    do {
      PendingTask pending_task = std::move(work_queue_.front());
      work_queue_.pop();

      if (pending_task.task.IsCancelled()) {
#if defined(OS_WIN)
        DecrementHighResTaskCountIfNeeded(pending_task);
#endif
      } else if (!pending_task.delayed_run_time.is_null()) {
        int sequence_num = pending_task.sequence_num;
        TimeTicks delayed_run_time = pending_task.delayed_run_time;
        AddToDelayedWorkQueue(std::move(pending_task));
        if (delayed_work_queue_.top().sequence_num == sequence_num)
          pump_->ScheduleDelayedWork(delayed_run_time);
      } else {
        if (DeferOrRunPendingTask(std::move(pending_task)))
          return true;
      }
    } while (!work_queue_.empty());
  }

  return false;
 }
