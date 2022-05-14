void RenderFrameHostImpl::OnDidAddMessageToConsole(
    int32_t level,
     const base::string16& message,
     int32_t line_no,
     const base::string16& source_id) {
  if (level < logging::LOG_VERBOSE || level > logging::LOG_FATAL) {
    bad_message::ReceivedBadMessage(
        GetProcess(), bad_message::RFH_DID_ADD_CONSOLE_MESSAGE_BAD_SEVERITY);
    return;
   }
 
  if (delegate_->DidAddMessageToConsole(level, message, line_no, source_id))
     return;
 
   const bool is_builtin_component =
       HasWebUIScheme(delegate_->GetMainFrameLastCommittedURL()) ||
      GetContentClient()->browser()->IsBuiltinComponent(
          GetProcess()->GetBrowserContext(), GetLastCommittedOrigin());
   const bool is_off_the_record =
       GetSiteInstance()->GetBrowserContext()->IsOffTheRecord();
 
  LogConsoleMessage(level, message, line_no, is_builtin_component,
                     is_off_the_record, source_id);
 }
