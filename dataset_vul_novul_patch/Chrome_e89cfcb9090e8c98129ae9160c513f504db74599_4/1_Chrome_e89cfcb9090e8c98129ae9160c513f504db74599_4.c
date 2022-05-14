void Browser::TabReplacedAt(TabStripModel* tab_strip_model,
                             TabContents* old_contents,
                             TabContents* new_contents,
                             int index) {
  TabDetachedAtImpl(old_contents, index, DETACH_TYPE_REPLACE);
   SessionService* session_service =
       SessionServiceFactory::GetForProfile(profile_);
   if (session_service)
    session_service->TabClosing(old_contents->web_contents());
  TabInsertedAt(new_contents->web_contents(), index, (index == active_index()));

  int entry_count =
      new_contents->web_contents()->GetController().GetEntryCount();
  if (entry_count > 0) {
    new_contents->web_contents()->GetController().NotifyEntryChanged(
        new_contents->web_contents()->GetController().GetEntryAtIndex(
            entry_count - 1),
        entry_count - 1);
  }

  if (session_service) {
    session_service->TabRestored(new_contents,
                                 tab_strip_model_->IsTabPinned(index));
  }

  content::DevToolsManager::GetInstance()->ContentsReplaced(
      old_contents->web_contents(), new_contents->web_contents());
}
