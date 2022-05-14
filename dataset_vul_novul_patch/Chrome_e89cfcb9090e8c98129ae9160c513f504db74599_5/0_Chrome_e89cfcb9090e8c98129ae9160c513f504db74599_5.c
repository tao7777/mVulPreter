 void BrowserCommandController::RemoveInterstitialObservers(
    WebContents* contents) {
   registrar_.Remove(this, content::NOTIFICATION_INTERSTITIAL_ATTACHED,
                    content::Source<WebContents>(contents));
   registrar_.Remove(this, content::NOTIFICATION_INTERSTITIAL_DETACHED,
                    content::Source<WebContents>(contents));
 }
