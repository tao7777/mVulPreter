 void FindBarController::UpdateFindBarForCurrentResult() {
  FindTabHelper* find_tab_helper = tab_contents_->find_tab_helper();
  const FindNotificationDetails& find_result = find_tab_helper->find_result();
 
  if (find_result.number_of_matches() > -1) {
    if (last_reported_matchcount_ > 0 &&
        find_result.number_of_matches() == 1 &&
        !find_result.final_update())
      return;  // Don't let interim result override match count.
     last_reported_matchcount_ = find_result.number_of_matches();
   }
 
  find_bar_->UpdateUIForFindResult(find_result, find_tab_helper->find_text());
 }
