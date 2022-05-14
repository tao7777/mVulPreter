void InstallablePaymentAppCrawler::OnPaymentMethodManifestParsed(
    const GURL& method_manifest_url,
    const std::vector<GURL>& default_applications,
    const std::vector<url::Origin>& supported_origins,
    bool all_origins_supported) {
  number_of_payment_method_manifest_to_parse_--;

  if (web_contents() == nullptr)
    return;
  content::PermissionManager* permission_manager =
      web_contents()->GetBrowserContext()->GetPermissionManager();
  if (permission_manager == nullptr)
    return;

  for (const auto& url : default_applications) {
    if (downloaded_web_app_manifests_.find(url) !=
        downloaded_web_app_manifests_.end()) {
       continue;
     }
 
    if (!net::registry_controlled_domains::SameDomainOrHost(
            method_manifest_url, url,
            net::registry_controlled_domains::INCLUDE_PRIVATE_REGISTRIES)) {
      WarnIfPossible("Installable payment app from " + url.spec() +
                     " is not allowed for the method " +
                     method_manifest_url.spec());
      continue;
    }

     if (permission_manager->GetPermissionStatus(
             content::PermissionType::PAYMENT_HANDLER, url.GetOrigin(),
             url.GetOrigin()) != blink::mojom::PermissionStatus::GRANTED) {
      continue;
    }

    number_of_web_app_manifest_to_download_++;
    downloaded_web_app_manifests_.insert(url);
    downloader_->DownloadWebAppManifest(
        url,
        base::BindOnce(
            &InstallablePaymentAppCrawler::OnPaymentWebAppManifestDownloaded,
            weak_ptr_factory_.GetWeakPtr(), method_manifest_url, url));
  }

  FinishCrawlingPaymentAppsIfReady();
}
