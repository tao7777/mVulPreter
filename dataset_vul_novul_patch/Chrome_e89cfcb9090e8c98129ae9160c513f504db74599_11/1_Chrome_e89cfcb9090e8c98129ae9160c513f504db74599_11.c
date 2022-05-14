void TabStripGtk::TabDetachedAt(TabContents* contents, int index) {
   GenerateIdealBounds();
  StartRemoveTabAnimation(index, contents->web_contents());
  GetTabAt(index)->set_closing(true);
}
