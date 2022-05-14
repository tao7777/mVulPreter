 void UnloadController::TabReplacedAt(TabStripModel* tab_strip_model,
                                      TabContents* old_contents,
                                      TabContents* new_contents,
                                      int index) {
  TabDetachedImpl(old_contents);
   TabAttachedImpl(new_contents->web_contents());
 }
