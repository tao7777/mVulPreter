void Browser::TabDetachedAt(TabContents* contents, int index) {
   TabDetachedAtImpl(contents, index, DETACH_TYPE_DETACH);
 }
