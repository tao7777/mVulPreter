void BlobURLRequestJob::HeadersCompleted(int status_code,
                                         const std::string& status_text) {
  std::string status("HTTP/1.1 ");
  status.append(base::IntToString(status_code));
  status.append(" ");
  status.append(status_text);
  status.append("\0\0", 2);
  net::HttpResponseHeaders* headers = new net::HttpResponseHeaders(status);

  if (status_code == kHTTPOk || status_code == kHTTPPartialContent) {
    std::string content_length_header(net::HttpRequestHeaders::kContentLength);
    content_length_header.append(": ");
    content_length_header.append(base::Int64ToString(remaining_bytes_));
    headers->AddHeader(content_length_header);
    if (!blob_data_->content_type().empty()) {
      std::string content_type_header(net::HttpRequestHeaders::kContentType);
      content_type_header.append(": ");
      content_type_header.append(blob_data_->content_type());
      headers->AddHeader(content_type_header);
    }
    if (!blob_data_->content_disposition().empty()) {
      std::string content_disposition_header("Content-Disposition: ");
      content_disposition_header.append(blob_data_->content_disposition());
      headers->AddHeader(content_disposition_header);
    }
  }

  response_info_.reset(new net::HttpResponseInfo());
   response_info_->headers = headers;
 
   set_expected_content_size(remaining_bytes_);
  NotifyHeadersComplete();
   headers_set_ = true;
 }
