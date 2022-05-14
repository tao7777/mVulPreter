 void PrintViewManager::RenderFrameDeleted(
     content::RenderFrameHost* render_frame_host) {
   if (render_frame_host == print_preview_rfh_)
    PrintPreviewDone();
   PrintViewManagerBase::RenderFrameDeleted(render_frame_host);
 }
