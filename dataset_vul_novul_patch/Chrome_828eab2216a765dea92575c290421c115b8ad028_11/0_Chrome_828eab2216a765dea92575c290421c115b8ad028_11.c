void ChromeNetworkDelegate::OnCompleted(net::URLRequest* request,
                                        bool started) {
  TRACE_EVENT_ASYNC_END0("net", "URLRequest", request);
  if (request->status().status() == net::URLRequestStatus::SUCCESS) {
    int64 received_content_length = request->received_response_content_length();

    bool is_http = request->url().SchemeIs("http");
    bool is_https = request->url().SchemeIs("https");

    if (!request->was_cached() &&         // Don't record cached content
        received_content_length &&        // Zero-byte responses aren't useful.
        (is_http || is_https)) {          // Only record for HTTP or HTTPS urls.
       int64 original_content_length =
           request->response_info().headers->GetInt64HeaderValue(
               "x-original-content-length");
      chrome_browser_net::DataReductionRequestType data_reduction_type =
          chrome_browser_net::GetDataReductionRequestType(
              reinterpret_cast<Profile*>(profile_), request);

       base::TimeDelta freshness_lifetime =
           request->response_info().headers->GetFreshnessLifetime(
               request->response_info().response_time);
      int64 adjusted_original_content_length =
          chrome_browser_net::GetAdjustedOriginalContentLength(
              data_reduction_type, original_content_length,
              received_content_length);
       AccumulateContentLength(received_content_length,
                               adjusted_original_content_length,
                              data_reduction_type);
       RecordContentLengthHistograms(received_content_length,
                                     original_content_length,
                                     freshness_lifetime);
      DVLOG(2) << __FUNCTION__
          << " received content length: " << received_content_length
          << " original content length: " << original_content_length
          << " url: " << request->url();
    }

    bool is_redirect = request->response_headers() &&
        net::HttpResponseHeaders::IsRedirectResponseCode(
            request->response_headers()->response_code());
    if (!is_redirect) {
      ExtensionWebRequestEventRouter::GetInstance()->OnCompleted(
          profile_, extension_info_map_.get(), request);
    }
  } else if (request->status().status() == net::URLRequestStatus::FAILED ||
             request->status().status() == net::URLRequestStatus::CANCELED) {
    ExtensionWebRequestEventRouter::GetInstance()->OnErrorOccurred(
            profile_, extension_info_map_.get(), request, started);
  } else {
    NOTREACHED();
  }
  ForwardProxyErrors(request, event_router_.get(), profile_);

  ForwardRequestStatus(REQUEST_DONE, request, profile_);
}
