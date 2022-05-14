bool AwMainDelegate::BasicStartupComplete(int* exit_code) {
  content::SetContentClient(&content_client_);

  content::RegisterMediaUrlInterceptor(new AwMediaUrlInterceptor());

  BrowserViewRenderer::CalculateTileMemoryPolicy();

  ui::GestureConfiguration::GetInstance()
      ->set_fling_touchscreen_tap_suppression_enabled(false);

  base::CommandLine* cl = base::CommandLine::ForCurrentProcess();
  cl->AppendSwitch(cc::switches::kEnableBeginFrameScheduling);

  cl->AppendSwitch(switches::kDisableOverscrollEdgeEffect);

  cl->AppendSwitch(switches::kDisablePullToRefreshEffect);

  cl->AppendSwitch(switches::kDisableSharedWorkers);

  cl->AppendSwitch(switches::kDisableFileSystem);

  cl->AppendSwitch(switches::kDisableNotifications);

  cl->AppendSwitch(switches::kDisableWebRtcHWDecoding);
  cl->AppendSwitch(switches::kDisableAcceleratedVideoDecode);

  cl->AppendSwitch(switches::kEnableThreadedTextureMailboxes);

  cl->AppendSwitch(switches::kDisableScreenOrientationLock);

  cl->AppendSwitch(switches::kDisableSpeechAPI);

  cl->AppendSwitch(switches::kDisablePermissionsAPI);

  cl->AppendSwitch(switches::kEnableAggressiveDOMStorageFlushing);

  cl->AppendSwitch(switches::kDisablePresentationAPI);

  if (cl->GetSwitchValueASCII(switches::kProcessType).empty()) {

#ifdef __LP64__
    const char kNativesFileName[] = "assets/natives_blob_64.bin";
    const char kSnapshotFileName[] = "assets/snapshot_blob_64.bin";
#else
    const char kNativesFileName[] = "assets/natives_blob_32.bin";
    const char kSnapshotFileName[] = "assets/snapshot_blob_32.bin";
#endif // __LP64__
    CHECK(base::android::RegisterApkAssetWithGlobalDescriptors(
        kV8NativesDataDescriptor, kNativesFileName));
    CHECK(base::android::RegisterApkAssetWithGlobalDescriptors(
         kV8SnapshotDataDescriptor, kSnapshotFileName));
   }
 
  if (cl->HasSwitch(switches::kWebViewSandboxedRenderer)) {
     cl->AppendSwitch(switches::kInProcessGPU);
     cl->AppendSwitchASCII(switches::kRendererProcessLimit, "1");
   }

  return false;
}
