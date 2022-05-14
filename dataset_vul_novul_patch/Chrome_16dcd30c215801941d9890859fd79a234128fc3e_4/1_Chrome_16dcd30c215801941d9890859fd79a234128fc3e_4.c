   void CleanUp(DownloadId id) {
    MockDownloadFile* file = download_file_factory_->GetExistingFile(id);
    ASSERT_TRUE(file != NULL);

    EXPECT_CALL(*file, Cancel());
 
     download_file_manager_->CancelDownload(id);
 
    EXPECT_TRUE(NULL == download_file_manager_->GetDownloadFile(id));
   }
