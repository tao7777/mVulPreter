void WebPreferences::Apply(WebView* web_view) const {
  WebSettings* settings = web_view->settings();
  ApplyFontsFromMap(standard_font_family_map, setStandardFontFamilyWrapper,
                    settings);
  ApplyFontsFromMap(fixed_font_family_map, setFixedFontFamilyWrapper, settings);
  ApplyFontsFromMap(serif_font_family_map, setSerifFontFamilyWrapper, settings);
  ApplyFontsFromMap(sans_serif_font_family_map, setSansSerifFontFamilyWrapper,
                    settings);
  ApplyFontsFromMap(cursive_font_family_map, setCursiveFontFamilyWrapper,
                    settings);
  ApplyFontsFromMap(fantasy_font_family_map, setFantasyFontFamilyWrapper,
                    settings);
  ApplyFontsFromMap(pictograph_font_family_map, setPictographFontFamilyWrapper,
                    settings);
  settings->setDefaultFontSize(default_font_size);
  settings->setDefaultFixedFontSize(default_fixed_font_size);
  settings->setMinimumFontSize(minimum_font_size);
  settings->setMinimumLogicalFontSize(minimum_logical_font_size);
  settings->setDefaultTextEncodingName(ASCIIToUTF16(default_encoding));
  settings->setApplyDefaultDeviceScaleFactorInCompositor(
      apply_default_device_scale_factor_in_compositor);
  settings->setApplyPageScaleFactorInCompositor(
      apply_page_scale_factor_in_compositor);
  settings->setPerTilePaintingEnabled(per_tile_painting_enabled);
  settings->setAcceleratedAnimationEnabled(accelerated_animation_enabled);
  settings->setJavaScriptEnabled(javascript_enabled);
  settings->setWebSecurityEnabled(web_security_enabled);
  settings->setJavaScriptCanOpenWindowsAutomatically(
      javascript_can_open_windows_automatically);
  settings->setLoadsImagesAutomatically(loads_images_automatically);
  settings->setImagesEnabled(images_enabled);
  settings->setPluginsEnabled(plugins_enabled);
  settings->setDOMPasteAllowed(dom_paste_enabled);
  settings->setDeveloperExtrasEnabled(developer_extras_enabled);
  settings->setNeedsSiteSpecificQuirks(site_specific_quirks_enabled);
  settings->setShrinksStandaloneImagesToFit(shrinks_standalone_images_to_fit);
  settings->setUsesEncodingDetector(uses_universal_detector);
  settings->setTextAreasAreResizable(text_areas_are_resizable);
  settings->setAllowScriptsToCloseWindows(allow_scripts_to_close_windows);
  if (user_style_sheet_enabled)
    settings->setUserStyleSheetLocation(user_style_sheet_location);
  else
    settings->setUserStyleSheetLocation(WebURL());
  settings->setAuthorAndUserStylesEnabled(author_and_user_styles_enabled);
  settings->setUsesPageCache(uses_page_cache);
  settings->setPageCacheSupportsPlugins(page_cache_supports_plugins);
  settings->setDownloadableBinaryFontsEnabled(remote_fonts_enabled);
  settings->setJavaScriptCanAccessClipboard(javascript_can_access_clipboard);
  settings->setXSSAuditorEnabled(xss_auditor_enabled);
  settings->setDNSPrefetchingEnabled(dns_prefetching_enabled);
  settings->setLocalStorageEnabled(local_storage_enabled);
  settings->setSyncXHRInDocumentsEnabled(sync_xhr_in_documents_enabled);
  WebRuntimeFeatures::enableDatabase(databases_enabled);
  settings->setOfflineWebApplicationCacheEnabled(application_cache_enabled);
  settings->setCaretBrowsingEnabled(caret_browsing_enabled);
  settings->setHyperlinkAuditingEnabled(hyperlink_auditing_enabled);
  settings->setCookieEnabled(cookie_enabled);

  settings->setEditableLinkBehaviorNeverLive();

  settings->setFrameFlatteningEnabled(frame_flattening_enabled);

  settings->setFontRenderingModeNormal();
  settings->setJavaEnabled(java_enabled);

  settings->setAllowUniversalAccessFromFileURLs(
      allow_universal_access_from_file_urls);
  settings->setAllowFileAccessFromFileURLs(allow_file_access_from_file_urls);

  settings->setTextDirectionSubmenuInclusionBehaviorNeverIncluded();

  settings->setWebAudioEnabled(webaudio_enabled);

  settings->setExperimentalWebGLEnabled(experimental_webgl_enabled);

  settings->setOpenGLMultisamplingEnabled(gl_multisampling_enabled);

  settings->setPrivilegedWebGLExtensionsEnabled(
      privileged_webgl_extensions_enabled);

  settings->setWebGLErrorsToConsoleEnabled(webgl_errors_to_console_enabled);

  settings->setShowDebugBorders(show_composited_layer_borders);

  settings->setShowFPSCounter(show_fps_counter);

  settings->setAcceleratedCompositingForOverflowScrollEnabled(
      accelerated_compositing_for_overflow_scroll_enabled);

  settings->setAcceleratedCompositingForScrollableFramesEnabled(
      accelerated_compositing_for_scrollable_frames_enabled);

  settings->setCompositedScrollingForFramesEnabled(
      composited_scrolling_for_frames_enabled);

  settings->setShowPlatformLayerTree(show_composited_layer_tree);

  settings->setShowPaintRects(show_paint_rects);

  settings->setRenderVSyncEnabled(render_vsync_enabled);

  settings->setAcceleratedCompositingEnabled(accelerated_compositing_enabled);

  settings->setAcceleratedCompositingForFixedPositionEnabled(
      fixed_position_compositing_enabled);

  settings->setAccelerated2dCanvasEnabled(accelerated_2d_canvas_enabled);

  settings->setDeferred2dCanvasEnabled(deferred_2d_canvas_enabled);

  settings->setAntialiased2dCanvasEnabled(!antialiased_2d_canvas_disabled);

  settings->setAcceleratedPaintingEnabled(accelerated_painting_enabled);

  settings->setAcceleratedFiltersEnabled(accelerated_filters_enabled);

  settings->setGestureTapHighlightEnabled(gesture_tap_highlight_enabled);

  settings->setAcceleratedCompositingFor3DTransformsEnabled(
      accelerated_compositing_for_3d_transforms_enabled);
  settings->setAcceleratedCompositingForVideoEnabled(
      accelerated_compositing_for_video_enabled);
  settings->setAcceleratedCompositingForAnimationEnabled(
      accelerated_compositing_for_animation_enabled);

  settings->setAcceleratedCompositingForPluginsEnabled(
      accelerated_compositing_for_plugins_enabled);

  settings->setAcceleratedCompositingForCanvasEnabled(
      experimental_webgl_enabled || accelerated_2d_canvas_enabled);

  settings->setMemoryInfoEnabled(memory_info_enabled);

  settings->setAsynchronousSpellCheckingEnabled(
      asynchronous_spell_checking_enabled);
  settings->setUnifiedTextCheckerEnabled(unified_textchecker_enabled);

  for (WebInspectorPreferences::const_iterator it = inspector_settings.begin();
       it != inspector_settings.end(); ++it)
    web_view->setInspectorSetting(WebString::fromUTF8(it->first),
                                  WebString::fromUTF8(it->second));

  web_view->setTabsToLinks(tabs_to_links);

  settings->setInteractiveFormValidationEnabled(true);

  settings->setFullScreenEnabled(fullscreen_enabled);
  settings->setAllowDisplayOfInsecureContent(allow_displaying_insecure_content);
  settings->setAllowRunningOfInsecureContent(allow_running_insecure_content);
  settings->setPasswordEchoEnabled(password_echo_enabled);
  settings->setShouldPrintBackgrounds(should_print_backgrounds);
  settings->setEnableScrollAnimator(enable_scroll_animator);
  settings->setVisualWordMovementEnabled(visual_word_movement_enabled);

  settings->setCSSStickyPositionEnabled(css_sticky_position_enabled);
  settings->setExperimentalCSSCustomFilterEnabled(css_shaders_enabled);
  settings->setExperimentalCSSVariablesEnabled(css_variables_enabled);
  settings->setExperimentalCSSGridLayoutEnabled(css_grid_layout_enabled);

  WebRuntimeFeatures::enableTouch(touch_enabled);
  settings->setDeviceSupportsTouch(device_supports_touch);
  settings->setDeviceSupportsMouse(device_supports_mouse);
  settings->setEnableTouchAdjustment(touch_adjustment_enabled);

  settings->setDefaultTileSize(
      WebSize(default_tile_width, default_tile_height));
  settings->setMaxUntiledLayerSize(
      WebSize(max_untiled_layer_width, max_untiled_layer_height));

  settings->setFixedPositionCreatesStackingContext(
      fixed_position_creates_stacking_context);

   settings->setDeferredImageDecodingEnabled(deferred_image_decoding_enabled);
   settings->setShouldRespectImageOrientation(should_respect_image_orientation);
 
  settings->setUnsafePluginPastingEnabled(false);
   settings->setEditingBehavior(
       static_cast<WebSettings::EditingBehavior>(editing_behavior));
 
  settings->setSupportsMultipleWindows(supports_multiple_windows);

  settings->setViewportEnabled(viewport_enabled);

#if defined(OS_ANDROID)
  settings->setAllowCustomScrollbarInMainFrame(false);
  settings->setTextAutosizingEnabled(text_autosizing_enabled);
  settings->setTextAutosizingFontScaleFactor(font_scale_factor);
  web_view->setIgnoreViewportTagMaximumScale(force_enable_zoom);
  settings->setAutoZoomFocusedNodeToLegibleScale(true);
  settings->setDoubleTapToZoomEnabled(true);
  settings->setMediaPlaybackRequiresUserGesture(
      user_gesture_required_for_media_playback);
#endif

  WebNetworkStateNotifier::setOnLine(is_online);
}
