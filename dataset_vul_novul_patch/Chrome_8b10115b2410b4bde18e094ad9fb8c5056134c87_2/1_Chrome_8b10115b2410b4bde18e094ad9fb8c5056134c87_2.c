  static void Register(const GURL& url,
                       const base::FilePath& root_http,
                       ReportResponseHeadersOnUI report_on_ui) {
    EXPECT_TRUE(
        content::BrowserThread::CurrentlyOn(content::BrowserThread::IO));
    base::FilePath file_path(root_http);
    file_path =
        file_path.AppendASCII(url.scheme() + "." + url.host() + ".html");
    net::URLRequestFilter::GetInstance()->AddUrlInterceptor(
        url, base::WrapUnique(
                 new MirrorMockJobInterceptor(file_path, report_on_ui)));
  }
