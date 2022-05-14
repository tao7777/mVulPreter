 void FindBarController::MaybeSetPrepopulateText() {
 #if !defined(OS_MACOSX)
  FindManager* find_manager = tab_contents_->GetFindManager();
  string16 find_string = find_manager->find_text();
   if (find_string.empty())
    find_string = find_manager->previous_find_text();
   if (find_string.empty()) {
     find_string =
         FindBarState::GetLastPrepopulateText(tab_contents_->profile());
  }

  find_bar_->SetFindText(find_string);
#else
#endif
}
