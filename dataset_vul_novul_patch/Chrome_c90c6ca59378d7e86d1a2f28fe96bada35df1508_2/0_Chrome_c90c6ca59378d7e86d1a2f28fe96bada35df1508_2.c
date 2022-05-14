void FindBarController::EndFindSession(SelectionAction action) {
  find_bar_->Hide(true);

   if (tab_contents_) {
    FindTabHelper* find_tab_helper = tab_contents_->find_tab_helper();
 
    find_tab_helper->StopFinding(action);
 
     if (action != kKeepSelection)
      find_bar_->ClearResults(find_tab_helper->find_result());
 
     find_bar_->RestoreSavedFocus();
  }
}
