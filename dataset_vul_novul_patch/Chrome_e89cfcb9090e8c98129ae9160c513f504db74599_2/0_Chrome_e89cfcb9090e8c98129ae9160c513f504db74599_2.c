void Browser::TabDetachedAt(TabContents* contents, int index) {
void Browser::TabDetachedAt(WebContents* contents, int index) {
   TabDetachedAtImpl(contents, index, DETACH_TYPE_DETACH);
 }
