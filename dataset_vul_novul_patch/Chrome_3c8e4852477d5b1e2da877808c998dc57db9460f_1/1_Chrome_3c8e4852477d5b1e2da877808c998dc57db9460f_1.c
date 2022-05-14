 void DevToolsSession::ReceivedBadMessage() {
   MojoConnectionDestroyed();
  if (process_) {
     bad_message::ReceivedBadMessage(
        process_, bad_message::RFH_INCONSISTENT_DEVTOOLS_MESSAGE);
   }
 }
