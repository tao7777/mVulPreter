  void RenameFile(const DownloadId& id,
                  const FilePath& new_path,
                  const FilePath& unique_path,
                  net::Error rename_error,
                  RenameFileState state,
                  RenameFileOverwrite should_overwrite) {
    MockDownloadFile* file = download_file_factory_->GetExistingFile(id);
    ASSERT_TRUE(file != NULL);

    EXPECT_CALL(*file, Rename(unique_path))
        .Times(1)
        .WillOnce(Return(rename_error));

    if (rename_error != net::OK) {
      EXPECT_CALL(*file, BytesSoFar())
          .Times(AtLeast(1))
          .WillRepeatedly(Return(byte_count_[id]));
      EXPECT_CALL(*file, GetHashState())
           .Times(AtLeast(1));
       EXPECT_CALL(*file, GetDownloadManager())
           .Times(AtLeast(1));
     }
 
    download_file_manager_->RenameDownloadFile(
        id, new_path, (should_overwrite == OVERWRITE),
        base::Bind(&TestDownloadManager::OnDownloadRenamed,
                   download_manager_, id.local()));
 
     if (rename_error != net::OK) {
       EXPECT_CALL(*download_manager_,
                  OnDownloadInterrupted(
                      id.local(),
                      byte_count_[id],
                      "",
                      content::ConvertNetErrorToInterruptReason(
                          rename_error,
                          content::DOWNLOAD_INTERRUPT_FROM_DISK)));
      EXPECT_CALL(*download_manager_,
                  OnDownloadRenamed(id.local(), FilePath()));
      ProcessAllPendingMessages();
      ++error_count_[id];
    } else {
      EXPECT_CALL(*download_manager_,
                  OnDownloadRenamed(id.local(), unique_path));
      ProcessAllPendingMessages();
     }
   }
