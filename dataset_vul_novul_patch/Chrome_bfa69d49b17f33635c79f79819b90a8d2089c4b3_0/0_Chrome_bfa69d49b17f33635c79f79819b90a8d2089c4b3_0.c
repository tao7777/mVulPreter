WebPreferences RenderViewHostDelegateHelper::GetWebkitPrefs(
    PrefService* prefs, bool is_dom_ui) {

  WebPreferences web_prefs;

  web_prefs.fixed_font_family =
      prefs->GetString(prefs::kWebKitFixedFontFamily);
  web_prefs.serif_font_family =
      prefs->GetString(prefs::kWebKitSerifFontFamily);
  web_prefs.sans_serif_font_family =
      prefs->GetString(prefs::kWebKitSansSerifFontFamily);
  if (prefs->GetBoolean(prefs::kWebKitStandardFontIsSerif))
    web_prefs.standard_font_family = web_prefs.serif_font_family;
  else
    web_prefs.standard_font_family = web_prefs.sans_serif_font_family;
  web_prefs.cursive_font_family =
      prefs->GetString(prefs::kWebKitCursiveFontFamily);
  web_prefs.fantasy_font_family =
      prefs->GetString(prefs::kWebKitFantasyFontFamily);

  web_prefs.default_font_size =
      prefs->GetInteger(prefs::kWebKitDefaultFontSize);
  web_prefs.default_fixed_font_size =
      prefs->GetInteger(prefs::kWebKitDefaultFixedFontSize);
  web_prefs.minimum_font_size =
      prefs->GetInteger(prefs::kWebKitMinimumFontSize);
  web_prefs.minimum_logical_font_size =
      prefs->GetInteger(prefs::kWebKitMinimumLogicalFontSize);

  web_prefs.default_encoding =
      WideToASCII(prefs->GetString(prefs::kDefaultCharset));

  web_prefs.javascript_can_open_windows_automatically =
      prefs->GetBoolean(prefs::kWebKitJavascriptCanOpenWindowsAutomatically);
  web_prefs.dom_paste_enabled =
      prefs->GetBoolean(prefs::kWebKitDomPasteEnabled);
  web_prefs.shrinks_standalone_images_to_fit =
      prefs->GetBoolean(prefs::kWebKitShrinksStandaloneImagesToFit);
  web_prefs.inspector_settings = WideToUTF8(
      prefs->GetString(prefs::kWebKitInspectorSettings));

  {  // Command line switches are used for preferences with no user interface.
    const CommandLine& command_line = *CommandLine::ForCurrentProcess();
    web_prefs.developer_extras_enabled =
        !command_line.HasSwitch(switches::kDisableDevTools);
    web_prefs.javascript_enabled =
        !command_line.HasSwitch(switches::kDisableJavaScript) &&
        prefs->GetBoolean(prefs::kWebKitJavascriptEnabled);
    web_prefs.web_security_enabled =
        !command_line.HasSwitch(switches::kDisableWebSecurity) &&
        prefs->GetBoolean(prefs::kWebKitWebSecurityEnabled);
    web_prefs.plugins_enabled =
        !command_line.HasSwitch(switches::kDisablePlugins) &&
        prefs->GetBoolean(prefs::kWebKitPluginsEnabled);
    web_prefs.java_enabled =
        !command_line.HasSwitch(switches::kDisableJava) &&
        prefs->GetBoolean(prefs::kWebKitJavaEnabled);
    web_prefs.loads_images_automatically =
        !command_line.HasSwitch(switches::kDisableImages) &&
        prefs->GetBoolean(prefs::kWebKitLoadsImagesAutomatically);
    web_prefs.uses_page_cache =
        command_line.HasSwitch(switches::kEnableFastback);
    web_prefs.remote_fonts_enabled =
        command_line.HasSwitch(switches::kEnableRemoteFonts);
    web_prefs.xss_auditor_enabled =
        !command_line.HasSwitch(switches::kDisableXSSAuditor);
    web_prefs.application_cache_enabled =
        command_line.HasSwitch(switches::kEnableApplicationCache);

    web_prefs.local_storage_enabled =
      command_line.HasSwitch(switches::kEnableLocalStorage);
    web_prefs.databases_enabled =
      command_line.HasSwitch(switches::kEnableDatabases);
    web_prefs.session_storage_enabled =
       command_line.HasSwitch(switches::kEnableSessionStorage);
     web_prefs.experimental_webgl_enabled =
       command_line.HasSwitch(switches::kEnableExperimentalWebGL);
   }
 
   web_prefs.uses_universal_detector =
      prefs->GetBoolean(prefs::kWebKitUsesUniversalDetector);
  web_prefs.text_areas_are_resizable =
      prefs->GetBoolean(prefs::kWebKitTextAreasAreResizable);


  web_prefs.default_encoding =
      CharacterEncoding::GetCanonicalEncodingNameByAliasName(
          web_prefs.default_encoding);
  if (web_prefs.default_encoding.empty()) {
    prefs->ClearPref(prefs::kDefaultCharset);
    web_prefs.default_encoding = WideToASCII(
        prefs->GetString(prefs::kDefaultCharset));
  }
  DCHECK(!web_prefs.default_encoding.empty());

  if (is_dom_ui) {
    web_prefs.loads_images_automatically = true;
    web_prefs.javascript_enabled = true;
  }

  return web_prefs;
}
