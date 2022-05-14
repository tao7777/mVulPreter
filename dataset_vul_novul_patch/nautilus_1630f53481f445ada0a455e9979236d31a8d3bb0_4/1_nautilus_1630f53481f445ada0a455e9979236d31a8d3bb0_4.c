nautilus_file_mark_desktop_file_trusted (GFile              *file,
                                         GtkWindow          *parent_window,
                                         gboolean            interactive,
                                         NautilusOpCallback  done_callback,
                                         gpointer            done_callback_data)
 {
     GTask *task;
     MarkTrustedJob *job;

    job = op_job_new (MarkTrustedJob, parent_window);
    job->file = g_object_ref (file);
    job->interactive = interactive;
     job->done_callback = done_callback;
     job->done_callback_data = done_callback_data;
 
    task = g_task_new (NULL, NULL, mark_trusted_task_done, job);
     g_task_set_task_data (task, job, NULL);
    g_task_run_in_thread (task, mark_trusted_task_thread_func);
     g_object_unref (task);
 }
