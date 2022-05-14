bool WtsSessionProcessDelegate::Core::Initialize(uint32 session_id) {
  if (base::win::GetVersion() == base::win::VERSION_XP)
     launch_elevated_ = false;
 
   if (launch_elevated_) {
    // GetNamedPipeClientProcessId() is available starting from Vista.
    HMODULE kernel32 = ::GetModuleHandle(L"kernel32.dll");
    CHECK(kernel32 != NULL);

    get_named_pipe_client_pid_ =
        reinterpret_cast<GetNamedPipeClientProcessIdFn>(
            GetProcAddress(kernel32, "GetNamedPipeClientProcessId"));
    CHECK(get_named_pipe_client_pid_ != NULL);

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
