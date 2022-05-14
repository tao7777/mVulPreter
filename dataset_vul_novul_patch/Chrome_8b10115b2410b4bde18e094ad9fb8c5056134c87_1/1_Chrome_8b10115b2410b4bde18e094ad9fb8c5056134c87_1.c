  MirrorMockURLRequestJob(net::URLRequest* request,
                          net::NetworkDelegate* network_delegate,
                          const base::FilePath& file_path,
                          ReportResponseHeadersOnUI report_on_ui)
      : net::URLRequestMockHTTPJob(request, network_delegate, file_path),
        report_on_ui_(report_on_ui) {}
