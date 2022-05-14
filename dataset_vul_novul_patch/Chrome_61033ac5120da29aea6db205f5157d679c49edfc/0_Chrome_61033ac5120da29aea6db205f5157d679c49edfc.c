void SafeBrowsingPrivateEventRouter::OnDangerousDeepScanningResult(
    const GURL& url,
     const std::string& file_name,
     const std::string& download_digest_sha256) {
   if (client_) {
    // Create a real-time event dictionary from the arguments and report it.
     base::Value event(base::Value::Type::DICTIONARY);
     event.SetStringKey(kKeyUrl, url.spec());
     event.SetStringKey(kKeyFileName, file_name);
    event.SetStringKey(kKeyDownloadDigestSha256, download_digest_sha256);
    event.SetStringKey(kKeyProfileUserName, GetProfileUserName());
    ReportRealtimeEvent(kKeyDangerousDownloadEvent, std::move(event));
   }
 }
