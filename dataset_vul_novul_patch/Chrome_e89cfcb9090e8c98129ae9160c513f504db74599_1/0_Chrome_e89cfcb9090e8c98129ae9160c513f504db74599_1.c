void BrowserLauncherItemController::TabDetachedAt(TabContents* contents,
void BrowserLauncherItemController::TabDetachedAt(
    content::WebContents* contents,
    int index) {
   launcher_controller()->UpdateAppState(
      contents, ChromeLauncherController::APP_STATE_REMOVED);
 }
