void UnloadController::TabDetachedImpl(TabContents* contents) {
void UnloadController::TabDetachedImpl(content::WebContents* contents) {
   if (is_attempting_to_close_browser_)
    ClearUnloadState(contents, false);
  registrar_.Remove(this,
                    content::NOTIFICATION_WEB_CONTENTS_DISCONNECTED,
                    content::Source<content::WebContents>(contents));
 }
