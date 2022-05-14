WebsiteSettings::WebsiteSettings(
     WebsiteSettingsUI* ui,
     Profile* profile,
     TabSpecificContentSettings* tab_specific_content_settings,
    content::WebContents* web_contents,
     const GURL& url,
     const content::SSLStatus& ssl,
     content::CertStore* cert_store)
     : TabSpecificContentSettings::SiteDataObserver(
           tab_specific_content_settings),
       ui_(ui),
      web_contents_(web_contents),
       show_info_bar_(false),
       site_url_(url),
       site_identity_status_(SITE_IDENTITY_STATUS_UNKNOWN),
      cert_id_(0),
      site_connection_status_(SITE_CONNECTION_STATUS_UNKNOWN),
      cert_store_(cert_store),
      content_settings_(profile->GetHostContentSettingsMap()),
      chrome_ssl_host_state_delegate_(
          ChromeSSLHostStateDelegateFactory::GetForProfile(profile)),
      did_revoke_user_ssl_decisions_(false) {
  Init(profile, url, ssl);

  PresentSitePermissions();
  PresentSiteData();
  PresentSiteIdentity();

  RecordWebsiteSettingsAction(WEBSITE_SETTINGS_OPENED);
}
