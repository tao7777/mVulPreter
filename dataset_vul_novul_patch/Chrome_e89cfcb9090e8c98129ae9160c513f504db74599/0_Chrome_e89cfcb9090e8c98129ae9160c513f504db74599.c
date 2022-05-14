void TabCountChangeObserver::TabDetachedAt(TabContents* contents,
void TabCountChangeObserver::TabDetachedAt(WebContents* contents,
                                            int index) {
   CheckTabCount();
 }
