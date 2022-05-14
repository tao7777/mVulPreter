int ChromeBrowserMainParts::PreCreateThreadsImpl() {
  TRACE_EVENT0("startup", "ChromeBrowserMainParts::PreCreateThreadsImpl")
  run_message_loop_ = false;
#if !defined(OS_ANDROID)
  chrome::MaybeShowInvalidUserDataDirWarningDialog();
#endif  // !defined(OS_ANDROID)
  if (!PathService::Get(chrome::DIR_USER_DATA, &user_data_dir_))
    return chrome::RESULT_CODE_MISSING_DATA;

  MediaCaptureDevicesDispatcher::GetInstance();

#if !defined(OS_ANDROID)
  process_singleton_.reset(new ChromeProcessSingleton(
      user_data_dir_, base::Bind(&ProcessSingletonNotificationCallback)));

  first_run::IsChromeFirstRun();
#endif  // !defined(OS_ANDROID)

  scoped_refptr<base::SequencedTaskRunner> local_state_task_runner =
      JsonPrefStore::GetTaskRunnerForFile(
          base::FilePath(chrome::kLocalStorePoolName),
          BrowserThread::GetBlockingPool());

  {
    TRACE_EVENT0("startup",
      "ChromeBrowserMainParts::PreCreateThreadsImpl:InitBrowswerProcessImpl");
    browser_process_.reset(new BrowserProcessImpl(local_state_task_runner.get(),
                                                  parsed_command_line()));
  }

  if (parsed_command_line().HasSwitch(switches::kEnableProfiling)) {
    TRACE_EVENT0("startup",
        "ChromeBrowserMainParts::PreCreateThreadsImpl:InitProfiling");
    std::string flag =
      parsed_command_line().GetSwitchValueASCII(switches::kEnableProfiling);
    tracked_objects::ThreadData::Status status =
          tracked_objects::ThreadData::PROFILING_ACTIVE;
    if (flag.compare("0") != 0)
      status = tracked_objects::ThreadData::DEACTIVATED;
    tracked_objects::ThreadData::InitializeAndSetTrackingStatus(status);
  }

  local_state_ = InitializeLocalState(
      local_state_task_runner.get(), parsed_command_line());

#if !defined(OS_ANDROID)
  master_prefs_.reset(new first_run::MasterPrefs);
  browser_creator_.reset(new StartupBrowserCreator);
  chrome::UMABrowsingActivityObserver::Init();
#endif  // !defined(OS_ANDROID)

#if !defined(OS_CHROMEOS)
  {
    TRACE_EVENT0("startup",
        "ChromeBrowserMainParts::PreCreateThreadsImpl:ConvertFlags");
    about_flags::PrefServiceFlagsStorage flags_storage_(
        g_browser_process->local_state());
    about_flags::ConvertFlagsToSwitches(&flags_storage_,
                                        base::CommandLine::ForCurrentProcess(),
                                        about_flags::kAddSentinels);
  }
#endif  // !defined(OS_CHROMEOS)

  local_state_->UpdateCommandLinePrefStore(
      new CommandLinePrefStore(base::CommandLine::ForCurrentProcess()));

  crash_keys::SetSwitchesFromCommandLine(
      base::CommandLine::ForCurrentProcess());

#if defined(OS_MACOSX)
  std::string locale =
      parameters().ui_task ? "en-US" : l10n_util::GetLocaleOverride();
  browser_process_->SetApplicationLocale(locale);
#else
  const std::string locale =
      local_state_->GetString(prefs::kApplicationLocale);


  TRACE_EVENT_BEGIN0("startup",
      "ChromeBrowserMainParts::PreCreateThreadsImpl:InitResourceBundle");
  const std::string loaded_locale =
      ui::ResourceBundle::InitSharedInstanceWithLocale(
          locale, NULL, ui::ResourceBundle::LOAD_COMMON_RESOURCES);
  TRACE_EVENT_END0("startup",
      "ChromeBrowserMainParts::PreCreateThreadsImpl:InitResourceBundle");

  if (loaded_locale.empty() &&
      !parsed_command_line().HasSwitch(switches::kNoErrorDialogs)) {
    ShowMissingLocaleMessageBox();
    return chrome::RESULT_CODE_MISSING_DATA;
  }
  CHECK(!loaded_locale.empty()) << "Locale could not be found for " << locale;
  browser_process_->SetApplicationLocale(loaded_locale);

  {
    TRACE_EVENT0("startup",
        "ChromeBrowserMainParts::PreCreateThreadsImpl:AddDataPack");
    base::FilePath resources_pack_path;
    PathService::Get(chrome::FILE_RESOURCES_PACK, &resources_pack_path);
#if defined(OS_ANDROID)
    ui::LoadMainAndroidPackFile("assets/resources.pak", resources_pack_path);
#else
    ResourceBundle::GetSharedInstance().AddDataPackFromPath(
        resources_pack_path, ui::SCALE_FACTOR_NONE);
#endif  // defined(OS_ANDROID)
  }
#endif  // defined(OS_MACOSX)

#if !defined(OS_ANDROID) && !defined(OS_CHROMEOS)
  if (first_run::IsChromeFirstRun()) {
    first_run::ProcessMasterPreferencesResult pmp_result =
        first_run::ProcessMasterPreferences(user_data_dir_,
                                            master_prefs_.get());
    if (pmp_result == first_run::EULA_EXIT_NOW)
      return chrome::RESULT_CODE_EULA_REFUSED;

    if (!parsed_command_line().HasSwitch(switches::kApp) &&
        !parsed_command_line().HasSwitch(switches::kAppId) &&
        !parsed_command_line().HasSwitch(switches::kShowAppList)) {
      AddFirstRunNewTabs(browser_creator_.get(), master_prefs_->new_tabs);
    }


     if (!master_prefs_->variations_seed.empty() ||
         !master_prefs_->compressed_variations_seed.empty()) {
       if (!master_prefs_->variations_seed.empty()) {
        local_state_->SetString(prefs::kVariationsSeed,
                              master_prefs_->variations_seed);
       }
       if (!master_prefs_->compressed_variations_seed.empty()) {
        local_state_->SetString(prefs::kVariationsCompressedSeed,
                                master_prefs_->compressed_variations_seed);
       }
       if (!master_prefs_->variations_seed_signature.empty()) {
        local_state_->SetString(prefs::kVariationsSeedSignature,
                                master_prefs_->variations_seed_signature);
       }
      local_state_->SetInt64(prefs::kVariationsSeedDate,
                              base::Time::Now().ToInternalValue());
     }
 
    if (!master_prefs_->suppress_default_browser_prompt_for_version.empty()) {
      local_state_->SetString(
          prefs::kBrowserSuppressDefaultBrowserPrompt,
          master_prefs_->suppress_default_browser_prompt_for_version);
    }

#if defined(OS_WIN)
    if (!master_prefs_->welcome_page_on_os_upgrade_enabled)
      local_state_->SetBoolean(prefs::kWelcomePageOnOSUpgradeEnabled, false);
#endif
  }
#endif  // !defined(OS_ANDROID) && !defined(OS_CHROMEOS)

#if defined(OS_LINUX) || defined(OS_OPENBSD) || defined(OS_MACOSX)
  base::debug::SetCrashKeyValue(crash_keys::kChannel,
                                chrome::GetChannelString());
#endif  // defined(OS_LINUX) || defined(OS_OPENBSD) || defined(OS_MACOSX)

  tracking_synchronizer_ = new metrics::TrackingSynchronizer(
      make_scoped_ptr(new base::DefaultTickClock()));

#if defined(OS_MACOSX)
  SecKeychainAddCallback(&KeychainCallback, 0, NULL);
#endif  // defined(OS_MACOSX)

#if defined(OS_CHROMEOS)
  chromeos::CrosSettings::Initialize();
#endif  // defined(OS_CHROMEOS)

  SetupMetricsAndFieldTrials();

  browser_process_->PreCreateThreads();

  return content::RESULT_CODE_NORMAL_EXIT;
}
