   virtual void SetUp() {
    old_client_ = content::GetContentClient();
    content::SetContentClient(&client_);
     old_browser_client_ = content::GetContentClient()->browser();
     content::GetContentClient()->set_browser(&browser_client_);
     url_util::AddStandardScheme(kPrivilegedScheme);
    url_util::AddStandardScheme(chrome::kChromeUIScheme);
  }
