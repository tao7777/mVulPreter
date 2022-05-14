 void WebPluginProxy::SetWindowlessPumpEvent(HANDLE pump_messages_event) {
   HANDLE pump_messages_event_for_renderer = NULL;
  DuplicateHandle(GetCurrentProcess(), pump_messages_event,
                  channel_->renderer_handle(),
                  &pump_messages_event_for_renderer,
                  0, FALSE, DUPLICATE_SAME_ACCESS);
   DCHECK(pump_messages_event_for_renderer != NULL);
   Send(new PluginHostMsg_SetWindowlessPumpEvent(
       route_id_, pump_messages_event_for_renderer));
}
