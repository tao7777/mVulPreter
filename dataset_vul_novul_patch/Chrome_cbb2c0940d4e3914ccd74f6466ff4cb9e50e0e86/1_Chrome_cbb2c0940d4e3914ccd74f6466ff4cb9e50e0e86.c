   explicit RemoveDownloadsTester(TestingProfile* testing_profile)
      : download_manager_(new content::MockDownloadManager()),
        chrome_download_manager_delegate_(testing_profile) {
     content::BrowserContext::SetDownloadManagerForTesting(
         testing_profile, base::WrapUnique(download_manager_));
    EXPECT_EQ(download_manager_,
              content::BrowserContext::GetDownloadManager(testing_profile));
    EXPECT_CALL(*download_manager_, GetDelegate())
        .WillOnce(Return(&chrome_download_manager_delegate_));
     EXPECT_CALL(*download_manager_, Shutdown());
   }
