void BackgroundLoaderOffliner::StartSnapshot() {
  if (!pending_request_.get()) {
    DVLOG(1) << "Pending request was cleared during delay.";
    return;
  }
  DCHECK(is_low_bar_met_)
      << "Minimum quality must have been reached before saving a snapshot";

  AddLoadingSignal("Snapshotting");

  SavePageRequest request(*pending_request_.get());
  if (page_load_state_ != SUCCESS) {
    Offliner::RequestStatus status;
    switch (page_load_state_) {
      case RETRIABLE_NET_ERROR:
        status = Offliner::RequestStatus::LOADING_FAILED_NET_ERROR;
        break;
      case RETRIABLE_HTTP_ERROR:
        status = Offliner::RequestStatus::LOADING_FAILED_HTTP_ERROR;
        break;
      case NONRETRIABLE:
        status = Offliner::RequestStatus::LOADING_FAILED_NO_RETRY;
        break;
      default:
        NOTREACHED();
        status = Offliner::RequestStatus::LOADING_FAILED;
    }

    std::move(completion_callback_).Run(request, status);
    ResetState();
    return;
  }

  content::WebContents* web_contents(
      content::WebContentsObserver::web_contents());

  Offliner::RequestStatus loaded_page_error =
      CanSavePageInBackground(web_contents);
  if (loaded_page_error != Offliner::RequestStatus::UNKNOWN) {
    std::move(completion_callback_).Run(request, loaded_page_error);
    ResetState();
    return;
  }

  save_state_ = SAVING;

   RequestStats& image_stats = stats_[ResourceDataType::IMAGE];
   RequestStats& css_stats = stats_[ResourceDataType::TEXT_CSS];
   RequestStats& xhr_stats = stats_[ResourceDataType::XHR];
  bool image_complete = (image_stats.requested == image_stats.completed);
  bool css_complete = (css_stats.requested == css_stats.completed);
  bool xhr_complete = (xhr_stats.requested == xhr_stats.completed);
  RecordResourceCompletionUMA(image_complete, css_complete, xhr_complete);
 
  if (IsOfflinePagesLoadSignalCollectingEnabled()) {
    signal_data_.SetDouble("StartedImages", image_stats.requested);
    signal_data_.SetDouble("CompletedImages", image_stats.completed);
    signal_data_.SetDouble("StartedCSS", css_stats.requested);
    signal_data_.SetDouble("CompletedCSS", css_stats.completed);
    signal_data_.SetDouble("StartedXHR", xhr_stats.requested);
    signal_data_.SetDouble("CompletedXHR", xhr_stats.completed);

    std::string headers = base::StringPrintf(
        "%s\r\n%s\r\n\r\n", kContentTransferEncodingBinary, kXHeaderForSignals);
    std::string body;
    base::JSONWriter::Write(signal_data_, &body);
    std::string content_type = kContentType;
    std::string content_location = base::StringPrintf(
        "cid:signal-data-%" PRId64 "@mhtml.blink", request.request_id());

    content::MHTMLExtraParts* extra_parts =
        content::MHTMLExtraParts::FromWebContents(web_contents);
    DCHECK(extra_parts);
    if (extra_parts != nullptr) {
      extra_parts->AddExtraMHTMLPart(content_type, content_location, headers,
                                     body);
    }
  }

  std::unique_ptr<OfflinePageArchiver> archiver(new OfflinePageMHTMLArchiver());

  OfflinePageModel::SavePageParams params;
  params.url = web_contents->GetLastCommittedURL();
  params.client_id = request.client_id();
  params.proposed_offline_id = request.request_id();
  params.is_background = true;
  params.use_page_problem_detectors = true;
  params.request_origin = request.request_origin();

  if (!request.original_url().is_empty())
    params.original_url = request.original_url();
  else if (params.url != request.url())
    params.original_url = request.url();

  offline_page_model_->SavePage(
      params, std::move(archiver), web_contents,
      base::Bind(&BackgroundLoaderOffliner::OnPageSaved,
                 weak_ptr_factory_.GetWeakPtr()));
}
