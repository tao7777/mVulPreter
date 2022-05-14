bool WebDriverCommand::Init(Response* const response) {
  std::string session_id = GetPathVariable(2);
  if (session_id.length() == 0) {
    response->SetError(
        new Error(kBadRequest, "No session ID specified"));
     return false;
   }
 
  VLOG(1) << "Fetching session: " << session_id;
   session_ = SessionManager::GetInstance()->GetSession(session_id);
   if (session_ == NULL) {
     response->SetError(
         new Error(kSessionNotFound, "Session not found: " + session_id));
     return false;
   }
 
  scoped_ptr<Error> error(session_->WaitForAllTabsToStopLoading());
  if (error.get()) {
    LOG(WARNING) << error->ToString();
   }
  error.reset(session_->SwitchToTopFrameIfCurrentFrameInvalid());
  if (error.get()) {
    LOG(WARNING) << error->ToString();
   }
 
   response->SetField("sessionId", Value::CreateStringValue(session_id));
  return true;
}
