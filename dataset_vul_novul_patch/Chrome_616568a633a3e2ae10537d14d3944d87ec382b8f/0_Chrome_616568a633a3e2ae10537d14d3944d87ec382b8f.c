 bool IsSiteMuted(const TabStripModel& tab_strip, const int index) {
   content::WebContents* web_contents = tab_strip.GetWebContentsAt(index);

  // TODO(steimel): Why was this not a problem for AreAllTabsMuted? Is this
  // going to be a problem for SetSitesMuted?
  // Prevent crashes with null WebContents (https://crbug.com/797647).
  if (!web_contents)
    return false;

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
