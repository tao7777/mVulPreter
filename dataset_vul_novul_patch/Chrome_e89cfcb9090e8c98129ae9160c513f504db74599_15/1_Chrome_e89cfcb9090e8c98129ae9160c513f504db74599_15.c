void UnloadController::TabDetachedImpl(TabContents* contents) {
   if (is_attempting_to_close_browser_)
    ClearUnloadState(contents->web_contents(), false);
  registrar_.Remove(
      this,
      content::NOTIFICATION_WEB_CONTENTS_DISCONNECTED,
      content::Source<content::WebContents>(contents->web_contents()));
 }
