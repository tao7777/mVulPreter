void OfflineAudioDestinationHandler::NotifyComplete() {
  DCHECK(IsMainThread());
 
   render_thread_.reset();
 
   if (Context() && Context()->GetExecutionContext())
     Context()->FireCompletionEvent();
}
