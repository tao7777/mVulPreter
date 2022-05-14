void BrowserCommandController::TabDetachedAt(TabContents* contents, int index) {
   RemoveInterstitialObservers(contents);
 }
