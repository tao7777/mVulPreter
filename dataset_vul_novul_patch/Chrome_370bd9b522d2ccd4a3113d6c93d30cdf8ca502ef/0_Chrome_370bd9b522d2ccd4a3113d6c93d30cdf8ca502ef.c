void WebURLLoaderImpl::Context::OnReceivedResponse(
    const ResourceResponseInfo& info) {
  if (!client_)
    return;

  WebURLResponse response;
  response.initialize();
  PopulateURLResponse(request_.url(), info, &response);

  bool show_raw_listing = (GURL(request_.url()).query() == "raw");

  if (info.mime_type == "text/vnd.chromium.ftp-dir") {
    if (show_raw_listing) {
      response.setMIMEType("text/plain");
    } else {
      response.setMIMEType("text/html");
     }
   }
 
  scoped_refptr<Context> protect(this);
   client_->didReceiveResponse(loader_, response);
 
  if (!client_)
    return;

  DCHECK(!ftp_listing_delegate_.get());
  DCHECK(!multipart_delegate_.get());
  if (info.headers && info.mime_type == "multipart/x-mixed-replace") {
    std::string content_type;
    info.headers->EnumerateHeader(NULL, "content-type", &content_type);

    std::string mime_type;
    std::string charset;
    bool had_charset = false;
    std::string boundary;
    net::HttpUtil::ParseContentType(content_type, &mime_type, &charset,
                                    &had_charset, &boundary);
    TrimString(boundary, " \"", &boundary);

    if (!boundary.empty()) {
      multipart_delegate_.reset(
          new MultipartResponseDelegate(client_, loader_, response, boundary));
    }
  } else if (info.mime_type == "text/vnd.chromium.ftp-dir" &&
             !show_raw_listing) {
    ftp_listing_delegate_.reset(
        new FtpDirectoryListingResponseDelegate(client_, loader_, response));
  }
}
