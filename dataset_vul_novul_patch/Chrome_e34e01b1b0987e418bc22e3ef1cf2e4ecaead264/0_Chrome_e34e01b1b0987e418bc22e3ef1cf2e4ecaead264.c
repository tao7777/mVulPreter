void RendererSchedulerImpl::OnShutdownTaskQueue(
    const scoped_refptr<MainThreadTaskQueue>& task_queue) {
  if (main_thread_only().was_shutdown)
    return;

  if (task_queue_throttler_)
    task_queue_throttler_->ShutdownTaskQueue(task_queue.get());

  if (task_runners_.erase(task_queue)) {
    switch (task_queue->queue_class()) {
       case MainThreadTaskQueue::QueueClass::kTimer:
         task_queue->RemoveTaskObserver(
             &main_thread_only().timer_task_cost_estimator);
        break;
       case MainThreadTaskQueue::QueueClass::kLoading:
         task_queue->RemoveTaskObserver(
             &main_thread_only().loading_task_cost_estimator);
        break;
       default:
         break;
     }
  }
}
