void PixelBufferRasterWorkerPool::OnRasterTasksFinished() {
   if (!should_notify_client_if_no_tasks_are_pending_)
     return;
 
  CheckForCompletedRasterTasks();
}
