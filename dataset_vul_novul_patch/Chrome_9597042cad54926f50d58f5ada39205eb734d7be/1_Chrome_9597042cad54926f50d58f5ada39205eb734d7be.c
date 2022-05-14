bool GpuProcessHost::LaunchGpuProcess(const std::string& channel_id) {
  if (!(gpu_enabled_ &&
      GpuDataManagerImpl::GetInstance()->ShouldUseSoftwareRendering()) &&
      !hardware_gpu_enabled_) {
    SendOutstandingReplies();
    return false;
  }

  const CommandLine& browser_command_line = *CommandLine::ForCurrentProcess();

  CommandLine::StringType gpu_launcher =
      browser_command_line.GetSwitchValueNative(switches::kGpuLauncher);

#if defined(OS_LINUX)
  int child_flags = gpu_launcher.empty() ? ChildProcessHost::CHILD_ALLOW_SELF :
                                           ChildProcessHost::CHILD_NORMAL;
#else
  int child_flags = ChildProcessHost::CHILD_NORMAL;
#endif

  FilePath exe_path = ChildProcessHost::GetChildPath(child_flags);
  if (exe_path.empty())
    return false;

  CommandLine* cmd_line = new CommandLine(exe_path);
  cmd_line->AppendSwitchASCII(switches::kProcessType, switches::kGpuProcess);
  cmd_line->AppendSwitchASCII(switches::kProcessChannelID, channel_id);

  if (kind_ == GPU_PROCESS_KIND_UNSANDBOXED)
    cmd_line->AppendSwitch(switches::kDisableGpuSandbox);

  static const char* const kSwitchNames[] = {
    switches::kDisableBreakpad,
    switches::kDisableGLMultisampling,
    switches::kDisableGpuSandbox,
    switches::kReduceGpuSandbox,
    switches::kDisableSeccompFilterSandbox,
    switches::kDisableGpuSwitching,
    switches::kDisableGpuVsync,
    switches::kDisableGpuWatchdog,
    switches::kDisableImageTransportSurface,
    switches::kDisableLogging,
    switches::kEnableGPUServiceLogging,
     switches::kEnableLogging,
 #if defined(OS_MACOSX)
     switches::kEnableSandboxLogging,
#endif
#if defined(OS_CHROMEOS)
    switches::kEnableVaapi,
 #endif
     switches::kGpuNoContextLost,
     switches::kGpuStartupDialog,
    switches::kLoggingLevel,
    switches::kNoSandbox,
    switches::kTestGLLib,
    switches::kTraceStartup,
    switches::kV,
    switches::kVModule,
  };
  cmd_line->CopySwitchesFrom(browser_command_line, kSwitchNames,
                             arraysize(kSwitchNames));
  cmd_line->CopySwitchesFrom(
      browser_command_line, switches::kGpuSwitches, switches::kNumGpuSwitches);

  content::GetContentClient()->browser()->AppendExtraCommandLineSwitches(
      cmd_line, process_->GetData().id);

  GpuDataManagerImpl::GetInstance()->AppendGpuCommandLine(cmd_line);

  if (cmd_line->HasSwitch(switches::kUseGL))
    software_rendering_ =
        (cmd_line->GetSwitchValueASCII(switches::kUseGL) == "swiftshader");

  UMA_HISTOGRAM_BOOLEAN("GPU.GPUProcessSoftwareRendering", software_rendering_);

  if (!gpu_launcher.empty())
    cmd_line->PrependWrapper(gpu_launcher);

  process_->Launch(
#if defined(OS_WIN)
      FilePath(),
#elif defined(OS_POSIX)
      false,  // Never use the zygote (GPU plugin can't be sandboxed).
      base::EnvironmentVector(),
#endif
      cmd_line);
  process_launched_ = true;

  UMA_HISTOGRAM_ENUMERATION("GPU.GPUProcessLifetimeEvents",
                            LAUNCHED, GPU_PROCESS_LIFETIME_EVENT_MAX);
  return true;
}
