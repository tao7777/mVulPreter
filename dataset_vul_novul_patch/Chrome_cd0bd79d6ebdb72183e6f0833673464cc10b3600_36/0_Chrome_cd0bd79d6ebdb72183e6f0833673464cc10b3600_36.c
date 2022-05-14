 void WebPluginProxy::SetWindowlessPumpEvent(HANDLE pump_messages_event) {
   HANDLE pump_messages_event_for_renderer = NULL;
  sandbox::BrokerDuplicateHandle(pump_messages_event, channel_->peer_pid(),
                                 &pump_messages_event_for_renderer,
                                 SYNCHRONIZE | EVENT_MODIFY_STATE, 0);
   DCHECK(pump_messages_event_for_renderer != NULL);
   Send(new PluginHostMsg_SetWindowlessPumpEvent(
       route_id_, pump_messages_event_for_renderer));
}
