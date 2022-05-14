bool WtsSessionProcessDelegate::Core::Initialize(uint32 session_id) {
  if (base::win::GetVersion() == base::win::VERSION_XP)
     launch_elevated_ = false;
 
   if (launch_elevated_) {
     process_exit_event_.Set(CreateEvent(NULL, TRUE, FALSE, NULL));
     if (!process_exit_event_.IsValid()) {
       LOG(ERROR) << "Failed to create a nameless event";
      return false;
    }

    io_task_runner_->PostTask(FROM_HERE,
                              base::Bind(&Core::InitializeJob, this));
  }

  return CreateSessionToken(session_id, &session_token_);
}
