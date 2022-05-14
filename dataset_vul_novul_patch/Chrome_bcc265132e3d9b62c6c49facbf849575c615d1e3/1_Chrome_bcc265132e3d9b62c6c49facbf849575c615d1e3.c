void ScreenOrientationDispatcherHost::OnLockRequest(
    RenderFrameHost* render_frame_host,
    blink::WebScreenOrientationLockType orientation,
    int request_id) {
  if (current_lock_) {
    NotifyLockError(current_lock_->request_id,
                     blink::WebLockOrientationErrorCanceled);
   }
 
  current_lock_ = new LockInformation(request_id,
                                      render_frame_host->GetProcess()->GetID(),
                                      render_frame_host->GetRoutingID());
   if (!provider_) {
     NotifyLockError(request_id,
                     blink::WebLockOrientationErrorNotAvailable);
     return;
   }
 
   provider_->LockOrientation(request_id, orientation);
 }
