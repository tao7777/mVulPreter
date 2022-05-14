void RenderFrameImpl::OnSelectPopupMenuItem(int selected_index) {
  if (external_popup_menu_ == NULL)
     return;
 
   blink::WebScopedUserGesture gesture(frame_);
  external_popup_menu_->DidSelectItem(selected_index);
  external_popup_menu_.reset();
 }
