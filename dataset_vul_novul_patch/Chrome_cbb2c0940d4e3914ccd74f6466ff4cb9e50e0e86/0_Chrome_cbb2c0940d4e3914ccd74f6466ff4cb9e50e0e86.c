   explicit RemoveDownloadsTester(TestingProfile* testing_profile)
      : download_manager_(new content::MockDownloadManager()) {
     content::BrowserContext::SetDownloadManagerForTesting(
         testing_profile, base::WrapUnique(download_manager_));
    std::unique_ptr<ChromeDownloadManagerDelegate> delegate =
        std::make_unique<ChromeDownloadManagerDelegate>(testing_profile);
    chrome_download_manager_delegate_ = delegate.get();
    service_ =
        DownloadCoreServiceFactory::GetForBrowserContext(testing_profile);
    service_->SetDownloadManagerDelegateForTesting(std::move(delegate));

    EXPECT_CALL(*download_manager_, GetBrowserContext())
        .WillRepeatedly(Return(testing_profile));
     EXPECT_CALL(*download_manager_, Shutdown());
   }
