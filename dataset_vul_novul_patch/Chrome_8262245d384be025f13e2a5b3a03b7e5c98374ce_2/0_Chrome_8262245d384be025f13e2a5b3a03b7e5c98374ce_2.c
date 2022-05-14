void BrowserRenderProcessHost::PropagateBrowserCommandLineToRenderer(
    const CommandLine& browser_cmd,
    CommandLine* renderer_cmd) const {
  static const char* const kSwitchNames[] = {
    switches::kChromeFrame,
    switches::kDisable3DAPIs,
    switches::kDisableAcceleratedCompositing,
    switches::kDisableApplicationCache,
    switches::kDisableAudio,
    switches::kDisableBreakpad,
    switches::kDisableDataTransferItems,
    switches::kDisableDatabases,
    switches::kDisableDesktopNotifications,
    switches::kDisableDeviceOrientation,
    switches::kDisableFileSystem,
    switches::kDisableGeolocation,
    switches::kDisableGLMultisampling,
    switches::kDisableGLSLTranslator,
    switches::kDisableGpuVsync,
    switches::kDisableIndexedDatabase,
    switches::kDisableJavaScriptI18NAPI,
    switches::kDisableLocalStorage,
    switches::kDisableLogging,
    switches::kDisableSeccompSandbox,
    switches::kDisableSessionStorage,
    switches::kDisableSharedWorkers,
    switches::kDisableSpeechInput,
    switches::kDisableWebAudio,
    switches::kDisableWebSockets,
    switches::kEnableAdaptive,
    switches::kEnableBenchmarking,
    switches::kEnableDCHECK,
    switches::kEnableGPUServiceLogging,
    switches::kEnableGPUClientLogging,
    switches::kEnableLogging,
    switches::kEnableMediaStream,
    switches::kEnablePepperTesting,
#if defined(OS_MACOSX)
    switches::kEnableSandboxLogging,
#endif
    switches::kEnableSeccompSandbox,
    switches::kEnableStatsTable,
    switches::kEnableVideoFullscreen,
    switches::kEnableVideoLogging,
    switches::kFullMemoryCrashReport,
#if !defined (GOOGLE_CHROME_BUILD)
    switches::kInProcessPlugins,
#endif  // GOOGLE_CHROME_BUILD
    switches::kInProcessWebGL,
    switches::kJavaScriptFlags,
    switches::kLoggingLevel,
    switches::kLowLatencyAudio,
    switches::kNoJsRandomness,
    switches::kNoReferrers,
    switches::kNoSandbox,
    switches::kPlaybackMode,
     switches::kPpapiOutOfProcess,
     switches::kRecordMode,
     switches::kRegisterPepperPlugins,
    switches::kRemoteShellPort,
     switches::kRendererAssertTest,
 #if !defined(OFFICIAL_BUILD)
     switches::kRendererCheckFalseTest,
#endif  // !defined(OFFICIAL_BUILD)
    switches::kRendererCrashTest,
    switches::kRendererStartupDialog,
    switches::kShowPaintRects,
    switches::kSimpleDataSource,
    switches::kTestSandbox,
    switches::kUseGL,
    switches::kUserAgent,
    switches::kV,
    switches::kVideoThreads,
    switches::kVModule,
    switches::kWebCoreLogChannels,
  };
  renderer_cmd->CopySwitchesFrom(browser_cmd, kSwitchNames,
                                 arraysize(kSwitchNames));

  if (profile()->IsOffTheRecord() &&
      !browser_cmd.HasSwitch(switches::kDisableDatabases)) {
    renderer_cmd->AppendSwitch(switches::kDisableDatabases);
  }
}
