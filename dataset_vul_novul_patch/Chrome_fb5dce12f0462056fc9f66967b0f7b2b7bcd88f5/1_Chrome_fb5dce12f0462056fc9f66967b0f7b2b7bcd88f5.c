content::WebUIDataSource* CreateOobeUIDataSource(
    const base::DictionaryValue& localized_strings,
    const std::string& display_type) {
  content::WebUIDataSource* source =
      content::WebUIDataSource::Create(chrome::kChromeUIOobeHost);
  source->AddLocalizedStrings(localized_strings);
  source->SetJsonPath(kStringsJSPath);

  if (display_type == OobeUI::kOobeDisplay) {
    source->SetDefaultResource(IDR_OOBE_HTML);
    source->AddResourcePath(kOobeJSPath, IDR_OOBE_JS);
    source->AddResourcePath(kCustomElementsHTMLPath,
                            IDR_CUSTOM_ELEMENTS_OOBE_HTML);
    source->AddResourcePath(kCustomElementsJSPath, IDR_CUSTOM_ELEMENTS_OOBE_JS);
  } else {
    source->SetDefaultResource(IDR_LOGIN_HTML);
    source->AddResourcePath(kLoginJSPath, IDR_LOGIN_JS);
    source->AddResourcePath(kCustomElementsHTMLPath,
                            IDR_CUSTOM_ELEMENTS_LOGIN_HTML);
     source->AddResourcePath(kCustomElementsJSPath,
                             IDR_CUSTOM_ELEMENTS_LOGIN_JS);
   }
  source->AddResourcePath(kPolymerConfigJSPath, IDR_POLYMER_CONFIG_JS);
   source->AddResourcePath(kKeyboardUtilsJSPath, IDR_KEYBOARD_UTILS_JS);
   source->OverrideContentSecurityPolicyFrameSrc(
       base::StringPrintf(
          "frame-src chrome://terms/ %s/;",
          extensions::kGaiaAuthExtensionOrigin));
  source->OverrideContentSecurityPolicyObjectSrc("object-src *;");
  bool is_webview_signin_enabled = StartupUtils::IsWebviewSigninEnabled();
  source->AddResourcePath("gaia_auth_host.js", is_webview_signin_enabled ?
      IDR_GAIA_AUTH_AUTHENTICATOR_JS : IDR_GAIA_AUTH_HOST_JS);

  source->AddResourcePath(kEnrollmentHTMLPath,
                          is_webview_signin_enabled
                              ? IDR_OOBE_ENROLLMENT_WEBVIEW_HTML
                              : IDR_OOBE_ENROLLMENT_HTML);
  source->AddResourcePath(kEnrollmentCSSPath,
                          is_webview_signin_enabled
                              ? IDR_OOBE_ENROLLMENT_WEBVIEW_CSS
                              : IDR_OOBE_ENROLLMENT_CSS);
  source->AddResourcePath(kEnrollmentJSPath,
                          is_webview_signin_enabled
                              ? IDR_OOBE_ENROLLMENT_WEBVIEW_JS
                              : IDR_OOBE_ENROLLMENT_JS);

  if (display_type == OobeUI::kOobeDisplay) {
    source->AddResourcePath("Roboto-Thin.ttf", IDR_FONT_ROBOTO_THIN);
    source->AddResourcePath("Roboto-Light.ttf", IDR_FONT_ROBOTO_LIGHT);
    source->AddResourcePath("Roboto-Regular.ttf", IDR_FONT_ROBOTO_REGULAR);
    source->AddResourcePath("Roboto-Medium.ttf", IDR_FONT_ROBOTO_MEDIUM);
    source->AddResourcePath("Roboto-Bold.ttf", IDR_FONT_ROBOTO_BOLD);
  }

  return source;
}
