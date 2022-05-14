void ChromeDownloadManagerDelegate::CheckIfSuggestedPathExists(
    int32 download_id,
    const FilePath& unverified_path,
    bool should_prompt,
    bool is_forced_path,
    content::DownloadDangerType danger_type,
    const FilePath& default_path) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::FILE));

  FilePath target_path(unverified_path);

  file_util::CreateDirectory(default_path);

  FilePath dir = target_path.DirName();
  FilePath filename = target_path.BaseName();
  if (!file_util::PathIsWritable(dir)) {
    VLOG(1) << "Unable to write to directory \"" << dir.value() << "\"";
    should_prompt = true;
    PathService::Get(chrome::DIR_USER_DOCUMENTS, &dir);
    target_path = dir.Append(filename);
  }


  bool should_uniquify =
      (!is_forced_path &&
       (danger_type == content::DOWNLOAD_DANGER_TYPE_NOT_DANGEROUS ||
        should_prompt));
  bool should_overwrite =
      (should_uniquify || is_forced_path);
  bool should_create_marker = (should_uniquify && !should_prompt);

  if (should_uniquify) {
     int uniquifier =
        download_util::GetUniquePathNumberWithCrDownload(target_path);

    if (uniquifier > 0) {
      target_path = target_path.InsertBeforeExtensionASCII(
          StringPrintf(" (%d)", uniquifier));
    } else if (uniquifier == -1) {
      VLOG(1) << "Unable to find a unique path for suggested path \""
              << target_path.value() << "\"";
      should_prompt = true;
    }
  }

  if (should_create_marker)
    file_util::WriteFile(download_util::GetCrDownloadPath(target_path), "", 0);

  DownloadItem::TargetDisposition disposition;
  if (should_prompt)
    disposition = DownloadItem::TARGET_DISPOSITION_PROMPT;
  else if (should_overwrite)
    disposition = DownloadItem::TARGET_DISPOSITION_OVERWRITE;
  else
    disposition = DownloadItem::TARGET_DISPOSITION_UNIQUIFY;

  BrowserThread::PostTask(
      BrowserThread::UI, FROM_HERE,
      base::Bind(&ChromeDownloadManagerDelegate::OnPathExistenceAvailable,
                 this, download_id, target_path, disposition, danger_type));
}
