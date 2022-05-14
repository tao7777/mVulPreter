int32_t PepperFlashRendererHost::OnNavigate(
    ppapi::host::HostMessageContext* host_context,
    const ppapi::URLRequestInfoData& data,
    const std::string& target,
    bool from_user_action) {
  content::PepperPluginInstance* plugin_instance =
      host_->GetPluginInstance(pp_instance());
   if (!plugin_instance)
     return PP_ERROR_FAILED;
 
  std::map<std::string, FlashNavigateUsage>& rejected_headers =
      g_rejected_headers.Get();
  if (rejected_headers.empty()) {
    for (size_t i = 0; i < arraysize(kRejectedHttpRequestHeaders); ++i)
      rejected_headers[kRejectedHttpRequestHeaders[i]] =
          static_cast<FlashNavigateUsage>(i);
  }

  net::HttpUtil::HeadersIterator header_iter(data.headers.begin(),
                                             data.headers.end(),
                                             "\n\r");
  bool rejected = false;
  while (header_iter.GetNext()) {
    std::string lower_case_header_name = StringToLowerASCII(header_iter.name());
    if (!IsSimpleHeader(lower_case_header_name, header_iter.values())) {
      rejected = true;

      std::map<std::string, FlashNavigateUsage>::const_iterator iter =
          rejected_headers.find(lower_case_header_name);
      FlashNavigateUsage usage = iter != rejected_headers.end() ?
          iter->second : REJECT_OTHER_HEADERS;
      RecordFlashNavigateUsage(usage);
    }
  }

  RecordFlashNavigateUsage(TOTAL_NAVIGATE_REQUESTS);
  if (rejected) {
    RecordFlashNavigateUsage(TOTAL_REJECTED_NAVIGATE_REQUESTS);
    return PP_ERROR_NOACCESS;
  }

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
