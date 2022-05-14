base::ProcessHandle StartProcessWithAccess(CommandLine* cmd_line,
                                           const FilePath& exposed_dir) {
  const CommandLine& browser_command_line = *CommandLine::ForCurrentProcess();
  content::ProcessType type;
  std::string type_str = cmd_line->GetSwitchValueASCII(switches::kProcessType);
  if (type_str == switches::kRendererProcess) {
    type = content::PROCESS_TYPE_RENDERER;
  } else if (type_str == switches::kPluginProcess) {
    type = content::PROCESS_TYPE_PLUGIN;
  } else if (type_str == switches::kWorkerProcess) {
    type = content::PROCESS_TYPE_WORKER;
  } else if (type_str == switches::kNaClLoaderProcess) {
    type = content::PROCESS_TYPE_NACL_LOADER;
  } else if (type_str == switches::kUtilityProcess) {
    type = content::PROCESS_TYPE_UTILITY;
  } else if (type_str == switches::kNaClBrokerProcess) {
    type = content::PROCESS_TYPE_NACL_BROKER;
  } else if (type_str == switches::kGpuProcess) {
    type = content::PROCESS_TYPE_GPU;
  } else if (type_str == switches::kPpapiPluginProcess) {
    type = content::PROCESS_TYPE_PPAPI_PLUGIN;
  } else if (type_str == switches::kPpapiBrokerProcess) {
    type = content::PROCESS_TYPE_PPAPI_BROKER;
  } else {
    NOTREACHED();
    return 0;
  }

  TRACE_EVENT_BEGIN_ETW("StartProcessWithAccess", 0, type_str);

  bool in_sandbox =
      (type != content::PROCESS_TYPE_NACL_BROKER) &&
      (type != content::PROCESS_TYPE_PLUGIN) &&
      (type != content::PROCESS_TYPE_PPAPI_BROKER);

  if ((type == content::PROCESS_TYPE_GPU) &&
      (cmd_line->HasSwitch(switches::kDisableGpuSandbox))) {
    in_sandbox = false;
    DVLOG(1) << "GPU sandbox is disabled";
  }

  if (browser_command_line.HasSwitch(switches::kNoSandbox) ||
      cmd_line->HasSwitch(switches::kNoSandbox)) {
    in_sandbox = false;
  }

#if !defined (GOOGLE_CHROME_BUILD)
  if (browser_command_line.HasSwitch(switches::kInProcessPlugins)) {
    in_sandbox = false;
  }
#endif
  if (!browser_command_line.HasSwitch(switches::kDisable3DAPIs) &&
      !browser_command_line.HasSwitch(switches::kDisableExperimentalWebGL) &&
      browser_command_line.HasSwitch(switches::kInProcessWebGL)) {
    in_sandbox = false;
  }

  if (browser_command_line.HasSwitch(switches::kChromeFrame)) {
    if (!cmd_line->HasSwitch(switches::kChromeFrame)) {
      cmd_line->AppendSwitch(switches::kChromeFrame);
    }
  }

  bool child_needs_help =
      DebugFlags::ProcessDebugFlags(cmd_line, type, in_sandbox);

  cmd_line->AppendArg(base::StringPrintf("/prefetch:%d", type));

  sandbox::ResultCode result;
  base::win::ScopedProcessInformation target;
  sandbox::TargetPolicy* policy = g_broker_services->CreatePolicy();

#if !defined(NACL_WIN64)  // We don't need this code on win nacl64.
  if (type == content::PROCESS_TYPE_PLUGIN &&
      !browser_command_line.HasSwitch(switches::kNoSandbox) &&
      content::GetContentClient()->SandboxPlugin(cmd_line, policy)) {
    in_sandbox = true;
  }
#endif

  if (!in_sandbox) {
     policy->Release();
     base::ProcessHandle process = 0;
     base::LaunchProcess(*cmd_line, base::LaunchOptions(), &process);
    g_broker_services->AddTargetPeer(process);
     return process;
   }
 
  if (type == content::PROCESS_TYPE_PLUGIN) {
    AddGenericDllEvictionPolicy(policy);
    AddPluginDllEvictionPolicy(policy);
  } else if (type == content::PROCESS_TYPE_GPU) {
    if (!AddPolicyForGPU(cmd_line, policy))
      return 0;
  } else {
    if (!AddPolicyForRenderer(policy))
      return 0;
    if (type == content::PROCESS_TYPE_RENDERER ||
        type == content::PROCESS_TYPE_WORKER) {
      AddBaseHandleClosePolicy(policy);

    } else if (type == content::PROCESS_TYPE_PPAPI_PLUGIN) {
      if (!AddPolicyForPepperPlugin(policy))
        return 0;
    }


    if (type_str != switches::kRendererProcess) {
      cmd_line->AppendSwitchASCII("ignored", " --type=renderer ");
    }
  }

  if (!exposed_dir.empty()) {
    result = policy->AddRule(sandbox::TargetPolicy::SUBSYS_FILES,
                             sandbox::TargetPolicy::FILES_ALLOW_ANY,
                             exposed_dir.value().c_str());
    if (result != sandbox::SBOX_ALL_OK)
      return 0;

    FilePath exposed_files = exposed_dir.AppendASCII("*");
    result = policy->AddRule(sandbox::TargetPolicy::SUBSYS_FILES,
                             sandbox::TargetPolicy::FILES_ALLOW_ANY,
                             exposed_files.value().c_str());
    if (result != sandbox::SBOX_ALL_OK)
      return 0;
  }

  if (!AddGenericPolicy(policy)) {
    NOTREACHED();
    return 0;
  }

  TRACE_EVENT_BEGIN_ETW("StartProcessWithAccess::LAUNCHPROCESS", 0, 0);

  result = g_broker_services->SpawnTarget(
      cmd_line->GetProgram().value().c_str(),
      cmd_line->GetCommandLineString().c_str(),
      policy, target.Receive());
  policy->Release();

  TRACE_EVENT_END_ETW("StartProcessWithAccess::LAUNCHPROCESS", 0, 0);

  if (sandbox::SBOX_ALL_OK != result) {
    DLOG(ERROR) << "Failed to launch process. Error: " << result;
    return 0;
  }

  if (type == content::PROCESS_TYPE_NACL_LOADER &&
      (base::win::OSInfo::GetInstance()->wow64_status() ==
          base::win::OSInfo::WOW64_DISABLED)) {
    const SIZE_T kOneGigabyte = 1 << 30;
    void* nacl_mem = VirtualAllocEx(target.process_handle(),
                                    NULL,
                                    kOneGigabyte,
                                    MEM_RESERVE,
                                    PAGE_NOACCESS);
    if (!nacl_mem) {
      DLOG(WARNING) << "Failed to reserve address space for Native Client";
    }
  }

  ResumeThread(target.thread_handle());

  if (child_needs_help)
    base::debug::SpawnDebuggerOnProcess(target.process_id());

  return target.TakeProcessHandle();
}
