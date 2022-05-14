  virtual void SetUp() {
     url_util::AddStandardScheme("tabcontentstest");
 
     old_browser_client_ = content::GetContentClient()->browser();
     content::GetContentClient()->set_browser(&browser_client_);
     RenderViewHostTestHarness::SetUp();
   }
