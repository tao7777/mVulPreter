void LoginUtilsImpl::CompleteOffTheRecordLogin(const GURL& start_url) {
  VLOG(1) << "Completing off the record login";

  UserManager::Get()->OffTheRecordUserLoggedIn();

  if (CrosLibrary::Get()->EnsureLoaded()) {
    static const char* kForwardSwitches[] = {
        switches::kEnableLogging,
        switches::kUserDataDir,
        switches::kScrollPixels,
        switches::kEnableGView,
        switches::kNoFirstRun,
        switches::kLoginProfile,
        switches::kEnableTabbedOptions,
        switches::kCompressSystemFeedback,
#if defined(USE_SECCOMP_SANDBOX)
        switches::kDisableSeccompSandbox,
#endif
    };
    const CommandLine& browser_command_line =
        *CommandLine::ForCurrentProcess();
    CommandLine command_line(browser_command_line.GetProgram());
    command_line.CopySwitchesFrom(browser_command_line,
                                  kForwardSwitches,
                                  arraysize(kForwardSwitches));
    command_line.AppendSwitch(switches::kGuestSession);
    command_line.AppendSwitch(switches::kIncognito);
    command_line.AppendSwitchASCII(switches::kLoggingLevel,
                                   kGuestModeLoggingLevel);
     command_line.AppendSwitchASCII(
         switches::kLoginUser,
         UserManager::Get()->logged_in_user().email());

     if (start_url.is_valid())
       command_line.AppendArg(start_url.spec());

    std::string cmd_line_str = command_line.command_line_string();
    // Special workaround for the arguments that should be quoted.
    // Copying switches won't be needed when Guest mode won't need restart
    // http://crosbug.com/6924
    if (browser_command_line.HasSwitch(switches::kRegisterPepperPlugins)) {
      cmd_line_str += base::StringPrintf(
          kSwitchFormatString,
          switches::kRegisterPepperPlugins,
          browser_command_line.GetSwitchValueNative(
              switches::kRegisterPepperPlugins).c_str());
    }

    CrosLibrary::Get()->GetLoginLibrary()->RestartJob(getpid(), cmd_line_str);
   }
 }
