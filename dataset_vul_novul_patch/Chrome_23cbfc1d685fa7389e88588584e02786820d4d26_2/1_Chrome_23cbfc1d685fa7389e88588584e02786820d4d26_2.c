  void Start() {
    DVLOG(2) << "Starting SafeBrowsing download check for: "
             << item_->DebugString(true);
    DCHECK_CURRENTLY_ON(BrowserThread::UI);
    DownloadCheckResultReason reason = REASON_MAX;
    if (!IsSupportedDownload(
        *item_, item_->GetTargetFilePath(), &reason, &type_)) {
      switch (reason) {
        case REASON_EMPTY_URL_CHAIN:
        case REASON_INVALID_URL:
        case REASON_UNSUPPORTED_URL_SCHEME:
          PostFinishTask(UNKNOWN, reason);
          return;

        case REASON_NOT_BINARY_FILE:
          RecordFileExtensionType(item_->GetTargetFilePath());
          PostFinishTask(UNKNOWN, reason);
          return;

        default:
          NOTREACHED();
      }
    }
    RecordFileExtensionType(item_->GetTargetFilePath());

     if (item_->GetTargetFilePath().MatchesExtension(
         FILE_PATH_LITERAL(".zip"))) {
       StartExtractZipFeatures();
     } else {
       DCHECK(!download_protection_util::IsArchiveFile(
           item_->GetTargetFilePath()));
      StartExtractFileFeatures();
    }
  }
