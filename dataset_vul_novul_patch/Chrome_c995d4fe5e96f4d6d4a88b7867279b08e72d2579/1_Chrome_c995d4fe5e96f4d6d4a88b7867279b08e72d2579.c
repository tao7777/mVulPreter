 bool ChromeContentBrowserClient::IsDataSaverEnabled(
     content::BrowserContext* browser_context) {
  data_reduction_proxy::DataReductionProxySettings*
      data_reduction_proxy_settings =
          DataReductionProxyChromeSettingsFactory::GetForBrowserContext(
              browser_context);
  return data_reduction_proxy_settings &&
         data_reduction_proxy_settings->IsDataSaverEnabledByUser();
 }
