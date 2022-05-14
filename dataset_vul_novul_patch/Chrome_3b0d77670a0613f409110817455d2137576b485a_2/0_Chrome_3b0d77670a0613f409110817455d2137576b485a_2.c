bool NaClProcessHost::ReplyToRenderer(
bool NaClProcessHost::ReplyToRenderer() {
   std::vector<nacl::FileDescriptor> handles_for_renderer;
   for (size_t i = 0; i < internal_->sockets_for_renderer.size(); i++) {
 #if defined(OS_WIN)
    HANDLE handle_in_renderer;
    if (!DuplicateHandle(base::GetCurrentProcessHandle(),
                         reinterpret_cast<HANDLE>(
                             internal_->sockets_for_renderer[i]),
                         chrome_render_message_filter_->peer_handle(),
                         &handle_in_renderer,
                         0,  // Unused given DUPLICATE_SAME_ACCESS.
                         FALSE,
                         DUPLICATE_CLOSE_SOURCE | DUPLICATE_SAME_ACCESS)) {
      DLOG(ERROR) << "DuplicateHandle() failed";
      return false;
    }
    handles_for_renderer.push_back(
        reinterpret_cast<nacl::FileDescriptor>(handle_in_renderer));
#else
    nacl::FileDescriptor imc_handle;
    imc_handle.fd = internal_->sockets_for_renderer[i];
    imc_handle.auto_close = true;
    handles_for_renderer.push_back(imc_handle);
#endif
  }

#if defined(OS_WIN)
  if (RunningOnWOW64()) {
    if (!content::BrokerAddTargetPeer(process_->GetData().handle)) {
      DLOG(ERROR) << "Failed to add NaCl process PID";
      return false;
    }
  }
 #endif
 
   ChromeViewHostMsg_LaunchNaCl::WriteReplyParams(
      reply_msg_, handles_for_renderer);
   chrome_render_message_filter_->Send(reply_msg_);
   chrome_render_message_filter_ = NULL;
   reply_msg_ = NULL;
  internal_->sockets_for_renderer.clear();
  return true;
}
