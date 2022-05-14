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
      bool via_data_reduction_proxy =
          request->response_info().headers->HasHeaderValue(
              "via", "1.1 Chrome Compression Proxy");
      int64 adjusted_original_content_length = original_content_length;
      if (adjusted_original_content_length == -1)
        adjusted_original_content_length = received_content_length;
       base::TimeDelta freshness_lifetime =
           request->response_info().headers->GetFreshnessLifetime(
               request->response_info().response_time);
       AccumulateContentLength(received_content_length,
                               adjusted_original_content_length,
                              via_data_reduction_proxy);
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
