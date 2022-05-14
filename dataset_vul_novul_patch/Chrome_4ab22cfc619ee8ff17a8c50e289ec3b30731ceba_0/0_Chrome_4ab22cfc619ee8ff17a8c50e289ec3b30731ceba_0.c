void Automation::InitWithBrowserPath(const FilePath& browser_exe,
                                     const CommandLine& options,
                                     Error** error) {
  if (!file_util::PathExists(browser_exe)) {
    std::string message = base::StringPrintf(
        "Could not find Chrome binary at: %" PRFilePath,
        browser_exe.value().c_str());
    *error = new Error(kUnknownError, message);
    return;
  }

  CommandLine command(browser_exe);
  command.AppendSwitch(switches::kDisableHangMonitor);
  command.AppendSwitch(switches::kDisablePromptOnRepost);
  command.AppendSwitch(switches::kDomAutomationController);
  command.AppendSwitch(switches::kFullMemoryCrashReport);
  command.AppendSwitchASCII(switches::kHomePage, chrome::kAboutBlankURL);
  command.AppendSwitch(switches::kNoDefaultBrowserCheck);
  command.AppendSwitch(switches::kNoFirstRun);
  command.AppendSwitchASCII(switches::kTestType, "webdriver");

  command.AppendArguments(options, false);

  launcher_.reset(new AnonymousProxyLauncher(false));
  ProxyLauncher::LaunchState launch_props = {
      false,  // clear_profile
      FilePath(),  // template_user_data
      ProxyLauncher::DEFAULT_THEME,
      command,
      true,  // include_testing_id
      true   // show_window
  };

   std::string chrome_details = base::StringPrintf(
       "Using Chrome binary at: %" PRFilePath,
       browser_exe.value().c_str());
  LOG(INFO) << chrome_details;
 
   if (!launcher_->LaunchBrowserAndServer(launch_props, true)) {
     *error = new Error(
        kUnknownError,
        "Unable to either launch or connect to Chrome. Please check that "
            "ChromeDriver is up-to-date. " + chrome_details);
    return;
   }
 
   launcher_->automation()->set_action_timeout_ms(base::kNoTimeout);
  LOG(INFO) << "Chrome launched successfully. Version: "
            << automation()->server_version();
 
   bool has_automation_version = false;
   *error = CompareVersion(730, 0, &has_automation_version);
  if (*error)
    return;

  chrome_details += ", version (" + automation()->server_version() + ")";
  if (has_automation_version) {
    int version = 0;
    std::string error_msg;
    if (!SendGetChromeDriverAutomationVersion(
            automation(), &version, &error_msg)) {
      *error = new Error(kUnknownError, error_msg + " " + chrome_details);
      return;
    }
    if (version > automation::kChromeDriverAutomationVersion) {
      *error = new Error(
          kUnknownError,
          "ChromeDriver is not compatible with this version of Chrome. " +
              chrome_details);
      return;
    }
  }
}
