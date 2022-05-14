 void BrowserCommandController::TabReplacedAt(TabStripModel* tab_strip_model,
                                              TabContents* old_contents,
                                              TabContents* new_contents,
                                              int index) {
  RemoveInterstitialObservers(old_contents);
   AddInterstitialObservers(new_contents->web_contents());
 }
