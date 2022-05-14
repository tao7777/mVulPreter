 void TaskService::RunTask(InstanceId instance_id,
                           RunnerId runner_id,
                           base::OnceClosure task) {
   {
    base::AutoLock tasks_in_flight_auto_lock(tasks_in_flight_lock_);
    ++tasks_in_flight_;
   }

  if (IsInstanceIdStillBound(instance_id))
    std::move(task).Run();

  {
    base::AutoLock tasks_in_flight_auto_lock(tasks_in_flight_lock_);
    --tasks_in_flight_;
    DCHECK_GE(tasks_in_flight_, 0);
    if (tasks_in_flight_ == 0)
      no_tasks_in_flight_cv_.Signal();
  }
}

bool TaskService::IsInstanceIdStillBound(InstanceId instance_id) {
  base::AutoLock lock(lock_);
  return instance_id == bound_instance_id_;
 }
