 void InfoBarContainer::ChangeTabContents(TabContents* contents) {
   registrar_.RemoveAll();
  // hierarchy does this automatically (see InfoBarView::InfoBarRemoved).
   RemoveAllChildViews(false);
   tab_contents_ = contents;
   if (tab_contents_) {
    UpdateInfoBars();
    Source<TabContents> tc_source(tab_contents_);
    registrar_.Add(this, NotificationType::TAB_CONTENTS_INFOBAR_ADDED,
                   tc_source);
    registrar_.Add(this, NotificationType::TAB_CONTENTS_INFOBAR_REMOVED,
                   tc_source);
    registrar_.Add(this, NotificationType::TAB_CONTENTS_INFOBAR_REPLACED,
                   tc_source);
  }
}
