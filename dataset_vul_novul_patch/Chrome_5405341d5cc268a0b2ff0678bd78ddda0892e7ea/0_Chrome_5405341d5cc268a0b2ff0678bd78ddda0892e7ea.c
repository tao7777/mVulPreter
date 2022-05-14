void RenderFrameImpl::OnSelectPopupMenuItem(int selected_index) {
  if (external_popup_menu_ == NULL)
     return;
 
   blink::WebScopedUserGesture gesture(frame_);
  // We need to reset |external_popup_menu_| before calling DidSelectItem(),
  // which might delete |this|.
  // See ExternalPopupMenuRemoveTest.RemoveFrameOnChange
  std::unique_ptr<ExternalPopupMenu> popup;
  popup.swap(external_popup_menu_);
  popup->DidSelectItem(selected_index);
 }
