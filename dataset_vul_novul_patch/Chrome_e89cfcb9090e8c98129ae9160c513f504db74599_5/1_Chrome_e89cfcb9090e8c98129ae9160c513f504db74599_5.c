 void BrowserCommandController::RemoveInterstitialObservers(
    TabContents* contents) {
   registrar_.Remove(this, content::NOTIFICATION_INTERSTITIAL_ATTACHED,
                    content::Source<WebContents>(contents->web_contents()));
   registrar_.Remove(this, content::NOTIFICATION_INTERSTITIAL_DETACHED,
                    content::Source<WebContents>(contents->web_contents()));
 }
