const service_manager::Manifest& GetChromeContentBrowserOverlayManifest() {
  static base::NoDestructor<service_manager::Manifest> manifest {
    service_manager::ManifestBuilder()
        .ExposeCapability("gpu",
                          service_manager::Manifest::InterfaceList<
                              metrics::mojom::CallStackProfileCollector>())
        .ExposeCapability("renderer",
                          service_manager::Manifest::InterfaceList<
                              chrome::mojom::AvailableOfflineContentProvider,
                              chrome::mojom::CacheStatsRecorder,
                              chrome::mojom::NetBenchmarking,
                              data_reduction_proxy::mojom::DataReductionProxy,
                              metrics::mojom::CallStackProfileCollector,
#if defined(OS_WIN)
                              mojom::ModuleEventSink,
#endif
                              rappor::mojom::RapporRecorder,
                              safe_browsing::mojom::SafeBrowsing>())
        .RequireCapability("ash", "system_ui")
        .RequireCapability("ash", "test")
        .RequireCapability("ash", "display")
        .RequireCapability("assistant", "assistant")
        .RequireCapability("assistant_audio_decoder", "assistant:audio_decoder")
        .RequireCapability("chrome", "input_device_controller")
        .RequireCapability("chrome_printing", "converter")
        .RequireCapability("cups_ipp_parser", "ipp_parser")
        .RequireCapability("device", "device:fingerprint")
        .RequireCapability("device", "device:geolocation_config")
        .RequireCapability("device", "device:geolocation_control")
        .RequireCapability("device", "device:ip_geolocator")
        .RequireCapability("ime", "input_engine")
        .RequireCapability("mirroring", "mirroring")
        .RequireCapability("nacl_broker", "browser")
        .RequireCapability("nacl_loader", "browser")
        .RequireCapability("noop", "noop")
        .RequireCapability("patch", "patch_file")
        .RequireCapability("preferences", "pref_client")
        .RequireCapability("preferences", "pref_control")
        .RequireCapability("profile_import", "import")
        .RequireCapability("removable_storage_writer",
                           "removable_storage_writer")
        .RequireCapability("secure_channel", "secure_channel")
        .RequireCapability("ui", "ime_registrar")
        .RequireCapability("ui", "input_device_controller")
        .RequireCapability("ui", "window_manager")
        .RequireCapability("unzip", "unzip_file")
        .RequireCapability("util_win", "util_win")
        .RequireCapability("xr_device_service", "xr_device_provider")
        .RequireCapability("xr_device_service", "xr_device_test_hook")
#if defined(OS_CHROMEOS)
        .RequireCapability("multidevice_setup", "multidevice_setup")
#endif
        .ExposeInterfaceFilterCapability_Deprecated(
            "navigation:frame", "renderer",
            service_manager::Manifest::InterfaceList<
                autofill::mojom::AutofillDriver,
                autofill::mojom::PasswordManagerDriver,
                chrome::mojom::OfflinePageAutoFetcher,
#if defined(OS_CHROMEOS)
                 chromeos_camera::mojom::CameraAppHelper,
                 chromeos::cellular_setup::mojom::CellularSetup,
                 chromeos::crostini_installer::mojom::PageHandlerFactory,
                 chromeos::ime::mojom::InputEngineManager,
                 chromeos::machine_learning::mojom::PageHandler,
                 chromeos::media_perception::mojom::MediaPerception,
                chromeos::multidevice_setup::mojom::MultiDeviceSetup,
                chromeos::multidevice_setup::mojom::PrivilegedHostDeviceSetter,
                chromeos::network_config::mojom::CrosNetworkConfig,
                cros::mojom::CameraAppDeviceProvider,
#endif
                contextual_search::mojom::ContextualSearchJsApiService,
#if BUILDFLAG(ENABLE_EXTENSIONS)
                extensions::KeepAlive,
#endif
                media::mojom::MediaEngagementScoreDetailsProvider,
                media_router::mojom::MediaRouter,
                page_load_metrics::mojom::PageLoadMetrics,
                translate::mojom::ContentTranslateDriver,

                downloads::mojom::PageHandlerFactory,
                feed_internals::mojom::PageHandler,
                new_tab_page::mojom::PageHandlerFactory,
#if defined(OS_ANDROID)
                explore_sites_internals::mojom::PageHandler,
#else
                app_management::mojom::PageHandlerFactory,
#endif
#if defined(OS_WIN) || defined(OS_MACOSX) || defined(OS_LINUX) || \
    defined(OS_CHROMEOS)
                discards::mojom::DetailsProvider, discards::mojom::GraphDump,
#endif
#if defined(OS_CHROMEOS)
                add_supervision::mojom::AddSupervisionHandler,
#endif
                mojom::BluetoothInternalsHandler,
                mojom::InterventionsInternalsPageHandler,
                mojom::OmniboxPageHandler, mojom::ResetPasswordHandler,
                mojom::SiteEngagementDetailsProvider,
                mojom::UsbInternalsPageHandler,
                snippets_internals::mojom::PageHandlerFactory>())
        .PackageService(prefs::GetManifest())
#if defined(OS_CHROMEOS)
        .PackageService(chromeos::multidevice_setup::GetManifest())
#endif  // defined(OS_CHROMEOS)
        .Build()
  };
  return *manifest;
}
