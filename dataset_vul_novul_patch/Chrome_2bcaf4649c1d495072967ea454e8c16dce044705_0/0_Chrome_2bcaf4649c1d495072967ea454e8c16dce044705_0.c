 void HttpResponseHeaders::ReplaceStatusLine(const std::string& new_status) {
  CheckDoesNotHaveEmbededNulls(new_status);
   std::string new_raw_headers(new_status);
   new_raw_headers.push_back('\0');

  HeaderSet empty_to_remove;
  MergeWithHeaders(new_raw_headers, empty_to_remove);
}
