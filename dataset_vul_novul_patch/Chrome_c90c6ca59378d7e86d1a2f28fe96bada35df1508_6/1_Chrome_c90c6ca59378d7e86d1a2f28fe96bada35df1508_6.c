 void FindBarController::UpdateFindBarForCurrentResult() {
  FindManager* find_manager = tab_contents_->GetFindManager();
  const FindNotificationDetails& find_result = find_manager->find_result();
 
  if (find_result.number_of_matches() > -1) {
    if (last_reported_matchcount_ > 0 &&
        find_result.number_of_matches() == 1 &&
        !find_result.final_update())
      return;  // Don't let interim result override match count.
     last_reported_matchcount_ = find_result.number_of_matches();
   }
 
  find_bar_->UpdateUIForFindResult(find_result, find_manager->find_text());
 }
