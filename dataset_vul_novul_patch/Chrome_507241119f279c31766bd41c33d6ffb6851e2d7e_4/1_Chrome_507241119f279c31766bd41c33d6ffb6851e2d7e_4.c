void CheckClientDownloadRequest::UploadBinary(
    DownloadCheckResult result,
    DownloadCheckResultReason reason) {
  saved_result_ = result;
  saved_reason_ = reason;

  bool upload_for_dlp = ShouldUploadForDlpScan();
  bool upload_for_malware = ShouldUploadForMalwareScan(reason);
  auto request = std::make_unique<DownloadItemRequest>(
      item_, /*read_immediately=*/true,
      base::BindOnce(&CheckClientDownloadRequest::OnDeepScanningComplete,
                     weakptr_factory_.GetWeakPtr()));

  Profile* profile = Profile::FromBrowserContext(GetBrowserContext());

  if (upload_for_dlp) {
    DlpDeepScanningClientRequest dlp_request;
    dlp_request.set_content_source(DlpDeepScanningClientRequest::FILE_DOWNLOAD);
    request->set_request_dlp_scan(std::move(dlp_request));
  }

  if (upload_for_malware) {
    MalwareDeepScanningClientRequest malware_request;
    malware_request.set_population(
        MalwareDeepScanningClientRequest::POPULATION_ENTERPRISE);
    malware_request.set_download_token(
        DownloadProtectionService::GetDownloadPingToken(item_));
     request->set_request_malware_scan(std::move(malware_request));
   }
 
  request->set_dm_token(
      policy::BrowserDMTokenStorage::Get()->RetrieveDMToken());
 
   service()->UploadForDeepScanning(profile, std::move(request));
 }
