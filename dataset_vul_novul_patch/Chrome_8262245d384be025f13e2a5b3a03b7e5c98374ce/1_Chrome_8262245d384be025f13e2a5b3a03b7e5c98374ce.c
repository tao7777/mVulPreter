void ChromeContentBrowserClient::AppendExtraCommandLineSwitches(
    CommandLine* command_line, int child_process_id) {
#if defined(USE_LINUX_BREAKPAD)
  if (IsCrashReporterEnabled()) {
    command_line->AppendSwitchASCII(switches::kEnableCrashReporter,
        child_process_logging::GetClientId() + "," + base::GetLinuxDistro());
  }
#elif defined(OS_MACOSX)
  if (IsCrashReporterEnabled()) {
    command_line->AppendSwitchASCII(switches::kEnableCrashReporter,
                                    child_process_logging::GetClientId());
  }
#endif  // OS_MACOSX

  if (logging::DialogsAreSuppressed())
    command_line->AppendSwitch(switches::kNoErrorDialogs);

  std::string process_type =
      command_line->GetSwitchValueASCII(switches::kProcessType);
  const CommandLine& browser_command_line = *CommandLine::ForCurrentProcess();
  if (process_type == switches::kExtensionProcess ||
      process_type == switches::kRendererProcess) {
    FilePath user_data_dir =
        browser_command_line.GetSwitchValuePath(switches::kUserDataDir);
    if (!user_data_dir.empty())
      command_line->AppendSwitchPath(switches::kUserDataDir, user_data_dir);
#if defined(OS_CHROMEOS)
    const std::string& login_profile =
        browser_command_line.GetSwitchValueASCII(switches::kLoginProfile);
    if (!login_profile.empty())
      command_line->AppendSwitchASCII(switches::kLoginProfile, login_profile);
#endif

    RenderProcessHost* process = RenderProcessHost::FromID(child_process_id);

    PrefService* prefs = process->profile()->GetPrefs();
    if (prefs->HasPrefPath(prefs::kDisable3DAPIs) &&
        prefs->GetBoolean(prefs::kDisable3DAPIs)) {
      command_line->AppendSwitch(switches::kDisable3DAPIs);
    }

    if (!prefs->GetBoolean(prefs::kSafeBrowsingEnabled) ||
        !g_browser_process->safe_browsing_detection_service()) {
      command_line->AppendSwitch(switches::kDisableClientSidePhishingDetection);
    }

    static const char* const kSwitchNames[] = {
      switches::kAllowHTTPBackgroundPage,
      switches::kAllowScriptingGallery,
      switches::kAppsCheckoutURL,
      switches::kAppsGalleryURL,
      switches::kCloudPrintServiceURL,
      switches::kDebugPrint,
#if defined(GOOGLE_CHROME_BUILD) && !defined(OS_CHROMEOS) && !defined(OS_MACOSX)
      switches::kDisablePrintPreview,
#else
      switches::kEnablePrintPreview,
#endif
      switches::kDomAutomationController,
      switches::kDumpHistogramsOnExit,
      switches::kEnableClickToPlay,
      switches::kEnableCrxlessWebApps,
      switches::kEnableExperimentalExtensionApis,
      switches::kEnableInBrowserThumbnailing,
      switches::kEnableIPCFuzzing,
      switches::kEnableNaCl,
      switches::kEnableRemoting,
      switches::kEnableResourceContentSettings,
      switches::kEnableSearchProviderApiV2,
      switches::kEnableWatchdog,
      switches::kExperimentalSpellcheckerFeatures,
      switches::kMemoryProfiling,
      switches::kMessageLoopHistogrammer,
      switches::kPpapiFlashArgs,
      switches::kPpapiFlashInProcess,
      switches::kPpapiFlashPath,
      switches::kPpapiFlashVersion,
       switches::kProfilingAtStart,
       switches::kProfilingFile,
       switches::kProfilingFlush,
      switches::kRemoteShellPort,
       switches::kSilentDumpOnDCHECK,
     };
 
    command_line->CopySwitchesFrom(browser_command_line, kSwitchNames,
                                   arraysize(kSwitchNames));
  } else if (process_type == switches::kUtilityProcess) {
    if (browser_command_line.HasSwitch(
            switches::kEnableExperimentalExtensionApis)) {
      command_line->AppendSwitch(switches::kEnableExperimentalExtensionApis);
    }
  } else if (process_type == switches::kPluginProcess) {
    static const char* const kSwitchNames[] = {
  #if defined(OS_CHROMEOS)
      switches::kLoginProfile,
  #endif
      switches::kMemoryProfiling,
      switches::kSilentDumpOnDCHECK,
      switches::kUserDataDir,
    };

    command_line->CopySwitchesFrom(browser_command_line, kSwitchNames,
                                   arraysize(kSwitchNames));
  } else if (process_type == switches::kZygoteProcess) {
    static const char* const kSwitchNames[] = {
      switches::kEnableRemoting,
      switches::kUserDataDir,  // Make logs go to the right file.
      switches::kPpapiFlashInProcess,
      switches::kPpapiFlashPath,
      switches::kPpapiFlashVersion,
    };

    command_line->CopySwitchesFrom(browser_command_line, kSwitchNames,
                                   arraysize(kSwitchNames));
  }
}
