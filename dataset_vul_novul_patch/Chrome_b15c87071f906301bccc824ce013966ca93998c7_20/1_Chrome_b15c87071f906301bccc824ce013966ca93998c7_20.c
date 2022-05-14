 void WtsSessionProcessDelegate::Core::OnJobNotification(DWORD message,
                                                      DWORD pid) {
   DCHECK(main_task_runner_->BelongsToCurrentThread());
 
   switch (message) {
    case JOB_OBJECT_MSG_ACTIVE_PROCESS_ZERO:
      CHECK(SetEvent(process_exit_event_));
      break;

    case JOB_OBJECT_MSG_NEW_PROCESS:
      worker_process_.Set(OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid));
      break;
  }
}
