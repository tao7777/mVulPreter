bool CheckClientDownloadRequest::ShouldUploadForMalwareScan(
    DownloadCheckResultReason reason) {
  if (!base::FeatureList::IsEnabled(kDeepScanningOfDownloads))
    return false;

  if (reason != DownloadCheckResultReason::REASON_DOWNLOAD_SAFE &&
      reason != DownloadCheckResultReason::REASON_DOWNLOAD_UNCOMMON &&
      reason != DownloadCheckResultReason::REASON_VERDICT_UNKNOWN)
    return false;

  content::BrowserContext* browser_context =
      content::DownloadItemUtils::GetBrowserContext(item_);
  if (!browser_context)
    return false;

  Profile* profile = Profile::FromBrowserContext(browser_context);
  if (!profile)
    return false;

  int send_files_for_malware_check = profile->GetPrefs()->GetInteger(
      prefs::kSafeBrowsingSendFilesForMalwareCheck);
  if (send_files_for_malware_check !=
          SendFilesForMalwareCheckValues::SEND_DOWNLOADS &&
      send_files_for_malware_check !=
           SendFilesForMalwareCheckValues::SEND_UPLOADS_AND_DOWNLOADS)
     return false;
 
  // If there's no valid DM token, the upload will fail, so we can skip
  // uploading now.
  return BrowserDMTokenStorage::Get()->RetrieveBrowserDMToken().is_valid();
 }
