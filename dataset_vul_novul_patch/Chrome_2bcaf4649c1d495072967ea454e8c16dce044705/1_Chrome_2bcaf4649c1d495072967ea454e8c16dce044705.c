 void HttpResponseHeaders::AddHeader(const std::string& header) {
   DCHECK_EQ('\0', raw_headers_[raw_headers_.size() - 2]);
   DCHECK_EQ('\0', raw_headers_[raw_headers_.size() - 1]);
  std::string new_raw_headers(raw_headers_, 0, raw_headers_.size() - 1);
  new_raw_headers.append(header);
  new_raw_headers.push_back('\0');
  new_raw_headers.push_back('\0');

  raw_headers_.clear();
  parsed_.clear();
  Parse(new_raw_headers);
 }
