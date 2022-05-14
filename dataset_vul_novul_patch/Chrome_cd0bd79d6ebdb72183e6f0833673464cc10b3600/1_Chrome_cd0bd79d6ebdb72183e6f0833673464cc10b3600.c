bool ChromeContentClient::SandboxPlugin(CommandLine* command_line,
                                        sandbox::TargetPolicy* policy) {
  std::wstring plugin_dll = command_line->
      GetSwitchValueNative(switches::kPluginPath);

  FilePath builtin_flash;
  if (!PathService::Get(chrome::FILE_FLASH_PLUGIN, &builtin_flash))
    return false;

  FilePath plugin_path(plugin_dll);
  if (plugin_path.BaseName() != builtin_flash.BaseName())
    return false;

  if (base::win::GetVersion() <= base::win::VERSION_XP ||
      CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kDisableFlashSandbox)) {
     return false;
   }
 
   if (policy->AddRule(sandbox::TargetPolicy::SUBSYS_NAMED_PIPES,
                       sandbox::TargetPolicy::NAMEDPIPES_ALLOW_ANY,
                      L"\\\\.\\pipe\\chrome.*") != sandbox::SBOX_ALL_OK) {
    NOTREACHED();
    return false;
  }

  if (LoadFlashBroker(plugin_path, command_line)) {
    policy->SetJobLevel(sandbox::JOB_UNPROTECTED, 0);
    policy->SetTokenLevel(sandbox::USER_RESTRICTED_SAME_ACCESS,
                          sandbox::USER_INTERACTIVE);
    if (base::win::GetVersion() == base::win::VERSION_VISTA) {
      ::ChangeWindowMessageFilter(WM_MOUSEWHEEL, MSGFLT_ADD);
      ::ChangeWindowMessageFilter(WM_APPCOMMAND, MSGFLT_ADD);
    }
    policy->SetIntegrityLevel(sandbox::INTEGRITY_LEVEL_LOW);
  } else {
    DLOG(WARNING) << "Failed to start flash broker";
    policy->SetJobLevel(sandbox::JOB_UNPROTECTED, 0);
    policy->SetTokenLevel(
        sandbox::USER_UNPROTECTED, sandbox::USER_UNPROTECTED);
  }

  return true;
}
