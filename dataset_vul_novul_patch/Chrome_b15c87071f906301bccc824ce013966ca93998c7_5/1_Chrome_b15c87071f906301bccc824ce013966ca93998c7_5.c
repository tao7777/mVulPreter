bool UnprivilegedProcessDelegate::LaunchProcess(
    IPC::Listener* delegate,
    ScopedHandle* process_exit_event_out) {
  DCHECK(main_task_runner_->BelongsToCurrentThread());
  std::string channel_name = GenerateIpcChannelName(this);

   ScopedHandle client;
   scoped_ptr<IPC::ChannelProxy> server;
  if (!CreateConnectedIpcChannel(channel_name, delegate, &client, &server))
     return false;
 
  std::string pipe_handle = base::StringPrintf(
      "%d", reinterpret_cast<ULONG_PTR>(client.Get()));

  CommandLine command_line(binary_path_);
  command_line.AppendSwitchASCII(kDaemonPipeSwitchName, pipe_handle);
  command_line.CopySwitchesFrom(*CommandLine::ForCurrentProcess(),
                                kCopiedSwitchNames,
                                arraysize(kCopiedSwitchNames));

  ScopedHandle worker_thread;
  worker_process_.Close();
  if (!LaunchProcessWithToken(command_line.GetProgram(),
                              command_line.GetCommandLineString(),
                              NULL,
                              true,
                              0,
                              &worker_process_,
                              &worker_thread)) {
    return false;
  }

  ScopedHandle process_exit_event;
  if (!DuplicateHandle(GetCurrentProcess(),
                       worker_process_,
                       GetCurrentProcess(),
                       process_exit_event.Receive(),
                       SYNCHRONIZE,
                       FALSE,
                       0)) {
    LOG_GETLASTERROR(ERROR) << "Failed to duplicate a handle";
    KillProcess(CONTROL_C_EXIT);
    return false;
  }

  channel_ = server.Pass();
  *process_exit_event_out = process_exit_event.Pass();
   return true;
 }
