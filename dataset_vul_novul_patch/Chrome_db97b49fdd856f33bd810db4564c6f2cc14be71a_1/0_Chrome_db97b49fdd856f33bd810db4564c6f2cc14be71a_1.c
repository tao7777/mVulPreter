void PixelBufferRasterWorkerPool::OnRasterTasksRequiredForActivationFinished() {
   if (!should_notify_client_if_no_tasks_required_for_activation_are_pending_)
     return;
  raster_required_for_activation_finished_task_pending_ = false;
 
  CheckForCompletedRasterTasks();
}
