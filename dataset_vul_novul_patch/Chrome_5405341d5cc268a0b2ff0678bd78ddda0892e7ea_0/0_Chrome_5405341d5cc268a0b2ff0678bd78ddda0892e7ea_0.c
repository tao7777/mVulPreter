 void RenderFrameImpl::OnSelectPopupMenuItems(
    bool canceled,
    const std::vector<int>& selected_indices) {
  if (!external_popup_menu_)
     return;
 
   blink::WebScopedUserGesture gesture(frame_);
  // We need to reset |external_popup_menu_| before calling DidSelectItems(),
  // which might delete |this|.
  // See ExternalPopupMenuRemoveTest.RemoveFrameOnChange
  std::unique_ptr<ExternalPopupMenu> popup;
  popup.swap(external_popup_menu_);
  popup->DidSelectItems(canceled, selected_indices);
 }
