int32_t PepperFlashRendererHost::OnNavigate(
    ppapi::host::HostMessageContext* host_context,
    const ppapi::URLRequestInfoData& data,
    const std::string& target,
    bool from_user_action) {
  content::PepperPluginInstance* plugin_instance =
      host_->GetPluginInstance(pp_instance());
   if (!plugin_instance)
     return PP_ERROR_FAILED;
 
  ppapi::proxy::HostDispatcher* host_dispatcher =
      ppapi::proxy::HostDispatcher::GetForInstance(pp_instance());
  host_dispatcher->set_allow_plugin_reentrancy();

  base::WeakPtr<PepperFlashRendererHost> weak_ptr = weak_factory_.GetWeakPtr();
  navigate_replies_.push_back(host_context->MakeReplyMessageContext());
  plugin_instance->Navigate(data, target.c_str(), from_user_action);
  if (weak_ptr.get()) {
    SendReply(navigate_replies_.back(), IPC::Message());
    navigate_replies_.pop_back();
  }

  return PP_OK_COMPLETIONPENDING;
}
