void AudioNode::Dispose() {
  DCHECK(IsMainThread());
#if DEBUG_AUDIONODE_REFERENCES
  fprintf(stderr, "[%16p]: %16p: %2d: AudioNode::dispose %16p\n", context(),
          this, Handler().GetNodeType(), handler_.get());
 #endif
   BaseAudioContext::GraphAutoLocker locker(context());
   Handler().Dispose();
  if (context()->ContextState() == BaseAudioContext::kRunning) {
     context()->GetDeferredTaskHandler().AddRenderingOrphanHandler(
         std::move(handler_));
   }
 }
