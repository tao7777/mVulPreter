void ScreenOrientationDispatcherHost::OnUnlockRequest(
    RenderFrameHost* render_frame_host) {
  if (current_lock_) {
    NotifyLockError(current_lock_->request_id,
                     blink::WebLockOrientationErrorCanceled);
   }
 
  if (!provider_)
     return;
 
   provider_->UnlockOrientation();
}
