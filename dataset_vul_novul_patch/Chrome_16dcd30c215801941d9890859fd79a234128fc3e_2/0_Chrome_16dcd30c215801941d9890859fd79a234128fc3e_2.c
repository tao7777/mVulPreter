void DownloadFileManager::RenameCompletingDownloadFile(
    DownloadManager* download_manager = download_file->GetDownloadManager();
    DCHECK(download_manager);

    BrowserThread::PostTask(
        BrowserThread::UI, FROM_HERE,
        base::Bind(&DownloadManager::OnDownloadInterrupted,
                   download_manager,
                   global_id.local(),
                   download_file->BytesSoFar(),
                   download_file->GetHashState(),
                   content::ConvertNetErrorToInterruptReason(
                       rename_error,
                       content::DOWNLOAD_INTERRUPT_FROM_DISK)));
 
     new_path.clear();
   }
   BrowserThread::PostTask(BrowserThread::UI, FROM_HERE,
                           base::Bind(callback, new_path));
 }
