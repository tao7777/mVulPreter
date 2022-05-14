 void SynchronousCompositorExternalBeginFrameSource::SetCompositor(
     SynchronousCompositorImpl* compositor) {
   DCHECK(CalledOnValidThread());
   compositor_ = compositor;
 }
