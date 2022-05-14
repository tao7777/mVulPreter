void ReportRequestHeaders(std::map<std::string, std::string>* request_headers,
                          const std::string& url,
                          const std::string& headers) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  EXPECT_FALSE(base::ContainsKey(*request_headers, url));
  (*request_headers)[url] = headers;
}
