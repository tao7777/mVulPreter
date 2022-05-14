void PixelBufferRasterWorkerPool::OnRasterTasksFinished() {
   if (!should_notify_client_if_no_tasks_are_pending_)
     return;
  raster_finished_task_pending_ = false;
 
  CheckForCompletedRasterTasks();
}
