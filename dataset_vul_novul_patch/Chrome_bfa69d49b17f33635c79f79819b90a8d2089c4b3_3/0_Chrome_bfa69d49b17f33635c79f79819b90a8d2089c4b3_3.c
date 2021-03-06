void WebPreferences::Apply(WebView* web_view) const {
  WebSettings* settings = web_view->settings();
  settings->setStandardFontFamily(WideToUTF16Hack(standard_font_family));
  settings->setFixedFontFamily(WideToUTF16Hack(fixed_font_family));
  settings->setSerifFontFamily(WideToUTF16Hack(serif_font_family));
  settings->setSansSerifFontFamily(WideToUTF16Hack(sans_serif_font_family));
  settings->setCursiveFontFamily(WideToUTF16Hack(cursive_font_family));
  settings->setFantasyFontFamily(WideToUTF16Hack(fantasy_font_family));
  settings->setDefaultFontSize(default_font_size);
  settings->setDefaultFixedFontSize(default_fixed_font_size);
  settings->setMinimumFontSize(minimum_font_size);
  settings->setMinimumLogicalFontSize(minimum_logical_font_size);
  settings->setDefaultTextEncodingName(ASCIIToUTF16(default_encoding));
  settings->setJavaScriptEnabled(javascript_enabled);
  settings->setWebSecurityEnabled(web_security_enabled);
  settings->setJavaScriptCanOpenWindowsAutomatically(
      javascript_can_open_windows_automatically);
  settings->setLoadsImagesAutomatically(loads_images_automatically);
  settings->setPluginsEnabled(plugins_enabled);
  settings->setDOMPasteAllowed(dom_paste_enabled);
  settings->setDeveloperExtrasEnabled(developer_extras_enabled);
  settings->setShrinksStandaloneImagesToFit(shrinks_standalone_images_to_fit);
  settings->setUsesEncodingDetector(uses_universal_detector);
  settings->setTextAreasAreResizable(text_areas_are_resizable);
  settings->setAllowScriptsToCloseWindows(allow_scripts_to_close_windows);
  if (user_style_sheet_enabled)
    settings->setUserStyleSheetLocation(user_style_sheet_location);
  else
    settings->setUserStyleSheetLocation(WebURL());
  settings->setUsesPageCache(uses_page_cache);
  settings->setDownloadableBinaryFontsEnabled(remote_fonts_enabled);
  settings->setXSSAuditorEnabled(xss_auditor_enabled);
  settings->setLocalStorageEnabled(local_storage_enabled);
  settings->setDatabasesEnabled(
       WebRuntimeFeatures::isDatabaseEnabled() || databases_enabled);
   settings->setSessionStorageEnabled(session_storage_enabled);
   settings->setOfflineWebApplicationCacheEnabled(application_cache_enabled);
 
  settings->setEditableLinkBehaviorNeverLive();

  settings->setFontRenderingModeNormal();
  settings->setJavaEnabled(java_enabled);

  settings->setShouldPaintCustomScrollbars(true);

  settings->setAllowUniversalAccessFromFileURLs(
      allow_universal_access_from_file_urls);

  settings->setTextDirectionSubmenuInclusionBehaviorNeverIncluded();

  settings->setExperimentalWebGLEnabled(experimental_webgl_enabled);

  web_view->setInspectorSettings(WebString::fromUTF8(inspector_settings));

  web_view->setTabsToLinks(tabs_to_links);
}
