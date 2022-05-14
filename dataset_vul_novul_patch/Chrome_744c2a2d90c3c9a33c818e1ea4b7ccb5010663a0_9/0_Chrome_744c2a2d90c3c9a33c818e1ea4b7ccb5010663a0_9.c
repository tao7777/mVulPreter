   virtual void TearDown() {
     content::GetContentClient()->set_browser(old_browser_client_);
    content::SetContentClient(old_client_);
     RenderViewHostTestHarness::TearDown();
   }
