void Browser::SetWebContentsBlocked(content::WebContents* web_contents,
                                    bool blocked) {
  int index = tab_strip_model_->GetIndexOfWebContents(web_contents);
  if (index == TabStripModel::kNoTab) {
     return;
   }

  // For security, if the WebContents is in fullscreen, have it drop fullscreen.
  // This gives the user the context they need in order to make informed
  // decisions.
  if (web_contents->IsFullscreenForCurrentTab())
    web_contents->ExitFullscreen(true);

   tab_strip_model_->SetTabBlocked(index, blocked);
 
   bool browser_active = BrowserList::GetInstance()->GetLastActive() == this;
  bool contents_is_active =
      tab_strip_model_->GetActiveWebContents() == web_contents;
  if (!blocked && contents_is_active && browser_active)
    web_contents->Focus();
}
