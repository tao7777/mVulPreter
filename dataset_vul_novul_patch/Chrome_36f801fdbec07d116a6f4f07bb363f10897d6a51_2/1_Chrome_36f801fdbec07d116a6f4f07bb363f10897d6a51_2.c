void RenderViewImpl::DidFocus() {
  WebFrame* main_frame = webview() ? webview()->MainFrame() : nullptr;
  bool is_processing_user_gesture =
      WebUserGestureIndicator::IsProcessingUserGesture(
          main_frame && main_frame->IsWebLocalFrame()
              ? main_frame->ToWebLocalFrame()
              : nullptr);
   if (is_processing_user_gesture &&
       !RenderThreadImpl::current()->layout_test_mode()) {
     Send(new ViewHostMsg_Focus(GetRoutingID()));
   }
 }
