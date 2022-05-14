 void FindBarController::Observe(NotificationType type,
                                 const NotificationSource& source,
                                 const NotificationDetails& details) {
  FindManager* find_manager = tab_contents_->GetFindManager();
   if (type == NotificationType::FIND_RESULT_AVAILABLE) {
     if (Source<TabContents>(source).ptr() == tab_contents_->tab_contents()) {
       UpdateFindBarForCurrentResult();
      if (find_manager->find_result().final_update() &&
          find_manager->find_result().number_of_matches() == 0) {
        const string16& last_search = find_manager->previous_find_text();
        const string16& current_search = find_manager->find_text();
         if (last_search.find(current_search) != 0)
           find_bar_->AudibleAlert();
       }
    }
  } else if (type == NotificationType::NAV_ENTRY_COMMITTED) {
    NavigationController* source_controller =
        Source<NavigationController>(source).ptr();
    if (source_controller == &tab_contents_->controller()) {
      NavigationController::LoadCommittedDetails* commit_details =
          Details<NavigationController::LoadCommittedDetails>(details).ptr();
      PageTransition::Type transition_type =
          commit_details->entry->transition_type();
      if (find_bar_->IsFindBarVisible()) {
        if (PageTransition::StripQualifier(transition_type) !=
            PageTransition::RELOAD) {
          EndFindSession(kKeepSelection);
         } else {
          find_manager->set_find_op_aborted(true);
         }
       }
     }
  }
}
