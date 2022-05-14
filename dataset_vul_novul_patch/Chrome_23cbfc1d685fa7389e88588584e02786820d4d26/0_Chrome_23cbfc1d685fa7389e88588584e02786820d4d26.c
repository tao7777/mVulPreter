  CheckClientDownloadRequest(
      content::DownloadItem* item,
      const CheckDownloadCallback& callback,
      DownloadProtectionService* service,
      const scoped_refptr<SafeBrowsingDatabaseManager>& database_manager,
      BinaryFeatureExtractor* binary_feature_extractor)
      : item_(item),
        url_chain_(item->GetUrlChain()),
         referrer_url_(item->GetReferrerUrl()),
         tab_url_(item->GetTabUrl()),
         tab_referrer_url_(item->GetTabReferrerUrl()),
        archived_executable_(false),
         callback_(callback),
         service_(service),
         binary_feature_extractor_(binary_feature_extractor),
        database_manager_(database_manager),
        pingback_enabled_(service_->enabled()),
        finished_(false),
        type_(ClientDownloadRequest::WIN_EXECUTABLE),
        start_time_(base::TimeTicks::Now()),
        weakptr_factory_(this) {
    DCHECK_CURRENTLY_ON(BrowserThread::UI);
    item_->AddObserver(this);
  }
