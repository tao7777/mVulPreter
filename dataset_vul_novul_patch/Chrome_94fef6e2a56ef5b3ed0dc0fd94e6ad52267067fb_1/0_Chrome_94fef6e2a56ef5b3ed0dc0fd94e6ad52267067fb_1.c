bool TaskService::UnbindInstance() {
  {
    base::AutoLock lock(lock_);
    if (bound_instance_id_ == kInvalidInstanceId)
      return false;
    bound_instance_id_ = kInvalidInstanceId;

     DCHECK(default_task_runner_);
     default_task_runner_ = nullptr;
   }

  // But invoked tasks might be still running here. To ensure no task runs on
  // quitting this method, wait for all tasks to complete.
  base::AutoLock tasks_in_flight_auto_lock(tasks_in_flight_lock_);
  while (tasks_in_flight_ > 0)
    no_tasks_in_flight_cv_.Wait();

   return true;
 }
