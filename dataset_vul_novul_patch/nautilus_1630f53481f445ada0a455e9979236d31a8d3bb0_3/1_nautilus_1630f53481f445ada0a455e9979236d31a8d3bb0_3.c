mark_trusted_task_thread_func (GTask        *task,
                               gpointer      source_object,
                               gpointer      task_data,
                               GCancellable *cancellable)
 {
     MarkTrustedJob *job = task_data;
     CommonJob *common;

    common = (CommonJob *) job;
 
     nautilus_progress_info_start (job->common.progress);
 
    mark_desktop_file_trusted (common,
                               cancellable,
                               job->file,
                               job->interactive);
 }
