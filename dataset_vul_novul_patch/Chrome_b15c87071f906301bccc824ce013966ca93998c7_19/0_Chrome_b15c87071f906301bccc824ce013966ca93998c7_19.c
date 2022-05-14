bool WtsSessionProcessDelegate::Core::LaunchProcess(
    IPC::Listener* delegate,
    ScopedHandle* process_exit_event_out) {
  DCHECK(main_task_runner_->BelongsToCurrentThread());

  CommandLine command_line(CommandLine::NO_PROGRAM);
  if (launch_elevated_) {
    if (!job_.IsValid()) {
      return false;
    }

    FilePath daemon_binary;
    if (!GetInstalledBinaryPath(kDaemonBinaryName, &daemon_binary))
      return false;

    command_line.SetProgram(daemon_binary);
    command_line.AppendSwitchPath(kElevateSwitchName, binary_path_);

    CHECK(ResetEvent(process_exit_event_));
  } else {
    command_line.SetProgram(binary_path_);
   }
 
   std::string channel_name = GenerateIpcChannelName(this);
  ScopedHandle pipe;
  if (!CreateIpcChannel(channel_name, channel_security_, &pipe))
     return false;
 
  // Wrap the pipe into an IPC channel.
  scoped_ptr<IPC::ChannelProxy> channel(new IPC::ChannelProxy(
      IPC::ChannelHandle(pipe),
      IPC::Channel::MODE_SERVER,
      delegate,
      io_task_runner_));

   command_line.AppendSwitchNative(kDaemonPipeSwitchName,
                                  UTF8ToWide(channel_name));
  command_line.CopySwitchesFrom(*CommandLine::ForCurrentProcess(),
                                kCopiedSwitchNames,
                                arraysize(kCopiedSwitchNames));

  ScopedHandle worker_process;
  ScopedHandle worker_thread;
  if (!LaunchProcessWithToken(command_line.GetProgram(),
                              command_line.GetCommandLineString(),
                              session_token_,
                              false,
                              CREATE_SUSPENDED | CREATE_BREAKAWAY_FROM_JOB,
                              &worker_process,
                              &worker_thread)) {
    return false;
  }

  HANDLE local_process_exit_event;
  if (launch_elevated_) {
    if (!AssignProcessToJobObject(job_, worker_process)) {
      LOG_GETLASTERROR(ERROR)
          << "Failed to assign the worker to the job object";
      TerminateProcess(worker_process, CONTROL_C_EXIT);
      return false;
    }

    local_process_exit_event = process_exit_event_;
  } else {
    worker_process_ = worker_process.Pass();
    local_process_exit_event = worker_process_;
  }

  if (!ResumeThread(worker_thread)) {
    LOG_GETLASTERROR(ERROR) << "Failed to resume the worker thread";
    KillProcess(CONTROL_C_EXIT);
    return false;
  }

  ScopedHandle process_exit_event;
  if (!DuplicateHandle(GetCurrentProcess(),
                       local_process_exit_event,
                       GetCurrentProcess(),
                       process_exit_event.Receive(),
                       SYNCHRONIZE,
                       FALSE,
                       0)) {
    LOG_GETLASTERROR(ERROR) << "Failed to duplicate a handle";
    KillProcess(CONTROL_C_EXIT);
    return false;
   }
 
   channel_ = channel.Pass();
  pipe_ = pipe.Pass();
   *process_exit_event_out = process_exit_event.Pass();
   return true;
 }
