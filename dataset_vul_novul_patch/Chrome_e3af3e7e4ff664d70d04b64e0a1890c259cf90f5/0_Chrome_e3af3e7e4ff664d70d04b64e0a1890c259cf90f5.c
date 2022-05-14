 void BaseAudioContext::ContextDestroyed(ExecutionContext*) {
  destination()->GetAudioDestinationHandler().ContextDestroyed();
   Uninitialize();
 }
