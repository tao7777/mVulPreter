void UnloadController::TabDetachedAt(TabContents* contents, int index) {
void UnloadController::TabDetachedAt(content::WebContents* contents,
                                     int index) {
   TabDetachedImpl(contents);
 }
