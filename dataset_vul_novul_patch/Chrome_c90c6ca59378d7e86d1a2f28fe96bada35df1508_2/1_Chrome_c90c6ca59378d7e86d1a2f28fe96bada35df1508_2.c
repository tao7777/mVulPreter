void FindBarController::EndFindSession(SelectionAction action) {
  find_bar_->Hide(true);

   if (tab_contents_) {
    FindManager* find_manager = tab_contents_->GetFindManager();
 
    find_manager->StopFinding(action);
 
     if (action != kKeepSelection)
      find_bar_->ClearResults(find_manager->find_result());
 
     find_bar_->RestoreSavedFocus();
  }
}
