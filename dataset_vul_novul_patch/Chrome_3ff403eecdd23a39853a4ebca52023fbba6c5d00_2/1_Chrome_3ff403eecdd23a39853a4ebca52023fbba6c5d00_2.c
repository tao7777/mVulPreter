 void MessageLoop::RunTask(PendingTask* pending_task) {
  DCHECK(nestable_tasks_allowed_);
   current_pending_task_ = pending_task;
 
 #if defined(OS_WIN)
  DecrementHighResTaskCountIfNeeded(*pending_task);
#endif

  nestable_tasks_allowed_ = false;

  TRACE_TASK_EXECUTION("MessageLoop::RunTask", *pending_task);

  for (auto& observer : task_observers_)
    observer.WillProcessTask(*pending_task);
  task_annotator_.RunTask("MessageLoop::PostTask", pending_task);
  for (auto& observer : task_observers_)
    observer.DidProcessTask(*pending_task);

  nestable_tasks_allowed_ = true;

  current_pending_task_ = nullptr;
}
