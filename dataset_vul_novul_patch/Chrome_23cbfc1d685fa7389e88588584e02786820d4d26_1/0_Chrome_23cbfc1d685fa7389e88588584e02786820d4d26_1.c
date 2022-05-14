  void SendRequest() {
    DCHECK_CURRENTLY_ON(BrowserThread::UI);

    if (!service_)
      return;
    bool is_extended_reporting = false;
    if (item_->GetBrowserContext()) {
      Profile* profile =
          Profile::FromBrowserContext(item_->GetBrowserContext());
      is_extended_reporting = profile &&
                              profile->GetPrefs()->GetBoolean(
                                  prefs::kSafeBrowsingExtendedReportingEnabled);
    }

    ClientDownloadRequest request;
    if (is_extended_reporting) {
      request.mutable_population()->set_user_population(
          ChromeUserPopulation::EXTENDED_REPORTING);
    } else {
      request.mutable_population()->set_user_population(
          ChromeUserPopulation::SAFE_BROWSING);
    }
    request.set_url(SanitizeUrl(item_->GetUrlChain().back()));
    request.mutable_digests()->set_sha256(item_->GetHash());
    request.set_length(item_->GetReceivedBytes());
    for (size_t i = 0; i < item_->GetUrlChain().size(); ++i) {
      ClientDownloadRequest::Resource* resource = request.add_resources();
      resource->set_url(SanitizeUrl(item_->GetUrlChain()[i]));
      if (i == item_->GetUrlChain().size() - 1) {
        resource->set_type(ClientDownloadRequest::DOWNLOAD_URL);
        resource->set_referrer(SanitizeUrl(item_->GetReferrerUrl()));
        DVLOG(2) << "dl url " << resource->url();
        if (!item_->GetRemoteAddress().empty()) {
          resource->set_remote_ip(item_->GetRemoteAddress());
          DVLOG(2) << "  dl url remote addr: " << resource->remote_ip();
        }
        DVLOG(2) << "dl referrer " << resource->referrer();
      } else {
        DVLOG(2) << "dl redirect " << i << " " << resource->url();
        resource->set_type(ClientDownloadRequest::DOWNLOAD_REDIRECT);
      }
    }
    for (size_t i = 0; i < tab_redirects_.size(); ++i) {
      ClientDownloadRequest::Resource* resource = request.add_resources();
      DVLOG(2) << "tab redirect " << i << " " << tab_redirects_[i].spec();
      resource->set_url(SanitizeUrl(tab_redirects_[i]));
      resource->set_type(ClientDownloadRequest::TAB_REDIRECT);
    }
    if (tab_url_.is_valid()) {
      ClientDownloadRequest::Resource* resource = request.add_resources();
      resource->set_url(SanitizeUrl(tab_url_));
      DVLOG(2) << "tab url " << resource->url();
      resource->set_type(ClientDownloadRequest::TAB_URL);
      if (tab_referrer_url_.is_valid()) {
        resource->set_referrer(SanitizeUrl(tab_referrer_url_));
        DVLOG(2) << "tab referrer " << resource->referrer();
      }
    }

    request.set_user_initiated(item_->HasUserGesture());
    request.set_file_basename(
        item_->GetTargetFilePath().BaseName().AsUTF8Unsafe());
    request.set_download_type(type_);
     request.mutable_signature()->CopyFrom(signature_info_);
     if (image_headers_)
       request.set_allocated_image_headers(image_headers_.release());
    if (archived_executable_)
       request.mutable_archived_binary()->Swap(&archived_binary_);
     if (!request.SerializeToString(&client_download_request_data_)) {
       FinishRequest(UNKNOWN, REASON_INVALID_REQUEST_PROTO);
      return;
    }
    service_->client_download_request_callbacks_.Notify(item_, &request);
 
     DVLOG(2) << "Sending a request for URL: "
              << item_->GetUrlChain().back();
    DVLOG(2) << "Detected " << request.archived_binary().size() << " archived "
             << "binaries";
     fetcher_ = net::URLFetcher::Create(0 /* ID used for testing */,
                                        GetDownloadRequestUrl(),
                                        net::URLFetcher::POST, this);
    fetcher_->SetLoadFlags(net::LOAD_DISABLE_CACHE);
    fetcher_->SetAutomaticallyRetryOn5xx(false);  // Don't retry on error.
    fetcher_->SetRequestContext(service_->request_context_getter_.get());
    fetcher_->SetUploadData("application/octet-stream",
                            client_download_request_data_);
    request_start_time_ = base::TimeTicks::Now();
    UMA_HISTOGRAM_COUNTS("SBClientDownload.DownloadRequestPayloadSize",
                         client_download_request_data_.size());
    fetcher_->Start();
  }
