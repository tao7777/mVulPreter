void PixelBufferRasterWorkerPool::OnRasterTasksRequiredForActivationFinished() {
   if (!should_notify_client_if_no_tasks_required_for_activation_are_pending_)
     return;
 
  CheckForCompletedRasterTasks();
}
