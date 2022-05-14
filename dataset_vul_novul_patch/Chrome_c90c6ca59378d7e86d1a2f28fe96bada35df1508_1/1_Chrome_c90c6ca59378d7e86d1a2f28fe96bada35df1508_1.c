void FindBarController::ChangeTabContents(TabContentsWrapper* contents) {
  if (tab_contents_) {
    registrar_.RemoveAll();
    find_bar_->StopAnimation();
  }

  tab_contents_ = contents;

   if (find_bar_->IsFindBarVisible() &&
      (!tab_contents_ || !tab_contents_->GetFindManager()->find_ui_active())) {
     find_bar_->Hide(false);
   }
 
  if (!tab_contents_)
    return;

  registrar_.Add(this, NotificationType::FIND_RESULT_AVAILABLE,
                 Source<TabContents>(tab_contents_->tab_contents()));
  registrar_.Add(this, NotificationType::NAV_ENTRY_COMMITTED,
                 Source<NavigationController>(&tab_contents_->controller()));
 
   MaybeSetPrepopulateText();
 
  if (tab_contents_->GetFindManager()->find_ui_active()) {
    find_bar_->Show(false);
  }

  UpdateFindBarForCurrentResult();
}
