 bool IsSiteMuted(const TabStripModel& tab_strip, const int index) {
   content::WebContents* web_contents = tab_strip.GetWebContentsAt(index);
   GURL url = web_contents->GetLastCommittedURL();
 
  if (url.SchemeIs(content::kChromeUIScheme)) {
    return web_contents->IsAudioMuted() &&
           GetTabAudioMutedReason(web_contents) ==
               TabMutedReason::CONTENT_SETTING_CHROME;
  }

  Profile* profile =
      Profile::FromBrowserContext(web_contents->GetBrowserContext());
  HostContentSettingsMap* settings =
      HostContentSettingsMapFactory::GetForProfile(profile);
  return settings->GetContentSetting(url, url, CONTENT_SETTINGS_TYPE_SOUND,
                                     std::string()) == CONTENT_SETTING_BLOCK;
}
