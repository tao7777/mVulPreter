bool WorkerProcessLauncherTest::LaunchProcess(
    IPC::Listener* delegate,
    ScopedHandle* process_exit_event_out) {
  process_exit_event_.Set(CreateEvent(NULL, TRUE, FALSE, NULL));
  if (!process_exit_event_.IsValid())
     return false;
 
   channel_name_ = GenerateIpcChannelName(this);
  if (!CreateIpcChannel(channel_name_, kIpcSecurityDescriptor, task_runner_,
                        delegate, &channel_server_)) {
     return false;
   }
 
  exit_code_ = STILL_ACTIVE;
   return DuplicateHandle(GetCurrentProcess(),
                          process_exit_event_,
                          GetCurrentProcess(),
                         process_exit_event_out->Receive(),
                         0,
                         FALSE,
                         DUPLICATE_SAME_ACCESS) != FALSE;
}
