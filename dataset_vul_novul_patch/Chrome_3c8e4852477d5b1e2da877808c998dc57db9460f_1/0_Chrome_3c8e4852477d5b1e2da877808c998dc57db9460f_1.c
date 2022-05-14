 void DevToolsSession::ReceivedBadMessage() {
   MojoConnectionDestroyed();
  RenderProcessHost* process = RenderProcessHost::FromID(process_host_id_);
  if (process) {
     bad_message::ReceivedBadMessage(
        process, bad_message::RFH_INCONSISTENT_DEVTOOLS_MESSAGE);
   }
 }
