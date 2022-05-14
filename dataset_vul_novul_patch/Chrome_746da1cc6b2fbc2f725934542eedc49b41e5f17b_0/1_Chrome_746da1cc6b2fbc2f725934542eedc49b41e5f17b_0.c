 void PrintViewManager::RenderFrameDeleted(
     content::RenderFrameHost* render_frame_host) {
   if (render_frame_host == print_preview_rfh_)
    print_preview_state_ = NOT_PREVIEWING;
   PrintViewManagerBase::RenderFrameDeleted(render_frame_host);
 }
