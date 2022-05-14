 void PresentationConnectionProxy::OnClose() {
   DCHECK(target_connection_ptr_);
  source_connection_->didChangeState(
      blink::WebPresentationConnectionState::Closed);
   target_connection_ptr_->DidChangeState(
       content::PRESENTATION_CONNECTION_STATE_CLOSED);
 }
