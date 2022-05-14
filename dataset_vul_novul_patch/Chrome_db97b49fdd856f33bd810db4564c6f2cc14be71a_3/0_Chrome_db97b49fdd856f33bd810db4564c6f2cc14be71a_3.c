void PixelBufferRasterWorkerPool::ScheduleMoreTasks() {
  TRACE_EVENT0("cc", "PixelBufferRasterWorkerPool::ScheduleMoreTasks");

  enum RasterTaskType {
    PREPAINT_TYPE = 0,
    REQUIRED_FOR_ACTIVATION_TYPE = 1,
    NUM_TYPES = 2
  };
  NodeVector tasks[NUM_TYPES];
  unsigned priority = 2u;  // 0-1 reserved for RasterFinished tasks.
  TaskGraph graph;

  size_t bytes_pending_upload = bytes_pending_upload_;
  bool did_throttle_raster_tasks = false;

  for (RasterTaskVector::const_iterator it = raster_tasks().begin();
       it != raster_tasks().end(); ++it) {
    internal::RasterWorkerPoolTask* task = it->get();

    TaskMap::iterator pixel_buffer_it = pixel_buffer_tasks_.find(task);
    if (pixel_buffer_it == pixel_buffer_tasks_.end())
      continue;

    if (task->HasFinishedRunning()) {
      DCHECK(std::find(completed_tasks_.begin(),
                       completed_tasks_.end(),
                       task) != completed_tasks_.end());
      continue;
    }

    size_t new_bytes_pending_upload = bytes_pending_upload;
    new_bytes_pending_upload += task->resource()->bytes();
    if (new_bytes_pending_upload > max_bytes_pending_upload_) {
      did_throttle_raster_tasks = true;
      break;
    }

    internal::WorkerPoolTask* pixel_buffer_task = pixel_buffer_it->second.get();

    if (pixel_buffer_task && pixel_buffer_task->HasCompleted()) {
      bytes_pending_upload = new_bytes_pending_upload;
      continue;
    }

    size_t scheduled_raster_task_count =
        tasks[PREPAINT_TYPE].container().size() +
        tasks[REQUIRED_FOR_ACTIVATION_TYPE].container().size();
    if (scheduled_raster_task_count >= kMaxScheduledRasterTasks) {
      did_throttle_raster_tasks = true;
      break;
    }

    bytes_pending_upload = new_bytes_pending_upload;

    RasterTaskType type = IsRasterTaskRequiredForActivation(task) ?
        REQUIRED_FOR_ACTIVATION_TYPE :
        PREPAINT_TYPE;

    if (pixel_buffer_task) {
      tasks[type].container().push_back(
          CreateGraphNodeForRasterTask(pixel_buffer_task,
                                       task->dependencies(),
                                       priority++,
                                       &graph));
      continue;
    }

    resource_provider()->AcquirePixelBuffer(task->resource()->id());

    uint8* buffer = resource_provider()->MapPixelBuffer(
        task->resource()->id());

    scoped_refptr<internal::WorkerPoolTask> new_pixel_buffer_task(
        new PixelBufferWorkerPoolTaskImpl(
            task,
            buffer,
            base::Bind(&PixelBufferRasterWorkerPool::OnRasterTaskCompleted,
                       base::Unretained(this),
                       make_scoped_refptr(task))));
    pixel_buffer_tasks_[task] = new_pixel_buffer_task;
    tasks[type].container().push_back(
        CreateGraphNodeForRasterTask(new_pixel_buffer_task.get(),
                                     task->dependencies(),
                                     priority++,
                                     &graph));
  }

  scoped_refptr<internal::WorkerPoolTask>
      new_raster_required_for_activation_finished_task;

  size_t scheduled_raster_task_required_for_activation_count =
        tasks[REQUIRED_FOR_ACTIVATION_TYPE].container().size();
  DCHECK_LE(scheduled_raster_task_required_for_activation_count,
            tasks_required_for_activation_.size());
  if (scheduled_raster_task_required_for_activation_count ==
      tasks_required_for_activation_.size() &&
       should_notify_client_if_no_tasks_required_for_activation_are_pending_) {
     new_raster_required_for_activation_finished_task =
         CreateRasterRequiredForActivationFinishedTask();
    raster_required_for_activation_finished_task_pending_ = true;
     internal::GraphNode* raster_required_for_activation_finished_node =
         CreateGraphNodeForTask(
             new_raster_required_for_activation_finished_task.get(),
            0u,  // Priority 0
            &graph);
    AddDependenciesToGraphNode(
        raster_required_for_activation_finished_node,
        tasks[REQUIRED_FOR_ACTIVATION_TYPE].container());
  }

  scoped_refptr<internal::WorkerPoolTask> new_raster_finished_task;

  size_t scheduled_raster_task_count =
      tasks[PREPAINT_TYPE].container().size() +
      tasks[REQUIRED_FOR_ACTIVATION_TYPE].container().size();
  DCHECK_LE(scheduled_raster_task_count, PendingRasterTaskCount());
   if (!did_throttle_raster_tasks &&
       should_notify_client_if_no_tasks_are_pending_) {
     new_raster_finished_task = CreateRasterFinishedTask();
    raster_finished_task_pending_ = true;
     internal::GraphNode* raster_finished_node =
         CreateGraphNodeForTask(new_raster_finished_task.get(),
                                1u,  // Priority 1
                               &graph);
    for (unsigned type = 0; type < NUM_TYPES; ++type) {
      AddDependenciesToGraphNode(
          raster_finished_node,
          tasks[type].container());
    }
  }

  SetTaskGraph(&graph);

  scheduled_raster_task_count_ = scheduled_raster_task_count;

  set_raster_finished_task(new_raster_finished_task);
  set_raster_required_for_activation_finished_task(
      new_raster_required_for_activation_finished_task);
}
