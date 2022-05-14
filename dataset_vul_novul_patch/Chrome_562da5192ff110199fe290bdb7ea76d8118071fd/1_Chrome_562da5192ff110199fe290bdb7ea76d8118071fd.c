bool InstallablePaymentAppCrawler::CompleteAndStorePaymentWebAppInfoIfValid(
    const GURL& method_manifest_url,
    const GURL& web_app_manifest_url,
    std::unique_ptr<WebAppInstallationInfo> app_info) {
  if (app_info == nullptr)
    return false;

  if (app_info->sw_js_url.empty() || !base::IsStringUTF8(app_info->sw_js_url)) {
    WarnIfPossible(
        "The installable payment app's js url is not a non-empty UTF8 string.");
    return false;
  }

  if (!GURL(app_info->sw_js_url).is_valid()) {
    GURL absolute_url = web_app_manifest_url.Resolve(app_info->sw_js_url);
    if (!absolute_url.is_valid()) {
      WarnIfPossible(
          "Failed to resolve the installable payment app's js url (" +
           app_info->sw_js_url + ").");
       return false;
     }
     app_info->sw_js_url = absolute_url.spec();
   }
 
  if (!GURL(app_info->sw_scope).is_valid()) {
    GURL absolute_scope =
        web_app_manifest_url.GetWithoutFilename().Resolve(app_info->sw_scope);
    if (!absolute_scope.is_valid()) {
      WarnIfPossible(
          "Failed to resolve the installable payment app's registration "
          "scope (" +
           app_info->sw_scope + ").");
       return false;
     }
     app_info->sw_scope = absolute_scope.spec();
   }
 
  std::string error_message;
  if (!content::PaymentAppProvider::GetInstance()->IsValidInstallablePaymentApp(
          web_app_manifest_url, GURL(app_info->sw_js_url),
          GURL(app_info->sw_scope), &error_message)) {
    WarnIfPossible(error_message);
    return false;
  }

  if (installable_apps_.find(method_manifest_url) != installable_apps_.end())
    return false;

  installable_apps_[method_manifest_url] = std::move(app_info);

  return true;
}
