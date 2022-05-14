 void SendTabToSelfInfoBarDelegate::OpenTab() {
  content::OpenURLParams open_url_params(
      entry_->GetURL(), content::Referrer(),
      WindowOpenDisposition::NEW_FOREGROUND_TAB,
      ui::PageTransition::PAGE_TRANSITION_LINK,
      false /* is_renderer_initiated */);
  web_contents_->OpenURL(open_url_params);

  // TODO(crbug.com/944602): Update the model to reflect that an infobar is
  // shown.
 }
