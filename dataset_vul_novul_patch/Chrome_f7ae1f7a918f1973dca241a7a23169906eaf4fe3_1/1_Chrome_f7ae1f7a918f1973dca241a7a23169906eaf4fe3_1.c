void MessageService::OpenChannelToTab(
    int source_process_id, int source_routing_id, int receiver_port_id,
    int tab_id, const std::string& extension_id,
    const std::string& channel_name) {
  content::RenderProcessHost* source =
      content::RenderProcessHost::FromID(source_process_id);
  if (!source)
    return;
  Profile* profile = Profile::FromBrowserContext(source->GetBrowserContext());

  WebContents* contents = NULL;
  scoped_ptr<MessagePort> receiver;
  if (ExtensionTabUtil::GetTabById(tab_id, profile, true,
                                   NULL, NULL, &contents, NULL)) {
    receiver.reset(new ExtensionMessagePort(
        contents->GetRenderProcessHost(),
        contents->GetRenderViewHost()->GetRoutingID(),
        extension_id));
  }

  if (contents && contents->GetController().NeedsReload()) {
    ExtensionMessagePort port(source, MSG_ROUTING_CONTROL, extension_id);
    port.DispatchOnDisconnect(GET_OPPOSITE_PORT_ID(receiver_port_id), true);
    return;
  }

  WebContents* source_contents = tab_util::GetWebContentsByID(
      source_process_id, source_routing_id);

   std::string tab_json = "null";
   if (source_contents) {
     scoped_ptr<DictionaryValue> tab_value(ExtensionTabUtil::CreateTabValue(
        source_contents, ExtensionTabUtil::INCLUDE_PRIVACY_SENSITIVE_FIELDS));
     base::JSONWriter::Write(tab_value.get(), &tab_json);
   }
 
  scoped_ptr<OpenChannelParams> params(new OpenChannelParams(source, tab_json,
                                                             receiver.release(),
                                                             receiver_port_id,
                                                             extension_id,
                                                             extension_id,
                                                             channel_name));
  OpenChannelImpl(params.Pass());
}
