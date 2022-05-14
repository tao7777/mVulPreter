void RenderMessageFilter::GetPluginsCallback(
    IPC::Message* reply_msg,
    const std::vector<webkit::WebPluginInfo>& all_plugins) {
  PluginServiceFilter* filter = PluginServiceImpl::GetInstance()->GetFilter();
  std::vector<webkit::WebPluginInfo> plugins;

  int child_process_id = -1;
  int routing_id = MSG_ROUTING_NONE;
   for (size_t i = 0; i < all_plugins.size(); ++i) {
     webkit::WebPluginInfo plugin(all_plugins[i]);
    if (!filter || filter->IsPluginAvailable(child_process_id,
                                             routing_id,
                                             resource_context_,
                                             GURL(),
                                             GURL(),
                                             &plugin)) {
       plugins.push_back(plugin);
     }
   }

  ViewHostMsg_GetPlugins::WriteReplyParams(reply_msg, plugins);
  Send(reply_msg);
}
