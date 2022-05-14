void AutomationProvider::SendFindRequest(
    TabContents* tab_contents,
    bool with_json,
    const string16& search_string,
    bool forward,
    bool match_case,
    bool find_next,
    IPC::Message* reply_message) {
  int request_id = FindInPageNotificationObserver::kFindInPageRequestId;
  FindInPageNotificationObserver* observer =
      new FindInPageNotificationObserver(this,
                                         tab_contents,
                                         with_json,
                                         reply_message);
  if (!with_json) {
    find_in_page_observer_.reset(observer);
  }
   TabContentsWrapper* wrapper =
       TabContentsWrapper::GetCurrentWrapperForContents(tab_contents);
   if (wrapper)
    wrapper->GetFindManager()->set_current_find_request_id(request_id);
 
   tab_contents->render_view_host()->StartFinding(
       FindInPageNotificationObserver::kFindInPageRequestId,
      search_string,
      forward,
      match_case,
      find_next);
}
