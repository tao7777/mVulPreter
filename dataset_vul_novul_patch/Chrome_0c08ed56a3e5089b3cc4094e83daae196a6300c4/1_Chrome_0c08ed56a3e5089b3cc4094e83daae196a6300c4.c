void PresentationConnectionProxy::DidChangeState(
    content::PresentationConnectionState state) {
  if (state == content::PRESENTATION_CONNECTION_STATE_CONNECTED) {
     source_connection_->didChangeState(
         blink::WebPresentationConnectionState::Connected);
   } else if (state == content::PRESENTATION_CONNECTION_STATE_CLOSED) {
    source_connection_->didChangeState(
        blink::WebPresentationConnectionState::Closed);
   } else {
     NOTREACHED();
   }
 }
