void RenderViewImpl::DidFocus() {
void RenderViewImpl::DidFocus(blink::WebLocalFrame* calling_frame) {
  WebFrame* main_frame = webview() ? webview()->MainFrame() : nullptr;
  bool is_processing_user_gesture =
      WebUserGestureIndicator::IsProcessingUserGesture(
          main_frame && main_frame->IsWebLocalFrame()
              ? main_frame->ToWebLocalFrame()
              : nullptr);
   if (is_processing_user_gesture &&
       !RenderThreadImpl::current()->layout_test_mode()) {
     Send(new ViewHostMsg_Focus(GetRoutingID()));

    // Tattle on the frame that called |window.focus()|.
    RenderFrameImpl* calling_render_frame =
        RenderFrameImpl::FromWebFrame(calling_frame);
    if (calling_render_frame)
      calling_render_frame->FrameDidCallFocus();
   }
 }
