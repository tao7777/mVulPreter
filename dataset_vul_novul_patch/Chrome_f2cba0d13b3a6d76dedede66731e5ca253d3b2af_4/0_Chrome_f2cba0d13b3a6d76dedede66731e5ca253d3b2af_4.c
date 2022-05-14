  WebsiteSettings* website_settings() {
     if (!website_settings_.get()) {
       website_settings_.reset(new WebsiteSettings(
           mock_ui(), profile(), tab_specific_content_settings(),
          web_contents(), url(), ssl(), cert_store()));
     }
     return website_settings_.get();
   }
