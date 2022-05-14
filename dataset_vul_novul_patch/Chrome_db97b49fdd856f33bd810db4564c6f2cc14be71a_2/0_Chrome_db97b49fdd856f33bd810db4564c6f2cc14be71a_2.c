PixelBufferRasterWorkerPool::PixelBufferRasterWorkerPool(
    ResourceProvider* resource_provider,
    ContextProvider* context_provider,
    size_t num_threads,
    size_t max_transfer_buffer_usage_bytes)
    : RasterWorkerPool(resource_provider, context_provider, num_threads),
      shutdown_(false),
      scheduled_raster_task_count_(0),
      bytes_pending_upload_(0),
      max_bytes_pending_upload_(max_transfer_buffer_usage_bytes),
      has_performed_uploads_since_last_flush_(false),
       check_for_completed_raster_tasks_pending_(false),
       should_notify_client_if_no_tasks_are_pending_(false),
       should_notify_client_if_no_tasks_required_for_activation_are_pending_(
          false),
      raster_finished_task_pending_(false),
      raster_required_for_activation_finished_task_pending_(false) {
 }
