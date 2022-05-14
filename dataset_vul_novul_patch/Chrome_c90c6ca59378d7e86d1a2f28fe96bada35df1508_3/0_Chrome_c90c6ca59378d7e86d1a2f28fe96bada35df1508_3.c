 void FindBarController::MaybeSetPrepopulateText() {
 #if !defined(OS_MACOSX)
  FindTabHelper* find_tab_helper = tab_contents_->find_tab_helper();
  string16 find_string = find_tab_helper->find_text();
   if (find_string.empty())
    find_string = find_tab_helper->previous_find_text();
   if (find_string.empty()) {
     find_string =
         FindBarState::GetLastPrepopulateText(tab_contents_->profile());
  }

  find_bar_->SetFindText(find_string);
#else
#endif
}
