 void SynchronousCompositorImpl::DidOverscroll(
     const DidOverscrollParams& params) {
   DCHECK(compositor_client_);
  compositor_client_->DidOverscroll(params.accumulated_overscroll,
                                    params.latest_overscroll_delta,
                                    params.current_fling_velocity);
 }
