void MaybeReportDownloadDeepScanningVerdict(
    Profile* profile,
    const GURL& url,
    const std::string& file_name,
     const std::string& download_digest_sha256,
     BinaryUploadService::Result result,
     DeepScanningClientResponse response) {
  if (result != BinaryUploadService::Result::SUCCESS)
    return;

  if (!g_browser_process->local_state()->GetBoolean(
          policy::policy_prefs::kUnsafeEventsReportingEnabled))
    return;

   if (response.malware_scan_verdict().verdict() ==
           MalwareDeepScanningVerdict::UWS ||
       response.malware_scan_verdict().verdict() ==
           MalwareDeepScanningVerdict::MALWARE) {
     extensions::SafeBrowsingPrivateEventRouterFactory::GetForProfile(profile)
         ->OnDangerousDeepScanningResult(url, file_name, download_digest_sha256);
   }

  if (response.dlp_scan_verdict().status() == DlpDeepScanningVerdict::SUCCESS) {
    if (!response.dlp_scan_verdict().triggered_rules().empty()) {
      extensions::SafeBrowsingPrivateEventRouterFactory::GetForProfile(profile)
          ->OnSensitiveDataEvent(response.dlp_scan_verdict(), url, file_name,
                                 download_digest_sha256);
    }
  }
 }
