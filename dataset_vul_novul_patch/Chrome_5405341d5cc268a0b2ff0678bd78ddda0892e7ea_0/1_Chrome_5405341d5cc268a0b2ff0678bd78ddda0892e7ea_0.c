 void RenderFrameImpl::OnSelectPopupMenuItems(
    bool canceled,
    const std::vector<int>& selected_indices) {
  if (!external_popup_menu_)
     return;
 
   blink::WebScopedUserGesture gesture(frame_);
  external_popup_menu_->DidSelectItems(canceled, selected_indices);
  external_popup_menu_.reset();
 }
