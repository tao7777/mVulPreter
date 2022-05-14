void DevToolsDataSource::StartDataRequest(
    const std::string& path,
    const content::ResourceRequestInfo::WebContentsGetter& wc_getter,
    const content::URLDataSource::GotDataCallback& callback) {
  std::string bundled_path_prefix(chrome::kChromeUIDevToolsBundledPath);
  bundled_path_prefix += "/";
  if (base::StartsWith(path, bundled_path_prefix,
                       base::CompareCase::INSENSITIVE_ASCII)) {
    StartBundledDataRequest(path.substr(bundled_path_prefix.length()),
                            callback);
    return;
  }

  std::string empty_path_prefix(chrome::kChromeUIDevToolsBlankPath);
  if (base::StartsWith(path, empty_path_prefix,
                       base::CompareCase::INSENSITIVE_ASCII)) {
    callback.Run(new base::RefCountedStaticMemory());
    return;
  }

  std::string remote_path_prefix(chrome::kChromeUIDevToolsRemotePath);
  remote_path_prefix += "/";
  if (base::StartsWith(path, remote_path_prefix,
                       base::CompareCase::INSENSITIVE_ASCII)) {
    GURL url(kRemoteFrontendBase + path.substr(remote_path_prefix.length()));

    CHECK_EQ(url.host(), kRemoteFrontendDomain);
    if (url.is_valid() && DevToolsUIBindings::IsValidRemoteFrontendURL(url)) {
      StartRemoteDataRequest(url, callback);
    } else {
      DLOG(ERROR) << "Refusing to load invalid remote front-end URL";
      callback.Run(new base::RefCountedStaticMemory(kHttpNotFound,
                                                    strlen(kHttpNotFound)));
    }
    return;
  }

  std::string custom_frontend_url =
      base::CommandLine::ForCurrentProcess()->GetSwitchValueASCII(
          switches::kCustomDevtoolsFrontend);

  if (custom_frontend_url.empty()) {
    callback.Run(NULL);
    return;
  }

  std::string custom_path_prefix(chrome::kChromeUIDevToolsCustomPath);
  custom_path_prefix += "/";

  if (base::StartsWith(path, custom_path_prefix,
                        base::CompareCase::INSENSITIVE_ASCII)) {
     GURL url = GURL(custom_frontend_url +
                     path.substr(custom_path_prefix.length()));
     StartCustomDataRequest(url, callback);
     return;
   }

  callback.Run(NULL);
}
