void BrowserView::TabDetachedAt(TabContents* contents, int index) {
void BrowserView::TabDetachedAt(WebContents* contents, int index) {
  if (index == browser_->active_index()) {
    contents_container_->SetWebContents(NULL);
    infobar_container_->ChangeTabContents(NULL);
    UpdateDevToolsForContents(NULL);
  }
}
