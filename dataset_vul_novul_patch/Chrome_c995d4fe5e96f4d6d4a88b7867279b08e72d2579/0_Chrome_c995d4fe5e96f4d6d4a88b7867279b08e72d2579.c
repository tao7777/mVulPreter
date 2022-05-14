 bool ChromeContentBrowserClient::IsDataSaverEnabled(
     content::BrowserContext* browser_context) {
  Profile* profile = Profile::FromBrowserContext(browser_context);
  return profile && data_reduction_proxy::DataReductionProxySettings::
                        IsDataSaverEnabledByUser(profile->GetPrefs());
 }
