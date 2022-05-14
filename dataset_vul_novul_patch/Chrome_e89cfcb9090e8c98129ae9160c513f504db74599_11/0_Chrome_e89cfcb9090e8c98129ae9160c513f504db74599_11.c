void TabStripGtk::TabDetachedAt(TabContents* contents, int index) {
void TabStripGtk::TabDetachedAt(WebContents* contents, int index) {
   GenerateIdealBounds();
  StartRemoveTabAnimation(index, contents);
  GetTabAt(index)->set_closing(true);
}
