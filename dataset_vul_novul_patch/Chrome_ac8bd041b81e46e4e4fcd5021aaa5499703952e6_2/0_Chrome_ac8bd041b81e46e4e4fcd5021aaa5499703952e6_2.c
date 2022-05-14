void PluginInfoMessageFilter::PluginsLoaded(
    const GetPluginInfo_Params& params,
    IPC::Message* reply_msg,
    const std::vector<WebPluginInfo>& plugins) {
  ChromeViewHostMsg_GetPluginInfo_Output output;
  scoped_ptr<PluginMetadata> plugin_metadata;
  if (context_.FindEnabledPlugin(params.render_view_id, params.url,
                                 params.top_origin_url, params.mime_type,
                                 &output.status, &output.plugin,
                                 &output.actual_mime_type,
                                 &plugin_metadata)) {
    context_.DecidePluginStatus(params, output.plugin, plugin_metadata.get(),
                                &output.status);
  }

  if (plugin_metadata) {
    output.group_identifier = plugin_metadata->identifier();
     output.group_name = plugin_metadata->name();
   }
 
  context_.MaybeGrantAccess(output.status, output.plugin.path);
 
   ChromeViewHostMsg_GetPluginInfo::WriteReplyParams(reply_msg, output);
   Send(reply_msg);
}
