void MaybeReportDownloadDeepScanningVerdict(
    Profile* profile,
    const GURL& url,
    const std::string& file_name,
     const std::string& download_digest_sha256,
     BinaryUploadService::Result result,
     DeepScanningClientResponse response) {
   if (response.malware_scan_verdict().verdict() ==
           MalwareDeepScanningVerdict::UWS ||
       response.malware_scan_verdict().verdict() ==
           MalwareDeepScanningVerdict::MALWARE) {
     extensions::SafeBrowsingPrivateEventRouterFactory::GetForProfile(profile)
         ->OnDangerousDeepScanningResult(url, file_name, download_digest_sha256);
   }
 }
