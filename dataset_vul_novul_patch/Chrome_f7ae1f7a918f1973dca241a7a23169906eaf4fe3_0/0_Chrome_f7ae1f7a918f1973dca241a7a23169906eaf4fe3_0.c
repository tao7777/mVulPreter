void MessageService::OpenChannelToNativeApp(
    int source_process_id,
    int source_routing_id,
    int receiver_port_id,
    const std::string& source_extension_id,
    const std::string& native_app_name,
    const std::string& channel_name,
    const std::string& connect_message) {
  content::RenderProcessHost* source =
      content::RenderProcessHost::FromID(source_process_id);
  if (!source)
    return;

  WebContents* source_contents = tab_util::GetWebContentsByID(
      source_process_id, source_routing_id);

   std::string tab_json = "null";
   if (source_contents) {
     scoped_ptr<DictionaryValue> tab_value(ExtensionTabUtil::CreateTabValue(
        source_contents));
     base::JSONWriter::Write(tab_value.get(), &tab_json);
   }
 
  scoped_ptr<MessageChannel> channel(new MessageChannel());
  channel->opener.reset(new ExtensionMessagePort(source, MSG_ROUTING_CONTROL,
                                                 source_extension_id));

  NativeMessageProcessHost::MessageType type =
      channel_name == "chrome.runtime.sendNativeMessage" ?
      NativeMessageProcessHost::TYPE_SEND_MESSAGE_REQUEST :
      NativeMessageProcessHost::TYPE_CONNECT;

  content::BrowserThread::PostTask(
      content::BrowserThread::FILE,
      FROM_HERE,
      base::Bind(&NativeMessageProcessHost::Create,
                 base::WeakPtr<NativeMessageProcessHost::Client>(
                    weak_factory_.GetWeakPtr()),
                 native_app_name, connect_message, receiver_port_id,
                 type,
                 base::Bind(&MessageService::FinalizeOpenChannelToNativeApp,
                            weak_factory_.GetWeakPtr(),
                            receiver_port_id,
                            channel_name,
                            base::Passed(&channel),
                            tab_json)));
}
