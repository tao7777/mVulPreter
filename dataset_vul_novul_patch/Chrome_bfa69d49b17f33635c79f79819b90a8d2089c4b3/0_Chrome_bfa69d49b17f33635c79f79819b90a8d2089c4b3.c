void BrowserRenderProcessHost::PropogateBrowserCommandLineToRenderer(
    const CommandLine& browser_cmd,
    CommandLine* renderer_cmd) const {
  static const char* const switch_names[] = {
    switches::kRendererAssertTest,
    switches::kRendererCrashTest,
    switches::kRendererStartupDialog,
    switches::kNoSandbox,
    switches::kTestSandbox,
    switches::kEnableSeccompSandbox,
#if !defined (GOOGLE_CHROME_BUILD)
    switches::kInProcessPlugins,
#endif
    switches::kDomAutomationController,
    switches::kUserAgent,
    switches::kJavaScriptFlags,
    switches::kRecordMode,
    switches::kPlaybackMode,
    switches::kNoJsRandomness,
    switches::kDisableBreakpad,
    switches::kFullMemoryCrashReport,
    switches::kEnableLogging,
    switches::kDumpHistogramsOnExit,
    switches::kDisableLogging,
    switches::kLoggingLevel,
    switches::kDebugPrint,
    switches::kMemoryProfiling,
    switches::kEnableWatchdog,
    switches::kMessageLoopHistogrammer,
    switches::kEnableDCHECK,
    switches::kSilentDumpOnDCHECK,
    switches::kUseLowFragHeapCrt,
    switches::kEnableStatsTable,
    switches::kExperimentalSpellcheckerFeatures,
    switches::kDisableAudio,
    switches::kSimpleDataSource,
    switches::kEnableBenchmarking,
    switches::kInternalNaCl,
     switches::kEnableDatabases,
     switches::kDisableByteRangeSupport,
     switches::kEnableWebSockets,
    switches::kEnableDesktopNotifications,
 #if defined(OS_MACOSX)
     switches::kEnableSandboxLogging,
#endif
  };

  for (size_t i = 0; i < arraysize(switch_names); ++i) {
    if (browser_cmd.HasSwitch(switch_names[i])) {
      renderer_cmd->AppendSwitchWithValue(switch_names[i],
          browser_cmd.GetSwitchValue(switch_names[i]));
    }
  }
}
