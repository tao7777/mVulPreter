 void TaskService::RunTask(InstanceId instance_id,
                           RunnerId runner_id,
                           base::OnceClosure task) {
  base::subtle::AutoReadLock task_lock(task_lock_);
   {
    base::AutoLock lock(lock_);
    if (instance_id != bound_instance_id_)
      return;
   }
  std::move(task).Run();
 }
