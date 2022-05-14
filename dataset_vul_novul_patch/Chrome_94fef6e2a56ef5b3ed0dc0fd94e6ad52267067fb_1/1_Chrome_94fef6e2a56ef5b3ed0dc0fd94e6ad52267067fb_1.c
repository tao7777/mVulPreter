bool TaskService::UnbindInstance() {
  {
    base::AutoLock lock(lock_);
    if (bound_instance_id_ == kInvalidInstanceId)
      return false;
    bound_instance_id_ = kInvalidInstanceId;

     DCHECK(default_task_runner_);
     default_task_runner_ = nullptr;
   }
  base::subtle::AutoWriteLock task_lock(task_lock_);
   return true;
 }
