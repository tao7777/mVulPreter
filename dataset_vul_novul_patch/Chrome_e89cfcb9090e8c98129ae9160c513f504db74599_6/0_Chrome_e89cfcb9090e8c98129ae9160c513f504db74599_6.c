void BrowserCommandController::TabDetachedAt(TabContents* contents, int index) {
void BrowserCommandController::TabDetachedAt(WebContents* contents, int index) {
   RemoveInterstitialObservers(contents);
 }
