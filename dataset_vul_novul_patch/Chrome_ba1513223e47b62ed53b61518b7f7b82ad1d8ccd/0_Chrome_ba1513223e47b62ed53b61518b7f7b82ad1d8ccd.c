 void ServerWrapper::OnHttpRequest(int connection_id,
                                   const net::HttpServerRequestInfo& info) {
  if (!RequestIsSafeToServe(info)) {
    Send500(connection_id,
            "Host header is specified and is not an IP address or localhost.");
    return;
  }

   server_->SetSendBufferSize(connection_id, kSendBufferSizeForDevTools);
 
   if (base::StartsWith(info.path, "/json", base::CompareCase::SENSITIVE)) {
    BrowserThread::PostTask(BrowserThread::UI, FROM_HERE,
                            base::BindOnce(&DevToolsHttpHandler::OnJsonRequest,
                                           handler_, connection_id, info));
    return;
  }

  if (info.path.empty() || info.path == "/") {
    BrowserThread::PostTask(
        BrowserThread::UI, FROM_HERE,
        base::BindOnce(&DevToolsHttpHandler::OnDiscoveryPageRequest, handler_,
                       connection_id));
    return;
  }

  if (!base::StartsWith(info.path, "/devtools/",
                        base::CompareCase::SENSITIVE)) {
    server_->Send404(connection_id, kDevtoolsHttpHandlerTrafficAnnotation);
    return;
  }

  std::string filename = PathWithoutParams(info.path.substr(10));
  std::string mime_type = GetMimeType(filename);

  if (!debug_frontend_dir_.empty()) {
    base::FilePath path = debug_frontend_dir_.AppendASCII(filename);
    std::string data;
    base::ReadFileToString(path, &data);
    server_->Send200(connection_id, data, mime_type,
                     kDevtoolsHttpHandlerTrafficAnnotation);
    return;
  }

  if (bundles_resources_) {
    BrowserThread::PostTask(
        BrowserThread::UI, FROM_HERE,
        base::BindOnce(&DevToolsHttpHandler::OnFrontendResourceRequest,
                       handler_, connection_id, filename));
    return;
  }
  server_->Send404(connection_id, kDevtoolsHttpHandlerTrafficAnnotation);
}
