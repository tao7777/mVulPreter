void BrowserWindowGtk::TabDetachedAt(TabContents* contents, int index) {
void BrowserWindowGtk::TabDetachedAt(WebContents* contents, int index) {
  if (index == browser_->active_index()) {
    infobar_container_->ChangeTabContents(NULL);
    UpdateDevToolsForContents(NULL);
  }
  contents_container_->DetachTab(contents);
}
