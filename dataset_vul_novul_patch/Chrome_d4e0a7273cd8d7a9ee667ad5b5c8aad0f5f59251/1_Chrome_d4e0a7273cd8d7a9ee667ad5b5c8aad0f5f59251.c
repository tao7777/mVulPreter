void FileManagerBrowserTestBase::SetUpCommandLine(
    base::CommandLine* command_line) {
  command_line->AppendSwitch(switches::kDisableAudioOutput);

  if (!GetRequiresStartupBrowser()) {
    command_line->AppendSwitch(switches::kNoStartupWindow);

    set_exit_when_last_browser_closes(false);
  }

  if (IsGuestModeTest()) {
    command_line->AppendSwitch(chromeos::switches::kGuestSession);
    command_line->AppendSwitchNative(chromeos::switches::kLoginUser, "$guest");
    command_line->AppendSwitchASCII(chromeos::switches::kLoginProfile, "user");
    command_line->AppendSwitch(switches::kIncognito);
    set_chromeos_user_ = false;
  }

  if (IsIncognitoModeTest()) {
     command_line->AppendSwitch(switches::kIncognito);
   }
 
   std::vector<base::Feature> enabled_features;
   std::vector<base::Feature> disabled_features;
 
  if (!IsGuestModeTest()) {
    enabled_features.emplace_back(features::kCrostini);
  }

  if (!IsNativeSmbTest()) {
    disabled_features.emplace_back(features::kNativeSmb);
  }

  if (IsDriveFsTest()) {
    enabled_features.emplace_back(chromeos::features::kDriveFs);
  } else {
    disabled_features.emplace_back(chromeos::features::kDriveFs);
  }

  if (IsMyFilesVolume()) {
    enabled_features.emplace_back(chromeos::features::kMyFilesVolume);
  } else {
    disabled_features.emplace_back(chromeos::features::kMyFilesVolume);
  }

  if (IsArcTest()) {
    arc::SetArcAvailableCommandLineForTesting(command_line);
  }

  if (IsDocumentsProviderTest()) {
    enabled_features.emplace_back(
        arc::kEnableDocumentsProviderInFilesAppFeature);
  } else {
    disabled_features.emplace_back(
        arc::kEnableDocumentsProviderInFilesAppFeature);
  }

  feature_list_.InitWithFeatures(enabled_features, disabled_features);

  extensions::ExtensionApiTest::SetUpCommandLine(command_line);
}
