bool BaseSessionService::RestoreUpdateTabNavigationCommand(
    const SessionCommand& command,
    TabNavigation* navigation,
    SessionID::id_type* tab_id) {
  scoped_ptr<Pickle> pickle(command.PayloadAsPickle());
  if (!pickle.get())
    return false;
  void* iterator = NULL;
  std::string url_spec;
  if (!pickle->ReadInt(&iterator, tab_id) ||
      !pickle->ReadInt(&iterator, &(navigation->index_)) ||
      !pickle->ReadString(&iterator, &url_spec) ||
      !pickle->ReadString16(&iterator, &(navigation->title_)) ||
      !pickle->ReadString(&iterator, &(navigation->state_)) ||
      !pickle->ReadInt(&iterator,
                       reinterpret_cast<int*>(&(navigation->transition_))))
    return false;
  bool has_type_mask = pickle->ReadInt(&iterator, &(navigation->type_mask_));

  if (has_type_mask) {
    std::string referrer_spec;
    pickle->ReadString(&iterator, &referrer_spec);
    int policy_int;
    WebReferrerPolicy policy;
    if (pickle->ReadInt(&iterator, &policy_int))
      policy = static_cast<WebReferrerPolicy>(policy_int);
    else
      policy = WebKit::WebReferrerPolicyDefault;
    navigation->referrer_ = content::Referrer(
        referrer_spec.empty() ? GURL() : GURL(referrer_spec),
         policy);
 
     std::string content_state;
     if (CompressDataHelper::ReadAndDecompressStringFromPickle(
             *pickle.get(), &iterator, &content_state) &&
         !content_state.empty()) {
       navigation->state_ = content_state;
     }
   }
 
   navigation->virtual_url_ = GURL(url_spec);
  return true;
}
