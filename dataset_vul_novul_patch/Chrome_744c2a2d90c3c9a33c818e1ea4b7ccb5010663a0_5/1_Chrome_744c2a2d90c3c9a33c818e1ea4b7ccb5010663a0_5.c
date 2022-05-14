   virtual void TearDown() {
     content::GetContentClient()->set_browser(old_browser_client_);
   }
