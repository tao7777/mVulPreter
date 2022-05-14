void BrowserTabStripController::TabDetachedAt(TabContents* contents,
void BrowserTabStripController::TabDetachedAt(WebContents* contents,
                                               int model_index) {
   hover_tab_selector_.CancelTabTransition();

  tabstrip_->RemoveTabAt(model_index);
}
