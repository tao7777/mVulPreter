void SetRuntimeFeaturesDefaultsAndUpdateFromArgs(
    const base::CommandLine& command_line) {
  bool enableExperimentalWebPlatformFeatures = command_line.HasSwitch(
      switches::kEnableExperimentalWebPlatformFeatures);
  if (enableExperimentalWebPlatformFeatures)
    WebRuntimeFeatures::EnableExperimentalFeatures(true);

  SetRuntimeFeatureDefaultsForPlatform();


  WebRuntimeFeatures::EnableOriginTrials(
      base::FeatureList::IsEnabled(features::kOriginTrials));

  if (!base::FeatureList::IsEnabled(features::kWebUsb))
    WebRuntimeFeatures::EnableWebUsb(false);

  WebRuntimeFeatures::EnableBlinkHeapIncrementalMarking(
      base::FeatureList::IsEnabled(features::kBlinkHeapIncrementalMarking));

  WebRuntimeFeatures::EnableBlinkHeapUnifiedGarbageCollection(
      base::FeatureList::IsEnabled(
          features::kBlinkHeapUnifiedGarbageCollection));

  if (base::FeatureList::IsEnabled(features::kBloatedRendererDetection))
    WebRuntimeFeatures::EnableBloatedRendererDetection(true);

  if (command_line.HasSwitch(switches::kDisableDatabases))
    WebRuntimeFeatures::EnableDatabase(false);

  if (command_line.HasSwitch(switches::kDisableNotifications)) {
    WebRuntimeFeatures::EnableNotifications(false);

    WebRuntimeFeatures::EnablePushMessaging(false);
  }

  if (!base::FeatureList::IsEnabled(features::kNotificationContentImage))
    WebRuntimeFeatures::EnableNotificationContentImage(false);

  WebRuntimeFeatures::EnableSharedArrayBuffer(
      base::FeatureList::IsEnabled(features::kSharedArrayBuffer) ||
      base::FeatureList::IsEnabled(features::kWebAssemblyThreads));

  if (command_line.HasSwitch(switches::kDisableSharedWorkers))
    WebRuntimeFeatures::EnableSharedWorker(false);

  if (command_line.HasSwitch(switches::kDisableSpeechAPI))
    WebRuntimeFeatures::EnableScriptedSpeech(false);

  if (command_line.HasSwitch(switches::kDisableFileSystem))
    WebRuntimeFeatures::EnableFileSystem(false);

  if (!command_line.HasSwitch(switches::kDisableAcceleratedJpegDecoding))
    WebRuntimeFeatures::EnableDecodeToYUV(true);

#if defined(SUPPORT_WEBGL2_COMPUTE_CONTEXT)
  if (command_line.HasSwitch(switches::kEnableWebGL2ComputeContext)) {
    WebRuntimeFeatures::EnableWebGL2ComputeContext(true);
  }
#endif

  if (command_line.HasSwitch(switches::kEnableWebGLDraftExtensions))
    WebRuntimeFeatures::EnableWebGLDraftExtensions(true);

  if (command_line.HasSwitch(switches::kEnableAutomation) ||
      command_line.HasSwitch(switches::kHeadless)) {
    WebRuntimeFeatures::EnableAutomationControlled(true);
  }

#if defined(OS_MACOSX)
  const bool enable_canvas_2d_image_chromium =
      command_line.HasSwitch(
          switches::kEnableGpuMemoryBufferCompositorResources) &&
      !command_line.HasSwitch(switches::kDisable2dCanvasImageChromium) &&
      !command_line.HasSwitch(switches::kDisableGpu) &&
      base::FeatureList::IsEnabled(features::kCanvas2DImageChromium);
#else
  constexpr bool enable_canvas_2d_image_chromium = false;
#endif
  WebRuntimeFeatures::EnableCanvas2dImageChromium(
      enable_canvas_2d_image_chromium);

#if defined(OS_MACOSX)
  const bool enable_web_gl_image_chromium =
      command_line.HasSwitch(
          switches::kEnableGpuMemoryBufferCompositorResources) &&
      !command_line.HasSwitch(switches::kDisableWebGLImageChromium) &&
      !command_line.HasSwitch(switches::kDisableGpu) &&
      base::FeatureList::IsEnabled(features::kWebGLImageChromium);
#else
  const bool enable_web_gl_image_chromium =
      command_line.HasSwitch(switches::kEnableWebGLImageChromium);
#endif
  WebRuntimeFeatures::EnableWebGLImageChromium(enable_web_gl_image_chromium);

  if (command_line.HasSwitch(switches::kForceOverlayFullscreenVideo))
    WebRuntimeFeatures::ForceOverlayFullscreenVideo(true);

  if (ui::IsOverlayScrollbarEnabled())
    WebRuntimeFeatures::EnableOverlayScrollbars(true);

  if (command_line.HasSwitch(switches::kEnablePreciseMemoryInfo))
    WebRuntimeFeatures::EnablePreciseMemoryInfo(true);

  if (command_line.HasSwitch(switches::kEnablePrintBrowser))
    WebRuntimeFeatures::EnablePrintBrowser(true);

  if (command_line.HasSwitch(switches::kEnableNetworkInformationDownlinkMax) ||
      enableExperimentalWebPlatformFeatures) {
    WebRuntimeFeatures::EnableNetInfoDownlinkMax(true);
  }

  if (command_line.HasSwitch(switches::kReducedReferrerGranularity))
    WebRuntimeFeatures::EnableReducedReferrerGranularity(true);

  if (command_line.HasSwitch(switches::kDisablePermissionsAPI))
    WebRuntimeFeatures::EnablePermissionsAPI(false);

  if (command_line.HasSwitch(switches::kDisableV8IdleTasks))
    WebRuntimeFeatures::EnableV8IdleTasks(false);
  else
    WebRuntimeFeatures::EnableV8IdleTasks(true);

  if (command_line.HasSwitch(switches::kEnableUnsafeWebGPU))
    WebRuntimeFeatures::EnableWebGPU(true);

  if (command_line.HasSwitch(switches::kEnableWebVR))
    WebRuntimeFeatures::EnableWebVR(true);

  if (base::FeatureList::IsEnabled(features::kWebXr))
    WebRuntimeFeatures::EnableWebXR(true);

  if (base::FeatureList::IsEnabled(features::kWebXrGamepadSupport))
    WebRuntimeFeatures::EnableWebXRGamepadSupport(true);

  if (base::FeatureList::IsEnabled(features::kWebXrHitTest))
    WebRuntimeFeatures::EnableWebXRHitTest(true);

  if (command_line.HasSwitch(switches::kDisablePresentationAPI))
    WebRuntimeFeatures::EnablePresentationAPI(false);

  if (command_line.HasSwitch(switches::kDisableRemotePlaybackAPI))
    WebRuntimeFeatures::EnableRemotePlaybackAPI(false);

  WebRuntimeFeatures::EnableSecMetadata(
      base::FeatureList::IsEnabled(features::kSecMetadata) ||
      enableExperimentalWebPlatformFeatures);

  WebRuntimeFeatures::EnableUserActivationV2(
      base::FeatureList::IsEnabled(features::kUserActivationV2));

  if (base::FeatureList::IsEnabled(features::kScrollAnchorSerialization))
    WebRuntimeFeatures::EnableScrollAnchorSerialization(true);

  if (command_line.HasSwitch(switches::kEnableBlinkGenPropertyTrees))
    WebRuntimeFeatures::EnableFeatureFromString("BlinkGenPropertyTrees", true);

  if (command_line.HasSwitch(switches::kEnableSlimmingPaintV2))
    WebRuntimeFeatures::EnableFeatureFromString("SlimmingPaintV2", true);

  WebRuntimeFeatures::EnablePassiveDocumentEventListeners(
      base::FeatureList::IsEnabled(features::kPassiveDocumentEventListeners));

  WebRuntimeFeatures::EnablePassiveDocumentWheelEventListeners(
      base::FeatureList::IsEnabled(
          features::kPassiveDocumentWheelEventListeners));

  WebRuntimeFeatures::EnableFeatureFromString(
      "FontCacheScaling",
      base::FeatureList::IsEnabled(features::kFontCacheScaling));

  WebRuntimeFeatures::EnableFeatureFromString(
      "FontSrcLocalMatching",
      base::FeatureList::IsEnabled(features::kFontSrcLocalMatching));

  WebRuntimeFeatures::EnableFeatureFromString(
      "FramebustingNeedsSameOriginOrUserGesture",
      base::FeatureList::IsEnabled(
          features::kFramebustingNeedsSameOriginOrUserGesture));

  if (command_line.HasSwitch(switches::kDisableBackgroundTimerThrottling))
    WebRuntimeFeatures::EnableTimerThrottlingForBackgroundTabs(false);

  WebRuntimeFeatures::EnableExpensiveBackgroundTimerThrottling(
      base::FeatureList::IsEnabled(
          features::kExpensiveBackgroundTimerThrottling));

  if (base::FeatureList::IsEnabled(features::kHeapCompaction))
    WebRuntimeFeatures::EnableHeapCompaction(true);

  WebRuntimeFeatures::EnableRenderingPipelineThrottling(
      base::FeatureList::IsEnabled(features::kRenderingPipelineThrottling));

  WebRuntimeFeatures::EnableTimerThrottlingForHiddenFrames(
      base::FeatureList::IsEnabled(features::kTimerThrottlingForHiddenFrames));

  if (base::FeatureList::IsEnabled(
          features::kSendBeaconThrowForBlobWithNonSimpleType))
    WebRuntimeFeatures::EnableSendBeaconThrowForBlobWithNonSimpleType(true);

#if defined(OS_ANDROID)
  if (command_line.HasSwitch(switches::kDisableMediaSessionAPI))
    WebRuntimeFeatures::EnableMediaSession(false);
#endif

  WebRuntimeFeatures::EnablePaymentRequest(
      base::FeatureList::IsEnabled(features::kWebPayments));

  if (base::FeatureList::IsEnabled(features::kServiceWorkerPaymentApps))
    WebRuntimeFeatures::EnablePaymentApp(true);

  WebRuntimeFeatures::EnableNetworkService(
      base::FeatureList::IsEnabled(network::features::kNetworkService));

  if (base::FeatureList::IsEnabled(features::kGamepadExtensions))
    WebRuntimeFeatures::EnableGamepadExtensions(true);

  if (base::FeatureList::IsEnabled(features::kGamepadVibration))
    WebRuntimeFeatures::EnableGamepadVibration(true);

  if (base::FeatureList::IsEnabled(features::kCompositeOpaqueFixedPosition))
    WebRuntimeFeatures::EnableFeatureFromString("CompositeOpaqueFixedPosition",
                                                true);

  if (!base::FeatureList::IsEnabled(features::kCompositeOpaqueScrollers))
    WebRuntimeFeatures::EnableFeatureFromString("CompositeOpaqueScrollers",
                                                false);
  if (base::FeatureList::IsEnabled(features::kCompositorTouchAction))
    WebRuntimeFeatures::EnableCompositorTouchAction(true);

  if (base::FeatureList::IsEnabled(features::kCSSFragmentIdentifiers))
    WebRuntimeFeatures::EnableCSSFragmentIdentifiers(true);

  if (!base::FeatureList::IsEnabled(features::kGenericSensor))
    WebRuntimeFeatures::EnableGenericSensor(false);

  if (base::FeatureList::IsEnabled(features::kGenericSensorExtraClasses))
    WebRuntimeFeatures::EnableGenericSensorExtraClasses(true);

  if (base::FeatureList::IsEnabled(network::features::kOutOfBlinkCORS))
    WebRuntimeFeatures::EnableOutOfBlinkCORS(true);

  WebRuntimeFeatures::EnableMediaCastOverlayButton(
      base::FeatureList::IsEnabled(media::kMediaCastOverlayButton));

  if (!base::FeatureList::IsEnabled(features::kBlockCredentialedSubresources)) {
    WebRuntimeFeatures::EnableFeatureFromString("BlockCredentialedSubresources",
                                                false);
  }

  if (base::FeatureList::IsEnabled(features::kRasterInducingScroll))
    WebRuntimeFeatures::EnableRasterInducingScroll(true);

  WebRuntimeFeatures::EnableFeatureFromString(
      "AllowContentInitiatedDataUrlNavigations",
      base::FeatureList::IsEnabled(
          features::kAllowContentInitiatedDataUrlNavigations));

#if defined(OS_ANDROID)
  if (base::FeatureList::IsEnabled(features::kWebNfc))
    WebRuntimeFeatures::EnableWebNfc(true);
#endif

  WebRuntimeFeatures::EnableWebAuth(
      base::FeatureList::IsEnabled(features::kWebAuth));

  WebRuntimeFeatures::EnableWebAuthGetTransports(
      base::FeatureList::IsEnabled(features::kWebAuthGetTransports) ||
      enableExperimentalWebPlatformFeatures);

  WebRuntimeFeatures::EnableClientPlaceholdersForServerLoFi(
      base::GetFieldTrialParamValue("PreviewsClientLoFi",
                                    "replace_server_placeholders") != "false");

  WebRuntimeFeatures::EnableResourceLoadScheduler(
      base::FeatureList::IsEnabled(features::kResourceLoadScheduler));

  if (base::FeatureList::IsEnabled(features::kLayeredAPI))
    WebRuntimeFeatures::EnableLayeredAPI(true);

  if (base::FeatureList::IsEnabled(blink::features::kLayoutNG))
    WebRuntimeFeatures::EnableLayoutNG(true);

  WebRuntimeFeatures::EnableLazyInitializeMediaControls(
      base::FeatureList::IsEnabled(features::kLazyInitializeMediaControls));

  WebRuntimeFeatures::EnableMediaEngagementBypassAutoplayPolicies(
      base::FeatureList::IsEnabled(
          media::kMediaEngagementBypassAutoplayPolicies));

  WebRuntimeFeatures::EnableOverflowIconsForMediaControls(
      base::FeatureList::IsEnabled(media::kOverflowIconsForMediaControls));

  WebRuntimeFeatures::EnableAllowActivationDelegationAttr(
      base::FeatureList::IsEnabled(features::kAllowActivationDelegationAttr));

  WebRuntimeFeatures::EnableModernMediaControls(
      base::FeatureList::IsEnabled(media::kUseModernMediaControls));

  WebRuntimeFeatures::EnableWorkStealingInScriptRunner(
      base::FeatureList::IsEnabled(features::kWorkStealingInScriptRunner));

  WebRuntimeFeatures::EnableScheduledScriptStreaming(
      base::FeatureList::IsEnabled(features::kScheduledScriptStreaming));

  WebRuntimeFeatures::EnableMergeBlockingNonBlockingPools(
      base::FeatureList::IsEnabled(base::kMergeBlockingNonBlockingPools));

  if (base::FeatureList::IsEnabled(features::kLazyFrameLoading))
    WebRuntimeFeatures::EnableLazyFrameLoading(true);
  if (base::FeatureList::IsEnabled(features::kLazyFrameVisibleLoadTimeMetrics))
    WebRuntimeFeatures::EnableLazyFrameVisibleLoadTimeMetrics(true);
  if (base::FeatureList::IsEnabled(features::kLazyImageLoading))
    WebRuntimeFeatures::EnableLazyImageLoading(true);
  if (base::FeatureList::IsEnabled(features::kLazyImageVisibleLoadTimeMetrics))
    WebRuntimeFeatures::EnableLazyImageVisibleLoadTimeMetrics(true);

   WebRuntimeFeatures::EnableV8ContextSnapshot(
       base::FeatureList::IsEnabled(features::kV8ContextSnapshot));
 
  WebRuntimeFeatures::EnableRequireCSSExtensionForFile(
      base::FeatureList::IsEnabled(features::kRequireCSSExtensionForFile));
   WebRuntimeFeatures::EnablePictureInPicture(
       base::FeatureList::IsEnabled(media::kPictureInPicture));
 
  WebRuntimeFeatures::EnableCacheInlineScriptCode(
      base::FeatureList::IsEnabled(features::kCacheInlineScriptCode));

  WebRuntimeFeatures::EnableIsolatedCodeCache(
      base::FeatureList::IsEnabled(net::features::kIsolatedCodeCache));

  if (base::FeatureList::IsEnabled(features::kSignedHTTPExchange)) {
    WebRuntimeFeatures::EnableSignedHTTPExchange(true);
    WebRuntimeFeatures::EnablePreloadImageSrcSetEnabled(true);
  }

  WebRuntimeFeatures::EnableNestedWorkers(
      base::FeatureList::IsEnabled(blink::features::kNestedWorkers));

  if (base::FeatureList::IsEnabled(
          features::kExperimentalProductivityFeatures)) {
    WebRuntimeFeatures::EnableExperimentalProductivityFeatures(true);
  }

  if (base::FeatureList::IsEnabled(features::kPageLifecycle))
    WebRuntimeFeatures::EnablePageLifecycle(true);

#if defined(OS_ANDROID)
  if (base::FeatureList::IsEnabled(features::kDisplayCutoutAPI) &&
      base::android::BuildInfo::GetInstance()->sdk_int() >=
          base::android::SDK_VERSION_P) {
    WebRuntimeFeatures::EnableDisplayCutoutAPI(true);
  }
#endif

  if (command_line.HasSwitch(switches::kEnableAccessibilityObjectModel))
    WebRuntimeFeatures::EnableAccessibilityObjectModel(true);

  if (base::FeatureList::IsEnabled(blink::features::kWritableFilesAPI))
    WebRuntimeFeatures::EnableFeatureFromString("WritableFiles", true);


  if (command_line.HasSwitch(
          switches::kDisableOriginTrialControlledBlinkFeatures)) {
    WebRuntimeFeatures::EnableOriginTrialControlledFeatures(false);
  }

  WebRuntimeFeatures::EnableAutoplayIgnoresWebAudio(
      base::FeatureList::IsEnabled(media::kAutoplayIgnoreWebAudio));

#if defined(OS_ANDROID)
  WebRuntimeFeatures::EnableMediaControlsExpandGesture(
      base::FeatureList::IsEnabled(media::kMediaControlsExpandGesture));
#endif

  for (const std::string& feature :
       FeaturesFromSwitch(command_line, switches::kEnableBlinkFeatures)) {
    WebRuntimeFeatures::EnableFeatureFromString(feature, true);
  }
  for (const std::string& feature :
       FeaturesFromSwitch(command_line, switches::kDisableBlinkFeatures)) {
    WebRuntimeFeatures::EnableFeatureFromString(feature, false);
  }

  WebRuntimeFeatures::EnablePortals(
      base::FeatureList::IsEnabled(blink::features::kPortals));

  if (!base::FeatureList::IsEnabled(features::kBackgroundFetch))
    WebRuntimeFeatures::EnableBackgroundFetch(false);

  WebRuntimeFeatures::EnableBackgroundFetchUploads(
      base::FeatureList::IsEnabled(features::kBackgroundFetchUploads));

  WebRuntimeFeatures::EnableNoHoverAfterLayoutChange(
      base::FeatureList::IsEnabled(features::kNoHoverAfterLayoutChange));

  WebRuntimeFeatures::EnableJankTracking(
      base::FeatureList::IsEnabled(blink::features::kJankTracking) ||
      enableExperimentalWebPlatformFeatures);

  WebRuntimeFeatures::EnableNoHoverDuringScroll(
      base::FeatureList::IsEnabled(features::kNoHoverDuringScroll));
}
