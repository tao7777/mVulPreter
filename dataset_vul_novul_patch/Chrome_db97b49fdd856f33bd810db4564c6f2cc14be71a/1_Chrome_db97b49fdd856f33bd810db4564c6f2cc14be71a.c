void PixelBufferRasterWorkerPool::CheckForCompletedRasterTasks() {
  TRACE_EVENT0(
      "cc", "PixelBufferRasterWorkerPool::CheckForCompletedRasterTasks");

  DCHECK(should_notify_client_if_no_tasks_are_pending_);

  check_for_completed_raster_tasks_callback_.Cancel();
  check_for_completed_raster_tasks_pending_ = false;

  CheckForCompletedWorkerTasks();
  CheckForCompletedUploads();
  FlushUploads();

   bool will_notify_client_that_no_tasks_required_for_activation_are_pending =
       (should_notify_client_if_no_tasks_required_for_activation_are_pending_ &&
        !HasPendingTasksRequiredForActivation());
   bool will_notify_client_that_no_tasks_are_pending =
       (should_notify_client_if_no_tasks_are_pending_ &&
        !HasPendingTasks());
 
  should_notify_client_if_no_tasks_required_for_activation_are_pending_ &=
      !will_notify_client_that_no_tasks_required_for_activation_are_pending;
  should_notify_client_if_no_tasks_are_pending_ &=
      !will_notify_client_that_no_tasks_are_pending;

  scheduled_raster_task_count_ = 0;
  if (PendingRasterTaskCount())
    ScheduleMoreTasks();

  TRACE_EVENT_ASYNC_STEP_INTO1(
      "cc", "ScheduledTasks", this, StateName(),
      "state", TracedValue::FromValue(StateAsValue().release()));

  if (HasPendingTasks())
    ScheduleCheckForCompletedRasterTasks();

  if (will_notify_client_that_no_tasks_required_for_activation_are_pending) {
    DCHECK(std::find_if(raster_tasks_required_for_activation().begin(),
                        raster_tasks_required_for_activation().end(),
                        WasCanceled) ==
          raster_tasks_required_for_activation().end());
    client()->DidFinishRunningTasksRequiredForActivation();
  }
  if (will_notify_client_that_no_tasks_are_pending) {
    TRACE_EVENT_ASYNC_END0("cc", "ScheduledTasks", this);
    DCHECK(!HasPendingTasksRequiredForActivation());
    client()->DidFinishRunningTasks();
  }
}
