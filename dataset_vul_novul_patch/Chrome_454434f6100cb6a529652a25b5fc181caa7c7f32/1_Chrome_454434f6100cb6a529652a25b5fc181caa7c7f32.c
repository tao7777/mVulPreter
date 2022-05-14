 bool ExtensionService::IsDownloadFromGallery(const GURL& download_url,
                                              const GURL& referrer_url) {
  if (IsDownloadFromMiniGallery(download_url) &&
      StartsWithASCII(referrer_url.spec(),
                      extension_urls::kMiniGalleryBrowsePrefix, false)) {
    return true;
  }

  const Extension* download_extension = GetExtensionByWebExtent(download_url);
  const Extension* referrer_extension = GetExtensionByWebExtent(referrer_url);
  const Extension* webstore_app = GetWebStoreApp();

   bool referrer_valid = (referrer_extension == webstore_app);
   bool download_valid = (download_extension == webstore_app);
 
   GURL store_url =
       GURL(CommandLine::ForCurrentProcess()->GetSwitchValueASCII(
           switches::kAppsGalleryURL));
  if (!store_url.is_empty()) {
    std::string store_tld =
        net::RegistryControlledDomainService::GetDomainAndRegistry(store_url);
    if (!referrer_valid) {
      std::string referrer_tld =
          net::RegistryControlledDomainService::GetDomainAndRegistry(
              referrer_url);
      referrer_valid = referrer_url.is_empty() || (referrer_tld == store_tld);
    }

    if (!download_valid) {
      std::string download_tld =
          net::RegistryControlledDomainService::GetDomainAndRegistry(
              download_url);

      download_valid = (download_tld == store_tld);
    }
  }

  return (referrer_valid && download_valid);
}
