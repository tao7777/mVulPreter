 void FindBarController::Observe(NotificationType type,
                                 const NotificationSource& source,
                                 const NotificationDetails& details) {
  FindTabHelper* find_tab_helper = tab_contents_->find_tab_helper();
   if (type == NotificationType::FIND_RESULT_AVAILABLE) {
     if (Source<TabContents>(source).ptr() == tab_contents_->tab_contents()) {
       UpdateFindBarForCurrentResult();
      if (find_tab_helper->find_result().final_update() &&
          find_tab_helper->find_result().number_of_matches() == 0) {
        const string16& last_search = find_tab_helper->previous_find_text();
        const string16& current_search = find_tab_helper->find_text();
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
          find_tab_helper->set_find_op_aborted(true);
         }
       }
     }
  }
}
