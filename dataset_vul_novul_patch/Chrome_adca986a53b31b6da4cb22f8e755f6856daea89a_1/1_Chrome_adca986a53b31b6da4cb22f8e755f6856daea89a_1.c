 void RenderFrameHostManager::EnsureRenderFrameHostVisibilityConsistent() {
  if (render_frame_host_->GetView() &&
      render_frame_host_->render_view_host()->GetWidget()->is_hidden() !=
          delegate_->IsHidden()) {
     if (delegate_->IsHidden()) {
      render_frame_host_->GetView()->Hide();
     } else {
      render_frame_host_->GetView()->Show();
     }
   }
 }
