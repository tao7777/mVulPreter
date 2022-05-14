 void FindBarController::Show() {
  FindManager* find_manager = tab_contents_->GetFindManager();
 
  if (!find_manager->find_ui_active()) {
     MaybeSetPrepopulateText();
 
    find_manager->set_find_ui_active(true);
     find_bar_->Show(true);
   }
   find_bar_->SetFocusAndSelection();
}
