void BrowserLauncherItemController::TabDetachedAt(TabContents* contents,
                                                  int index) {
   launcher_controller()->UpdateAppState(
      contents->web_contents(), ChromeLauncherController::APP_STATE_REMOVED);
 }
