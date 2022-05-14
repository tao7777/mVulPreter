void ChromeMetricsServiceClient::RegisterMetricsServiceProviders() {
  PrefService* local_state = g_browser_process->local_state();

  metrics_service_->RegisterMetricsProvider(
      std::make_unique<SubprocessMetricsProvider>());

#if BUILDFLAG(ENABLE_EXTENSIONS)
  metrics_service_->RegisterMetricsProvider(
      std::make_unique<ExtensionsMetricsProvider>(metrics_state_manager_));
#endif

  metrics_service_->RegisterMetricsProvider(
      std::make_unique<metrics::NetworkMetricsProvider>(
          content::CreateNetworkConnectionTrackerAsyncGetter(),
          std::make_unique<metrics::NetworkQualityEstimatorProviderImpl>()));

  metrics_service_->RegisterMetricsProvider(
      std::make_unique<OmniboxMetricsProvider>(
          base::Bind(&chrome::IsIncognitoSessionActive)));

  metrics_service_->RegisterMetricsProvider(
      std::make_unique<ChromeStabilityMetricsProvider>(local_state));

   metrics_service_->RegisterMetricsProvider(
       std::make_unique<metrics::GPUMetricsProvider>());
 
   metrics_service_->RegisterMetricsProvider(
       std::make_unique<metrics::ScreenInfoMetricsProvider>());
 
  metrics_service_->RegisterMetricsProvider(CreateFileMetricsProvider(
      metrics_state_manager_->IsMetricsReportingEnabled()));

  metrics_service_->RegisterMetricsProvider(
      std::make_unique<metrics::DriveMetricsProvider>(
          chrome::FILE_LOCAL_STATE));

  metrics_service_->RegisterMetricsProvider(
      std::make_unique<metrics::CallStackProfileMetricsProvider>());

  metrics_service_->RegisterMetricsProvider(
      std::make_unique<metrics::SamplingMetricsProvider>());

  metrics_service_->RegisterMetricsProvider(
      std::make_unique<translate::TranslateRankerMetricsProvider>());

  metrics_service_->RegisterMetricsProvider(
      std::make_unique<metrics::ComponentMetricsProvider>(
          g_browser_process->component_updater()));

#if defined(OS_ANDROID)
  metrics_service_->RegisterMetricsProvider(
      std::make_unique<AndroidMetricsProvider>());
  metrics_service_->RegisterMetricsProvider(
      std::make_unique<PageLoadMetricsProvider>());
#endif  // defined(OS_ANDROID)

#if defined(OS_WIN)
  metrics_service_->RegisterMetricsProvider(
      std::make_unique<GoogleUpdateMetricsProviderWin>());

  base::FilePath user_data_dir;
  base::FilePath crash_dir;
  if (!base::PathService::Get(chrome::DIR_USER_DATA, &user_data_dir) ||
      !base::PathService::Get(chrome::DIR_CRASH_DUMPS, &crash_dir)) {
    user_data_dir = base::FilePath();
    crash_dir = base::FilePath();
  }
  metrics_service_->RegisterMetricsProvider(
      std::make_unique<browser_watcher::WatcherMetricsProviderWin>(
          chrome::GetBrowserExitCodesRegistryPath(), user_data_dir, crash_dir,
          base::Bind(&GetExecutableVersionDetails)));

  metrics_service_->RegisterMetricsProvider(
      std::make_unique<AntiVirusMetricsProvider>());
#endif  // defined(OS_WIN)

#if BUILDFLAG(ENABLE_PLUGINS)
  plugin_metrics_provider_ = new PluginMetricsProvider(local_state);
  metrics_service_->RegisterMetricsProvider(
      std::unique_ptr<metrics::MetricsProvider>(plugin_metrics_provider_));
#endif  // BUILDFLAG(ENABLE_PLUGINS)

#if defined(OS_CHROMEOS)
  metrics_service_->RegisterMetricsProvider(
      std::make_unique<ChromeOSMetricsProvider>());

  metrics_service_->RegisterMetricsProvider(
      std::make_unique<SigninStatusMetricsProviderChromeOS>());

  if (metrics::GetMetricsReportingDefaultState(local_state) ==
      metrics::EnableMetricsDefault::DEFAULT_UNKNOWN) {
    metrics::RecordMetricsReportingDefaultState(
        local_state, metrics::EnableMetricsDefault::OPT_OUT);
  }

  metrics_service_->RegisterMetricsProvider(
      std::make_unique<chromeos::PrinterMetricsProvider>());
#endif  // defined(OS_CHROMEOS)

#if !defined(OS_CHROMEOS)
  metrics_service_->RegisterMetricsProvider(
      SigninStatusMetricsProvider::CreateInstance(
          std::make_unique<ChromeSigninStatusMetricsProviderDelegate>()));
#endif  // !defined(OS_CHROMEOS)

  metrics_service_->RegisterMetricsProvider(
      std::make_unique<syncer::DeviceCountMetricsProvider>(
          base::Bind(&browser_sync::ChromeSyncClient::GetDeviceInfoTrackers)));

  metrics_service_->RegisterMetricsProvider(
      std::make_unique<HttpsEngagementMetricsProvider>());

  metrics_service_->RegisterMetricsProvider(
      std::make_unique<CertificateReportingMetricsProvider>());

#if !defined(OS_ANDROID) && !defined(OS_CHROMEOS)
  metrics_service_->RegisterMetricsProvider(
      std::make_unique<UpgradeMetricsProvider>());
#endif  //! defined(OS_ANDROID) && !defined(OS_CHROMEOS)

#if defined(OS_MACOSX)
  metrics_service_->RegisterMetricsProvider(
      std::make_unique<PowerMetricsProvider>());
#endif

#if BUILDFLAG(ENABLE_CROS_ASSISTANT)
  metrics_service_->RegisterMetricsProvider(
      std::make_unique<AssistantServiceMetricsProvider>());
#endif  // BUILDFLAG(ENABLE_CROS_ASSISTANT)
}
