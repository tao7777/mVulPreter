mark_trusted_task_done (GObject      *source_object,
mark_desktop_file_executable_task_done (GObject      *source_object,
                                        GAsyncResult *res,
                                        gpointer      user_data)
 {
     MarkTrustedJob *job = user_data;
 
    g_object_unref (job->file);

    if (job->done_callback)
    {
        job->done_callback (!job_aborted ((CommonJob *) job),
                            job->done_callback_data);
    }

    finalize_common ((CommonJob *) job);
}
