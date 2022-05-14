void OfflineAudioDestinationHandler::NotifyComplete() {
  DCHECK(IsMainThread());
 
   render_thread_.reset();
 
  // If the execution context has been destroyed, there's no where to
  // send the notification, so just return.
  if (IsExecutionContextDestroyed()) {
    return;
  }

   if (Context() && Context()->GetExecutionContext())
     Context()->FireCompletionEvent();
}
