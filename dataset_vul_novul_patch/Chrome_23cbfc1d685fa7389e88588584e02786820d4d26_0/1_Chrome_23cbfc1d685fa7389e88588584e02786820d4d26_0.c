  void OnZipAnalysisFinished(const zip_analyzer::Results& results) {
    DCHECK_CURRENTLY_ON(BrowserThread::UI);
    DCHECK_EQ(ClientDownloadRequest::ZIPPED_EXECUTABLE, type_);
     if (!service_)
       return;
     if (results.success) {
      zipped_executable_ = results.has_executable;
       archived_binary_.CopyFrom(results.archived_binary);
       DVLOG(1) << "Zip analysis finished for " << item_->GetFullPath().value()
                << ", has_executable=" << results.has_executable
               << " has_archive=" << results.has_archive;
    } else {
       DVLOG(1) << "Zip analysis failed for " << item_->GetFullPath().value();
     }
     UMA_HISTOGRAM_BOOLEAN("SBClientDownload.ZipFileHasExecutable",
                          zipped_executable_);
     UMA_HISTOGRAM_BOOLEAN("SBClientDownload.ZipFileHasArchiveButNoExecutable",
                          results.has_archive && !zipped_executable_);
     UMA_HISTOGRAM_TIMES("SBClientDownload.ExtractZipFeaturesTime",
                         base::TimeTicks::Now() - zip_analysis_start_time_);
     for (const auto& file_extension : results.archived_archive_filetypes)
       RecordArchivedArchiveFileExtensionType(file_extension);
 
    if (!zipped_executable_ && !results.has_archive) {
       PostFinishTask(UNKNOWN, REASON_ARCHIVE_WITHOUT_BINARIES);
       return;
     }
 
    if (!zipped_executable_ && results.has_archive)
       type_ = ClientDownloadRequest::ZIPPED_ARCHIVE;
     OnFileFeatureExtractionDone();
   }
