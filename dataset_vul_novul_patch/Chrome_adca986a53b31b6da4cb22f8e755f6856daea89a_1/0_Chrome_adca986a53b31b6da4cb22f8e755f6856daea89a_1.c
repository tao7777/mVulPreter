 void RenderFrameHostManager::EnsureRenderFrameHostVisibilityConsistent() {
  RenderWidgetHostView* view = GetRenderWidgetHostView();
  if (view && static_cast<RenderWidgetHostImpl*>(view->GetRenderWidgetHost())
                      ->is_hidden() != delegate_->IsHidden()) {
     if (delegate_->IsHidden()) {
      view->Hide();
     } else {
      view->Show();
     }
   }
 }
