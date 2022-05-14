void ResourceMultiBufferDataProvider::DidReceiveResponse(
    const WebURLResponse& response) {
#if DCHECK_IS_ON()
  std::string version;
  switch (response.HttpVersion()) {
    case WebURLResponse::kHTTPVersion_0_9:
      version = "0.9";
      break;
    case WebURLResponse::kHTTPVersion_1_0:
      version = "1.0";
      break;
    case WebURLResponse::kHTTPVersion_1_1:
      version = "1.1";
      break;
    case WebURLResponse::kHTTPVersion_2_0:
      version = "2.1";
      break;
    case WebURLResponse::kHTTPVersionUnknown:
      version = "unknown";
      break;
  }
  DVLOG(1) << "didReceiveResponse: HTTP/" << version << " "
           << response.HttpStatusCode();
#endif
  DCHECK(active_loader_);

  scoped_refptr<UrlData> destination_url_data(url_data_);

  if (!redirects_to_.is_empty()) {
    destination_url_data =
        url_data_->url_index()->GetByUrl(redirects_to_, cors_mode_);
    redirects_to_ = GURL();
  }

  base::Time last_modified;
  if (base::Time::FromString(
          response.HttpHeaderField("Last-Modified").Utf8().data(),
          &last_modified)) {
    destination_url_data->set_last_modified(last_modified);
  }

  destination_url_data->set_etag(
      response.HttpHeaderField("ETag").Utf8().data());

  destination_url_data->set_valid_until(base::Time::Now() +
                                        GetCacheValidUntil(response));

  uint32_t reasons = GetReasonsForUncacheability(response);
  destination_url_data->set_cacheable(reasons == 0);
  UMA_HISTOGRAM_BOOLEAN("Media.CacheUseful", reasons == 0);
  int shift = 0;
  int max_enum = base::bits::Log2Ceiling(kMaxReason);
  while (reasons) {
    DCHECK_LT(shift, max_enum);  // Sanity check.
    if (reasons & 0x1) {
      UMA_HISTOGRAM_EXACT_LINEAR("Media.UncacheableReason", shift,
                                 max_enum);  // PRESUBMIT_IGNORE_UMA_MAX
    }

    reasons >>= 1;
    ++shift;
  }

   int64_t content_length = response.ExpectedContentLength();
   bool end_of_file = false;
   bool do_fail = false;
   bytes_to_discard_ = 0;
 
  if (destination_url_data->url().SchemeIsHTTPOrHTTPS()) {
    bool partial_response = (response.HttpStatusCode() == kHttpPartialContent);
    bool ok_response = (response.HttpStatusCode() == kHttpOK);

    std::string accept_ranges =
        response.HttpHeaderField("Accept-Ranges").Utf8();
    if (accept_ranges.find("bytes") != std::string::npos)
      destination_url_data->set_range_supported();

    if (partial_response &&
        VerifyPartialResponse(response, destination_url_data)) {
      destination_url_data->set_range_supported();
    } else if (ok_response) {
      destination_url_data->set_length(content_length);
      bytes_to_discard_ = byte_pos();
    } else if (response.HttpStatusCode() == kHttpRangeNotSatisfiable) {
      end_of_file = true;
    } else {
      active_loader_.reset();
      do_fail = true;
    }
  } else {
    destination_url_data->set_range_supported();
    if (content_length != kPositionNotSpecified) {
      destination_url_data->set_length(content_length + byte_pos());
    }
  }

  if (!do_fail) {
    destination_url_data =
         url_data_->url_index()->TryInsert(destination_url_data);
   }
 
  destination_url_data->set_has_opaque_data(
      network::cors::IsCORSCrossOriginResponseType(response.GetType()));
 
   if (destination_url_data != url_data_) {

    scoped_refptr<UrlData> old_url_data(url_data_);
    destination_url_data->Use();

    std::unique_ptr<DataProvider> self(
        url_data_->multibuffer()->RemoveProvider(this));
    url_data_ = destination_url_data.get();
    url_data_->multibuffer()->AddProvider(std::move(self));

    old_url_data->RedirectTo(destination_url_data);
  }

  if (do_fail) {
    destination_url_data->Fail();
    return;  // "this" may be deleted now.
  }

  const GURL& original_url = response.WasFetchedViaServiceWorker()
                                 ? response.OriginalURLViaServiceWorker()
                                 : response.Url();
  if (!url_data_->ValidateDataOrigin(original_url.GetOrigin())) {
    active_loader_.reset();
    url_data_->Fail();
    return;  // "this" may be deleted now.
  }

  if (end_of_file) {
    fifo_.push_back(DataBuffer::CreateEOSBuffer());
    url_data_->multibuffer()->OnDataProviderEvent(this);
  }
}
