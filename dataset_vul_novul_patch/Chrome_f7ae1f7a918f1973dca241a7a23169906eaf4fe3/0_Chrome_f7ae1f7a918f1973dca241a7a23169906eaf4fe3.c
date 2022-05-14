void MessageService::OpenChannelToExtension(
    int source_process_id, int source_routing_id, int receiver_port_id,
    const std::string& source_extension_id,
    const std::string& target_extension_id,
    const std::string& channel_name) {
  content::RenderProcessHost* source =
      content::RenderProcessHost::FromID(source_process_id);
  if (!source)
    return;
  Profile* profile = Profile::FromBrowserContext(source->GetBrowserContext());

  MessagePort* receiver = new ExtensionMessagePort(
      GetExtensionProcess(profile, target_extension_id), MSG_ROUTING_CONTROL,
      target_extension_id);
  WebContents* source_contents = tab_util::GetWebContentsByID(
      source_process_id, source_routing_id);

   std::string tab_json = "null";
   if (source_contents) {
     scoped_ptr<DictionaryValue> tab_value(ExtensionTabUtil::CreateTabValue(
        source_contents));
     base::JSONWriter::Write(tab_value.get(), &tab_json);
   }
 
  OpenChannelParams* params = new OpenChannelParams(source, tab_json, receiver,
                                                    receiver_port_id,
                                                    source_extension_id,
                                                    target_extension_id,
                                                    channel_name);

  if (MaybeAddPendingOpenChannelTask(profile, params)) {
    return;
  }

  OpenChannelImpl(scoped_ptr<OpenChannelParams>(params));
}
